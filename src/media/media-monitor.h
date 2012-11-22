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

#include <QObject>
#include <QDir>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QTimer>

class MediaMonitor : public QObject
{
 Q_OBJECT
 
 signals:
  void media_item_added(QFileInfo new_item);

 public:
  MediaMonitor(const QDir& target_directory);
  virtual ~MediaMonitor();
  
 private:
  const QDir target_directory_;
  const QFileSystemWatcher watcher_;
  QStringList manifest_;
  QTimer file_activity_timer_;
  
  static QStringList get_manifest(const QDir& dir);
  static bool subtract_manifest(const QStringList& m1, const QStringList& m2,
    QStringList& difference);
  
  void notify_media_item_added(const QString& item_path);

 private slots:
  void on_directory_event(const QString& event_source);
  void on_file_activity_ceased();
};

#endif // GALLERY_MEDIA_MONITOR_H_
