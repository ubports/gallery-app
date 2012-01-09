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

#ifndef GALLERY_ALBUM_PICKER_H_
#define GALLERY_ALBUM_PICKER_H_

#include <QObject>

#include "core/selectable-view-collection.h"
#include "qml/qml-album-collection-model.h"

class AlbumPicker : public QObject {
  Q_OBJECT

 public:
  static AlbumPicker* instance();

  virtual ~AlbumPicker();

  QmlAlbumCollectionModel* universal_albums_model();

  Album* GetAlbumForIndex(int index);

 private:
  explicit AlbumPicker(QObject *parent = NULL);

  static AlbumPicker* instance_;

  QmlAlbumCollectionModel* universal_albums_model_;
  SelectableViewCollection* universal_albums_view_;
};

#endif // GALLERY_ALBUM_PICKER_H_
