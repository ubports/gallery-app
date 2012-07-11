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
 * Charles Lindsay <chaz@yorba.org>
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
  // is_left determines whether the page is meant to be displayed on the right
  // or left of a spread.  Final arguments are a list of PageOrientation enums
  // that describe the page's frames (slots) from top to bottom, left to right.
  AlbumTemplatePage(const char* name, const char* qml_rc, bool is_left,
                    int frame_count, ...);
  
  const QString& name() const;
  const QString& qml_rc() const;
  bool is_left() const;
  
  int FrameCount() const;
  int FramesFor(PageOrientation orientation) const;
  const QList<PageOrientation>& Layout() const;
  
 private:
  QString name_;
  QString qml_rc_;
  bool is_left_;
  QList<PageOrientation> layout_;
};

#endif  // GALLERY_ALBUM_TEMPLATE_PAGE_H_
