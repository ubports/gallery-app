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

#ifndef GALLERY_ALBUM_PAGE_H_
#define GALLERY_ALBUM_PAGE_H_

#include <QObject>

#include "album/album-template-page.h"
#include "core/container-source.h"

class AlbumPage : public ContainerSource {
  Q_OBJECT
  
 public:
  AlbumPage(int page_number, AlbumTemplatePage* template_page);
  
  int page_number() const;
  AlbumTemplatePage* template_page() const;
  
 protected:
  virtual void DestroySource(bool destroy_backing);
  
 private:
  int page_number_;
  AlbumTemplatePage* template_page_;
};

#endif  // GALLERY_ALBUM_PAGE_H_
