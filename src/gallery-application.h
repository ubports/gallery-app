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
 * Charles Lindsay <chaz@yorba.org>
 */

#ifndef GALLERYAPPLICATION_H
#define GALLERYAPPLICATION_H

#include "media/media-monitor.h"

#include <QApplication>
#include <QQmlEngine>
#include <QProcess>
#include <QQuickView>
#include <QElapsedTimer>
#include <QFileInfo>

class GalleryManager;

class GalleryApplication : public QApplication {
  Q_OBJECT

 public:
  explicit GalleryApplication(int& argc, char** argv);
  virtual ~GalleryApplication();
  
  int exec();
  
  static GalleryApplication* instance();
  
  // register objects' ownership (QML/Javascript vs. C++)
  void setObjectOwnership(QObject* object, QQmlEngine::ObjectOwnership ownership);

  // Used for content sharing.
  Q_INVOKABLE bool run_command(const QString &cmd, const QString &arg);

  QHash<QString, QSize>* form_factors() { return &form_factors_; }
  QString* form_factor() { return &form_factor_; }

 private:
  void register_qml();
  void process_args();
  void create_view();
  void init_collections();
  
  QHash<QString, QSize> form_factors_;
  QString form_factor_;
  int bgu_size_;
  QQuickView view_;
  QElapsedTimer timer_;
  MediaMonitor* monitor_;

 private slots:
  void start_init_collections();
  void on_media_item_added(QFileInfo item_info);

signals:
  void media_loaded();
};

#endif // GALLERYAPPLICATION_H
