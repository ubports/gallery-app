/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Lucas Beeler <lucas@yorba.org>
 */

#include "media-monitor.h"
#include "media-collection.h"
#include "media-source.h"

#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QSet>
#include <QString>

/*!
 * \brief MediaMonitor::MediaMonitor
 */
MediaMonitor::MediaMonitor(QObject *parent)
    : QObject(parent),
      m_workerThread(this)
{
    m_worker = new MediaMonitorWorker();
    m_worker->moveToThread(&m_workerThread);
    QObject::connect(&m_workerThread, SIGNAL(finished()),
                     m_worker, SLOT(deleteLater()));

    QObject::connect(m_worker, SIGNAL(mediaItemAdded(QString, int)),
                     this, SIGNAL(mediaItemAdded(QString, int)), Qt::QueuedConnection);
    QObject::connect(m_worker, SIGNAL(mediaItemRemoved(qint64)),
                     this, SIGNAL(mediaItemRemoved(qint64)), Qt::QueuedConnection);
    QObject::connect(m_worker, SIGNAL(consistencyCheckFinished()),
                     this, SIGNAL(consistencyCheckFinished()), Qt::QueuedConnection);

    m_workerThread.start(QThread::LowPriority);
}

/*!
 * \brief MediaMonitor::~MediaMonitor
 */
MediaMonitor::~MediaMonitor()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

/*!
 * \brief MediaMonitor::startMonitoring starts monitoring the given directories
 * new and delted files
 * \param targetDirectories
 */
void MediaMonitor::startMonitoring(const QStringList &targetDirectories)
{
    QMetaObject::invokeMethod(m_worker, "startMonitoring", Qt::QueuedConnection,
                              Q_ARG(QStringList, targetDirectories));
}

/*!
 * \brief MediaMonitor::checkConsistency checks the given datastructure, if it is
 * in sync with the file system (files got added, deleted meanwhile)
 * \param mediaCollection
 */
void MediaMonitor::checkConsistency(const MediaCollection *mediaCollection)
{
    m_worker->setMediaCollection(mediaCollection);
    QMetaObject::invokeMethod(m_worker, "checkConsistency", Qt::QueuedConnection);
}

/*!
 * \brief MediaMonitor::setMonitoringOnHold when true, all actions based on file
 * activity are delayed until it's set back to true
 * \param onHold
 */
void MediaMonitor::setMonitoringOnHold(bool onHold)
{
    m_worker->setMonitoringOnHold(onHold);
}

/*!
 * \brief MediaMonitor::manifest return a list of all files found on monitoring process.
 * It is beeing used on unit tests to check if monitoring process is correct.
 */
QStringList MediaMonitor::manifest() {
    return m_worker->getManifest();
}

/*!
 * \brief MediaMonitor::MediaMonitor
 */
MediaMonitorWorker::MediaMonitorWorker(QObject *parent)
    : QObject(parent),
      m_targetDirectories(),
      m_watcher(this),
      m_manifest(),
      m_fileActivityTimer(this),
      m_onHold(false)
{
    QObject::connect(&m_watcher, SIGNAL(directoryChanged(const QString&)), this,
                     SLOT(onDirectoryEvent(const QString&)));

    m_fileActivityTimer.setSingleShot(true);
    m_fileActivityTimer.setInterval(100);
    QObject::connect(&m_fileActivityTimer, SIGNAL(timeout()), this,
                     SLOT(onFileActivityCeased()));
}

/*!
 * \brief MediaMonitor::~MediaMonitor
 */
MediaMonitorWorker::~MediaMonitorWorker()
{
}

/*!
 * \brief MediaMonitorWorker::setMediaCollection
 * \param mediaCollection
 */
void MediaMonitorWorker::setMediaCollection(const MediaCollection *mediaCollection)
{
    m_mediaCollection = mediaCollection;
}

/*!
 * \brief MediaMonitorWorker::setMonitoringOnHold
 * \param onHold
 */
void MediaMonitorWorker::setMonitoringOnHold(bool onHold)
{
    m_onHold = onHold;
}

/*!
 * \brief MediaMonitor::getManifest is a getter for m_manifest
 */
QStringList MediaMonitorWorker::getManifest()
{
    return m_manifest;
}

/*!
 * \brief MediaMonitor::findNewSubDirectories List all sub directories under
 * that are not on the current directories list
 * \param currentDirectories
 */
QStringList MediaMonitorWorker::findNewSubDirectories(const QStringList& currentDirectories)
{
    QStringList newDirectories;
    foreach (const QString& dirPath, currentDirectories) {
        foreach (const QString& d, expandSubDirectories(dirPath)) {
            if (!m_targetDirectories.contains(d)) {
                newDirectories.append(d);
            }
        }
    }
    return newDirectories;
}

/*!
 * \brief MediaMonitor::expandSubDirectories List all sub directories under
 * the base one. Use it to add to watch list
 * \param dirPath
 */
QStringList MediaMonitorWorker::expandSubDirectories(const QString& dirPath)
{
    QStack<QString> dirStack;
    if(QDir(dirPath).exists()) {
        dirStack.push(dirPath);
    }

    QStringList dirList;
    while(!dirStack.isEmpty()) {
        QDir dir(dirStack.pop());
        dirList.append(dir.absolutePath());

        foreach(const QFileInfo &info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs)) {
            QString path(info.absoluteFilePath());
            if (info.isSymLink() && info.exists()) {
                // If it's a SymLink and points to a valid target need to get the target path
                path = info.symLinkTarget();
            }

            QFileInfo pathInfo(path);
            if(!dirList.contains(path) && !pathInfo.isHidden()) {
                // In case we already visited the folder we didn't expand it anymore
                dirStack.push(path);
            }
        }
    }

    return dirList;
}

/*!
 * \brief MediaMonitor::startMonitoring
 * \param targetDirectories
 */
void MediaMonitorWorker::startMonitoring(const QStringList &targetDirectories)
{
    QStringList newDirectories = findNewSubDirectories(targetDirectories);
    m_targetDirectories += newDirectories;
    m_manifest = generateManifest(m_targetDirectories);
    m_watcher.addPaths(newDirectories);
}

/*!
 * \brief MediaMonitorWorker::checkConsistency
 * \param mediaCollection
 */
void MediaMonitorWorker::checkConsistency()
{
    checkForNewMedias();
    emit consistencyCheckFinished();
}

/*!
 * \brief MediaMonitor::onDirectoryEvent
 * \param eventSource
 */
void MediaMonitorWorker::onDirectoryEvent(const QString& eventSource)
{
    m_fileActivityTimer.start();
}

/*!
 * \brief MediaMonitor::onFileActivityCeased
 */
void MediaMonitorWorker::onFileActivityCeased()
{
    if (m_onHold) {
        m_fileActivityTimer.start();
        return;
    }

    QStringList currentDirectories = QStringList(m_targetDirectories);
    QStringList newDirectories = findNewSubDirectories(currentDirectories);

    m_targetDirectories += newDirectories;
    m_watcher.addPaths(newDirectories);

    QStringList new_manifest = generateManifest(m_targetDirectories);

    QStringList added = subtractManifest(new_manifest, m_manifest);
    for (int i = 0; i < added.size(); i++)
        emit mediaItemAdded(added.at(i), Qt::HighEventPriority);

    QStringList removed = subtractManifest(m_manifest, new_manifest);
    for (int i = 0; i < removed.size(); i++) {
        QFileInfo file(removed.at(i));
        const MediaSource *media = m_mediaCollection->mediaFromFileinfo(file);
        if (media)
            emit mediaItemRemoved(media->id());
    }

    m_manifest = new_manifest;
}

/*!
 * \brief MediaMonitor::generateManifest
 * \param dir
 * \return
 */
QStringList MediaMonitorWorker::generateManifest(const QStringList &dirs)
{
    QStringList allFiles;
    foreach (const QString &dirName, dirs) {
        QDir dir(dirName);
        QStringList fileList = dir.entryList(QDir::Files, QDir::Time);
        foreach (const QString &fileName, fileList) {
            const QFileInfo fi(dirName + QDir::separator() + fileName);
            allFiles.append(fi.absoluteFilePath());
        }
    }
    return allFiles;
}

/*!
 * \brief MediaMonitor::subtractManifest
 * \param m1
 * \param m2
 * \return
 */
QStringList MediaMonitorWorker::subtractManifest(const QStringList& m1,
                                            const QStringList& m2)
{
    QSet<QString> result = QSet<QString>::fromList(m1);
    result.subtract(QSet<QString>::fromList(m2));
    return QStringList(result.toList());
}

/*!
 * \brief MediaMonitorWorker::checkForNewMedias checks for files in the filesystem
 * that are not in the datastructure
 * \param mediaCollection
 */
void MediaMonitorWorker::checkForNewMedias()
{
    foreach (const QString& file, m_manifest) {
        if (!m_mediaCollection->containsFile(file))
            emit mediaItemAdded(file, Qt::NormalEventPriority);
    }
}
