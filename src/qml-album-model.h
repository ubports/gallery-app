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

#ifndef GALLERY_ALBUM_MODEL_H_
#define GALLERY_ALBUM_MODEL_H_

#include <QObject>
#include <QVariant>

#include "album.h"
#include "data-object.h"
#include "qml-view-collection-model.h"
#include "selectable-view-collection.h"

class QmlAlbumModel : public QmlViewCollectionModel {
  Q_OBJECT
  
 public:
  enum Role {
    PreviewAPathRole = QmlViewCollectionModel::LastCommonRole,
    PreviewBPathRole,
    NameRole,
  };
  
  explicit QmlAlbumModel(QObject* parent);
  
  static void RegisterType();
  
  // Init() required because QmlAlbumModel is a QML Declarative Type which
  // has restrictions on its ctor signature
  void Init(SelectableViewCollection* view);
  
 protected:
  virtual QVariant DataForRole(DataObject* object, int role) const;
  
 private slots:
  void on_album_current_page_contents_altered(Album* album);
};


#endif  // GALLERY_ALBUM_MODEL_H_
