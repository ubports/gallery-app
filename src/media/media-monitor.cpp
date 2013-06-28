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

#include <QDir>
#include <QSet>
#include <QString>

/*!
 * \brief MediaMonitor::MediaMonitor
 * \param targetDirectory
 */
MediaMonitor::MediaMonitor(QObject *parent)
    : QObject(parent),
      m_workerThread(this)
{
    m_worker = new MediaMonitorWorker();
    m_worker->moveToThread(&m_workerThread);
    QObject::connect(&m_workerThread, SIGNAL(finished()),
                     m_worker, SLOT(deleteLater()));

    QObject::connect(m_worker, SIGNAL(mediaItemAdded(QString)),
                     this, SIGNAL(mediaItemAdded(QString)), Qt::QueuedConnection);

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
 * \brief MediaMonitor::startMonitoring
 * \param targetDirectories
 */
void MediaMonitor::startMonitoring(const QStringList &targetDirectories)
{
    QMetaObject::invokeMethod(m_worker, "startMonitoring", Qt::QueuedConnection,
                              Q_ARG(QStringList, targetDirectories));
}


/*!
 * \brief MediaMonitor::MediaMonitor
 * \param targetDirectory
 */
MediaMonitorWorker::MediaMonitorWorker(QObject *parent)
    : QObject(parent),
      m_targetDirectories(),
      m_watcher(this),
      m_manifest(),
      m_fileActivityTimer(this)
{
    QObject::connect(&m_watcher, SIGNAL(directoryChanged(const QString&)), this,
                     SLOT(onDirectoryEvent(const QString&)));

    m_fileActivityTimer.setSingleShot(true);
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
 * \brief MediaMonitor::startMonitoring
 * \param targetDirectories
 */
void MediaMonitorWorker::startMonitoring(const QStringList &targetDirectories)
{
    QStringList newDirectories;
    foreach (const QString& dir, targetDirectories) {
        if (!m_targetDirectories.contains(dir))
            newDirectories.append(dir);
    }
    m_targetDirectories += newDirectories;
    m_manifest = getManifest(m_targetDirectories);
    m_watcher.addPaths(newDirectories);
}

/*!
 * \brief MediaMonitor::onDirectoryEvent
 * \param eventSource
 */
void MediaMonitorWorker::onDirectoryEvent(const QString& eventSource)
{
    m_fileActivityTimer.start(100);
}

/*!
 * \brief MediaMonitor::onFileActivityCeased
 */
void MediaMonitorWorker::onFileActivityCeased()
{
    QStringList new_manifest = getManifest(m_targetDirectories);

    QStringList difference = subtractManifest(new_manifest, m_manifest);
    for (int i = 0; i < difference.size(); i++)
        emit mediaItemAdded(difference.at(i));

    m_manifest = new_manifest;
}

/*!
 * \brief MediaMonitor::getManifest
 * \param dir
 * \return
 */
QStringList MediaMonitorWorker::getManifest(const QStringList &dirs)
{
    QStringList allFiles;
    foreach (const QString &dirName, dirs) {
        QDir dir(dirName);
        QStringList fileList = dir.entryList(QDir::Files, QDir::Time);
        foreach (const QString &fileName, fileList) {
            allFiles.append(dirName + QDir::separator() + fileName);
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
