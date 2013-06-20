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

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QObject>
#include <QStringList>
#include <QTimer>

/*!
 * \brief The MediaMonitor class
 */
class MediaMonitor : public QObject
{
    Q_OBJECT

signals:
    void mediaItemAdded(QFileInfo newItem);

public:
    MediaMonitor(const QStringList& targetDirectories);
    virtual ~MediaMonitor();

private slots:
    void onDirectoryEvent(const QString& eventSource);
    void onFileActivityCeased();

private:
    static QStringList getManifest(const QStringList& dirs);
    static QStringList subtractManifest(const QStringList& m1,
                                        const QStringList& m2);

    void notifyMediaItemAdded(const QString& itemPath);

    QStringList m_targetDirectories;
    QFileSystemWatcher m_watcher;
    QStringList m_manifest;
    QTimer m_fileActivityTimer;
};

#endif // GALLERY_MEDIA_MONITOR_H_
