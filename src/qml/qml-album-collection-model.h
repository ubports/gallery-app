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

#ifndef GALLERY_QML_ALBUM_COLLECTION_MODEL_H_
#define GALLERY_QML_ALBUM_COLLECTION_MODEL_H_

#include <QObject>
#include <QVariant>
#include <QtDeclarative>

#include "album/album.h"
#include "core/data-object.h"
#include "qml/qml-view-collection-model.h"

class QmlAlbumCollectionModel : public QmlViewCollectionModel {
  Q_OBJECT
  
 public:
  QmlAlbumCollectionModel(QObject* parent = NULL);
  
  static void RegisterType();
  
  Q_INVOKABLE void createAlbum(QVariant vmedia);
  
 protected:
  virtual QVariant VariantFor(DataObject* object) const;
  virtual DataObject* FromVariant(QVariant var) const;
  
 private slots:
  void on_album_current_page_contents_altered(Album* album);
};

QML_DECLARE_TYPE(QmlAlbumCollectionModel)

#endif  // GALLERY_QML_ALBUM_COLLECTION_MODEL_H_
