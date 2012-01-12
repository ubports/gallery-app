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

#include "qml/qml-album-collection-model.h"

#include <QHash>
#include <QtDeclarative>

#include "album/album.h"
#include "album/album-collection.h"
#include "media/media-source.h"

QmlAlbumCollectionModel::QmlAlbumCollectionModel(QObject* parent)
  : QmlViewCollectionModel(parent) {
}

void QmlAlbumCollectionModel::RegisterType() {
  qmlRegisterType<QmlAlbumCollectionModel>("org.yorba.qt.qmlalbumcollectionmodel",
    1, 0, "QmlAlbumCollectionModel");
}

void QmlAlbumCollectionModel::Init(SelectableViewCollection* view) {
  QHash<int, QByteArray> roles;
  roles[QmlViewCollectionModel::ObjectNumberRole] = "object_number";
  roles[QmlViewCollectionModel::SelectionRole] = "is_selected";
  roles[AlbumRole] = "album";
  roles[CurrentPageMediaSourceListRole] = "currentPageMediaSourceList";
  
  QmlViewCollectionModel::Init(view, roles);
  
  QObject::connect(
    AlbumCollection::instance(), SIGNAL(album_current_page_contents_altered(Album*)),
    this, SLOT(on_album_current_page_contents_altered(Album*)));
}

QVariant QmlAlbumCollectionModel::DataForRole(DataObject *object, int role) const {
  Album* album = qobject_cast<Album*>(object);
  if (album == NULL)
    return QVariant();
  
  switch (role) {
    case AlbumRole:
      return QVariant::fromValue(album);
    
    case CurrentPageMediaSourceListRole: {
      QList<QVariant> varlist;
      
      AlbumPage* page = album->GetPage(album->current_page());
      if (page == NULL)
        return QVariant(varlist);
      
      DataObject* object;
      foreach (object, page->contained()->GetAll())
        varlist.append(QVariant::fromValue(qobject_cast<MediaSource*>(object)));
      
      return QVariant(varlist);
    }
    
    default:
      return QVariant();
  }
}

void QmlAlbumCollectionModel::on_album_current_page_contents_altered(Album* album) {
  NotifyElementAltered(BackingViewCollection()->IndexOf(album), CurrentPageMediaSourceListRole);
}
