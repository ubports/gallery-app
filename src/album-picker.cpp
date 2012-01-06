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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "album-picker.h"
#include "album-collection.h"

AlbumPicker* AlbumPicker::instance_ = NULL;

AlbumPicker::AlbumPicker(QObject *parent) : QObject(parent) {
  universal_albums_view_ =
      new SelectableViewCollection("AlbumPicker Universal View");
  universal_albums_view_->MonitorDataCollection(AlbumCollection::instance(),
    NULL, false);

  universal_albums_model_ = new QmlAlbumCollectionModel();
  universal_albums_model_->Init(universal_albums_view_);
}

AlbumPicker::~AlbumPicker() {
  delete universal_albums_model_;
  delete universal_albums_view_;
}

AlbumPicker* AlbumPicker::instance() {
  if (instance_ == NULL)
    instance_ = new AlbumPicker();

  return instance_;
}

QmlAlbumCollectionModel* AlbumPicker::universal_albums_model() {
  return universal_albums_model_;
}

Album* AlbumPicker::GetAlbumForIndex(int index) {
  return qobject_cast<Album*>(universal_albums_view_->FindByNumber(index));
}
