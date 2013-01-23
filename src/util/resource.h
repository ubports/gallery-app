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
 * Eric Gregory <eric@yorba.org>
 */
#ifndef RESOURCE_H
#define RESOURCE_H

#include <QDir>
#include <QString>
#include <QUrl>

class Resource
{
 public:
  explicit Resource(const QString& application_dir, const QString& install_dir);

  /// Returns true if we're installed, false if we're running locally.
  bool is_installed() const;
  
  /// Converts a relative path in the resource folder to a fully-qualified URL.
  /// Do NOT use this function if you need an absolute path.
  QUrl get_rc_url(const QString& path) const;
  
  /*!
   * @brief maxTextureSize
   * @return
   * returns max texture size provided by OpenGL
   */
  int maxTextureSize() const;
  
 private:
  QString trailing_slash(QString path) const;
  
  QDir app_dir_;
  QDir install_dir_;

  friend class tst_Resource;
};

#endif // RESOURCE_H
