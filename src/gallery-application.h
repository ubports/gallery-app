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
#include <QQuickView>

class GalleryApplication : public QApplication
{
  Q_OBJECT

 public:
  static const int GRIDUNIT = 8;

  explicit GalleryApplication(int& argc, char** argv);
  int exec();
  
 signals:
  void media_loaded();

 private:
  void register_qml();
  void process_args();
  void init_common();
  void create_view();
  void init_collections();

  QHash<QString, QSize> form_factors_;
  QString form_factor_;
  bool is_portrait_;
  QDir pictures_dir_;
  QQuickView view_;

 private slots:
  void start_init_collections();
};

#endif // GALLERYAPPLICATION_H
