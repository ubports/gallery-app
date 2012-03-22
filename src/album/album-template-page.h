/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 */

#ifndef GALLERY_ALBUM_TEMPLATE_PAGE_H_
#define GALLERY_ALBUM_TEMPLATE_PAGE_H_

#include <QObject>
#include <QList>
#include <QString>
#include <QUrl>

enum PageOrientation {
  PORTRAIT,
  LANDSCAPE,
  SQUARE
};

class AlbumTemplatePage : public QObject {
  Q_OBJECT
  
 public:
  // Final arguments are a list of PageOrientation enums that describe the page's
  // frames (slots) from top to bottom, left to right.
  AlbumTemplatePage(const char* name, const char* left_qml_rc, const char* right_qml_rc,
    int frame_count, ...);
  
  const QString& name() const;
  const QUrl& left_qml_rc() const;
  const QUrl& right_qml_rc() const;
  
  int FrameCount() const;
  int FramesFor(PageOrientation orientation) const;
  const QList<PageOrientation>& Layout() const;
  
 private:
  QString name_;
  QUrl left_qml_rc_;
  QUrl right_qml_rc_;
  QList<PageOrientation> layout_;
};

#endif  // GALLERY_ALBUM_TEMPLATE_PAGE_H_
