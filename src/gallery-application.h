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

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QQuickView>
#include <QElapsedTimer>

class GalleryApplication : public QApplication
{
  Q_OBJECT

 public:
  explicit GalleryApplication(int& argc, char** argv);
  int exec();
  static Q_INVOKABLE int run_command(const QString &cmdline);
  static Q_INVOKABLE bool run_command_detached(const QString &cmdline);
  
 signals:
  void media_loaded();

 private:
  void register_qml();
  void usage(bool error = false);
  void invalid_arg(QString arg);
  void process_args();
  void init_common();
  void create_view();
  void init_collections();

  QHash<QString, QSize> form_factors_;
  QString form_factor_;
  bool is_portrait_;
  int bgu_size_;
  QDir pictures_dir_;
  QQuickView view_;
  bool startup_timer_;
  QElapsedTimer timer_;

 private slots:
  void start_init_collections();
};

#endif // GALLERYAPPLICATION_H
