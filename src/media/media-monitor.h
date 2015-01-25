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

#ifndef GALLERY_MEDIA_MONITOR_H_
#define GALLERY_MEDIA_MONITOR_H_

#include <QFileSystemWatcher>
#include <QObject>
#include <QStringList>
#include <QThread>
#include <QTimer>

class MediaCollection;
class MediaMonitorWorker;

/*!
 * \brief The MediaMonitor class monitor directories for added files. And does a
 * check if the files in the datastructure and on thef ile system are in sync.
 * All this is done in an extra thread.
 */
class MediaMonitor : public QObject
{
    Q_OBJECT

public:
    MediaMonitor(QObject *parent=0);
    virtual ~MediaMonitor();

    void startMonitoring(const QStringList& targetDirectories);
    void checkConsistency(const MediaCollection *mediaCollection);
    QStringList manifest();

public slots:
    void setMonitoringOnHold(bool onHold);

signals:
    void mediaItemAdded(QString newItem, int priority);
    void mediaItemRemoved(qint64 mediaId);
    void consistencyCheckFinished();

private:
    MediaMonitorWorker* m_worker;
    QThread m_workerThread;
};


/*!
 * \brief The MediaMonitorWorker class does the actucal fiel monitoring, but is
 * supposed to to it in a thread
 */
class MediaMonitorWorker : public QObject
{
    Q_OBJECT

public:
    MediaMonitorWorker(QObject *parent=0);
    virtual ~MediaMonitorWorker();

    void setMediaCollection(const MediaCollection *mediaCollection);
    void setMonitoringOnHold(bool onHold);
    QStringList getManifest();

public slots:
    void startMonitoring(const QStringList& targetDirectories);
    QStringList findNewSubDirectories(const QStringList& currentDirectories);
    QStringList expandSubDirectories(const QString& dirPath);
    void checkConsistency();

signals:
    void mediaItemAdded(QString newItem, int priority);
    void mediaItemRemoved(qint64 mediaId);
    void consistencyCheckFinished();

private slots:
    void onDirectoryEvent(const QString& eventSource);
    void onFileActivityCeased();

private:
    QStringList generateManifest(const QStringList& dirs);
    QStringList subtractManifest(const QStringList& m1, const QStringList& m2);
    void checkForNewMedias();

    QStringList m_targetDirectories;
    QFileSystemWatcher m_watcher;
    QStringList m_manifest;
    QTimer m_fileActivityTimer;
    const MediaCollection *m_mediaCollection;
    bool m_onHold;
};

#endif // GALLERY_MEDIA_MONITOR_H_
