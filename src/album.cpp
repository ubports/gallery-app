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

#include "album.h"

#include "album-collection.h"
#include "media-source.h"

const char *Album::DEFAULT_NAME = "New Photo Album";

Album::Album() :
  name_(DEFAULT_NAME), current_page_(1) {
}

Album::Album(const QString& name) :
  name_(name) {
}

const QString& Album::name() const {
  return name_;
}

int Album::current_page() const {
  return current_page_;
}

const QList<MediaSource*>& Album::current_page_contents() const {
  return current_page_contents_;
}

void Album::notify_current_page_contents_altered() {
  emit current_page_contents_altered();
  
  // Don't call AlbumCollection::instance directly -- it's possible the
  // object is orphaned
  AlbumCollection* membership = qobject_cast<AlbumCollection*>(member_of());
  if (membership != NULL)
    membership->notify_album_current_page_contents_altered(this);
}

void Album::DestroySource(bool destroy_backing) {
  // TODO: Remove album entry in database
}

void Album::notify_container_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  ContainerSource::notify_container_contents_altered(added, removed);
  
  // TODO: Can be smarter than this, but since we don't know how position(s)
  // in the contained sources list have now changed, need to reset and start
  // afresh ... in the future, this could simply be optimized to see if the
  // elements on the current page have changed
  current_page_contents_.clear();
  for (int ctr = 0; ctr < 2; ctr++) {
    MediaSource* media = qobject_cast<MediaSource*>(contained()->GetAt(ctr));
    if (media != NULL)
      current_page_contents_.append(media);
  }
  
  notify_current_page_contents_altered();
}
