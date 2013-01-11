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

#include <QObject>
#include <QDir>
#include <QUrl>

class Resource : public QObject {
  Q_OBJECT
  
 public:
  explicit Resource(const QString& application_dir, const QString& install_dir, QObject* parent = 0);

  // Returns true if we're installed, false if we're running locally.
  bool is_installed();
  
  // Converts a relative path in the resource folder to a fully-qualified URL.
  // Do NOT use this function if you need an absolute path.
  QUrl get_rc_url(const QString& path);
  
  // Same as get_rc_url(), but returns a QDir.
  QDir get_rc_dir(const QString& path);

  /**
   * @brief maxTextureSize
   * @return
   * returns max texture size provided by OpenGL
   */
  int maxTextureSize() const;
  
 private:
  QString trailing_slash(QString path);
  
  QDir app_dir_;
  QDir install_dir_;
};

#endif // RESOURCE_H
