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
 * Jim Nelson <jim@yorba.org>
 */

#ifndef GALLERY_QML_ALBUM_PAGE_MODEL_H_
#define GALLERY_QML_ALBUM_PAGE_MODEL_H_

#include <QObject>
#include <QVariant>
#include <QtDeclarative>

#include "album/album.h"
#include "core/data-object.h"
#include "qml/qml-view-collection-model.h"

class QmlAlbumPageModel : public QmlViewCollectionModel {
  Q_OBJECT
  Q_PROPERTY(QVariant forAlbum READ for_album WRITE set_for_album NOTIFY album_changed)
  Q_PROPERTY(bool includeCover READ include_cover WRITE set_include_cover
    NOTIFY include_cover_changed)
  
 signals:
  void album_changed();
  void include_cover_changed();
  
 public:
  QmlAlbumPageModel(QObject* parent = NULL);
  
  static void RegisterType();
  
  QVariant for_album() const;
  void set_for_album(QVariant valbum);
  
  bool include_cover() const;
  void set_include_cover(bool include_cover);
  
 protected:
  virtual QVariant VariantFor(DataObject* object) const;
  
 private:
  Album* album_;
  bool include_cover_;
  
  static bool Comparator(DataObject* a, DataObject* b);
};

QML_DECLARE_TYPE(QmlAlbumPageModel)

#endif  // GALLERY_QML_ALBUM_PAGE_MODEL_H_
