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

#include "qml-album-collection-model.h"
#include "album/album.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "core/gallery-manager.h"
#include "core/selectable-view-collection.h"
#include "util/variants.h"

/*!
 * \brief QmlAlbumCollectionModel::QmlAlbumCollectionModel
 * \param parent
 */
QmlAlbumCollectionModel::QmlAlbumCollectionModel(QObject* parent)
    : QmlViewCollectionModel(parent, "album", NULL)
{
    MonitorSourceCollection(GalleryManager::GetInstance()->album_collection());
    QObject::connect(GalleryManager::GetInstance()->album_collection(),
        SIGNAL(album_current_page_contents_altered(Album*)),
        this, SLOT(on_album_current_page_contents_altered(Album*)));
}

/*!
 * \brief QmlAlbumCollectionModel::RegisterType
 */
void QmlAlbumCollectionModel::RegisterType()
{
  qmlRegisterType<QmlAlbumCollectionModel>("Gallery", 1, 0, "AlbumCollectionModel");
}

/*!
 * \brief QmlAlbumCollectionModel::createAlbum
 * \param vmedia
 */
void QmlAlbumCollectionModel::createAlbum(QVariant vmedia)
{
  Album* album = new Album(GalleryManager::GetInstance()->album_default_template());
  album->Attach(VariantToObject<MediaSource*>(vmedia));

  GalleryManager::GetInstance()->album_collection()->Add(album);
}

/*!
 * \brief QmlAlbumCollectionModel::destroyAlbum
 * \param valbum
 */
void QmlAlbumCollectionModel::destroyAlbum(QVariant valbum)
{
  Album* album = VariantToObject<Album*>(valbum);

  if (album != NULL)
    GalleryManager::GetInstance()->album_collection()->Destroy(album, true, true);
}

/*!
 * \brief QmlAlbumCollectionModel::createOrphan
 * \return
 */
QVariant QmlAlbumCollectionModel::createOrphan()
{
  return QVariant::fromValue(new Album(GalleryManager::GetInstance()->album_default_template()));
}

/*!
 * \brief QmlAlbumCollectionModel::destroyOrphan
 * \param valbum
 */
void QmlAlbumCollectionModel::destroyOrphan(QVariant valbum)
{
  Album* album = VariantToObject<Album*>(valbum);

  if (album != NULL) {
    album->DestroyOrphan(true);
    delete album;
  }
}

/*!
 * \brief QmlAlbumCollectionModel::addOrphan
 * \param valbum
 */
void QmlAlbumCollectionModel::addOrphan(QVariant valbum)
{
  Album* album = VariantToObject<Album*>(valbum);

  if (album != NULL)
    GalleryManager::GetInstance()->album_collection()->Add(album);
}

/*!
 * \brief QmlAlbumCollectionModel::VariantFor
 * \param object
 * \return
 */
QVariant QmlAlbumCollectionModel::VariantFor(DataObject* object) const
{
  Album* album = qobject_cast<Album*>(object);

  return (album != NULL) ? QVariant::fromValue(album) : QVariant();
}

/*!
 * \brief QmlAlbumCollectionModel::FromVariant
 * \param var
 * \return
 */
DataObject* QmlAlbumCollectionModel::FromVariant(QVariant var) const
{
  return UncheckedVariantToObject<Album*>(var);
}

/*!
 * \brief QmlAlbumCollectionModel::on_album_current_page_contents_altered
 * \param album
 */
void QmlAlbumCollectionModel::on_album_current_page_contents_altered(Album* album)
{
  NotifyElementAltered(BackingViewCollection()->IndexOf(album), SubclassRole);
}
