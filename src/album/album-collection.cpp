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

#include "album/album-collection.h"

AlbumCollection *AlbumCollection::instance_ = NULL;

AlbumCollection::AlbumCollection()
  : ContainerSourceCollection("AlbumCollection") {
}

AlbumCollection* AlbumCollection::instance() {
  if (instance_ == NULL)
    instance_ = new AlbumCollection();
  
  return instance_;
}

void AlbumCollection::notify_album_current_page_contents_altered(Album* album) {
  emit album_current_page_contents_altered(album);
}
