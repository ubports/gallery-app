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

#ifndef GALLERY_ALBUM_TEMPLATE_H_
#define GALLERY_ALBUM_TEMPLATE_H_

#include <QObject>
#include <QList>
#include <QString>

#include "album/album-template-page.h"

class AlbumTemplate : public QObject {
  Q_OBJECT
  
 public:
  int page_count() const;
  const QList<AlbumTemplatePage*>& pages() const;
  
 protected:
  explicit AlbumTemplate(const char* name);
  
  // AlbumTemplate assumes ownership of the AlbumTemplatePage at this point
  // and will delete it when destroyed
  void AddPage(AlbumTemplatePage* page);
  
 private:
  QString name_;
  QList<AlbumTemplatePage*> pages_;
};

#endif  // GALLERY_ALBUM_TEMPLATE_H_
