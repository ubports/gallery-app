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
#include "album.h"
#include "album-collection.h"
#include "album-default-template.h"
#include "selectable-view-collection.h"
#include "variants.h"
#include "gallery-manager.h"

/*!
 * \brief QmlAlbumCollectionModel::QmlAlbumCollectionModel
 * \param parent
 */
QmlAlbumCollectionModel::QmlAlbumCollectionModel(QObject* parent)
    : QmlViewCollectionModel(parent, "album", NULL)
{
    monitorSourceCollection(GalleryManager::instance()->album_collection());
    QObject::connect(GalleryManager::instance()->album_collection(),
                     SIGNAL(albumCurrentPageContentsChanged(Album*)),
                     this, SLOT(onAlbumCurrentPageContentsChanged(Album*)));
}

/*!
 * \brief QmlAlbumCollectionModel::createAlbum
 * \param vmedia
 */
void QmlAlbumCollectionModel::createAlbum(QVariant vmedia)
{
    Album* album = new Album(GalleryManager::instance()->album_default_template());
    album->attach(VariantToObject<MediaSource*>(vmedia));

    GalleryManager::instance()->album_collection()->add(album);
}

/*!
 * \brief QmlAlbumCollectionModel::destroyAlbum
 * \param valbum
 */
void QmlAlbumCollectionModel::destroyAlbum(QVariant valbum)
{
    Album* album = VariantToObject<Album*>(valbum);

    if (album != NULL)
        GalleryManager::instance()->album_collection()->destroy(album, true, true);
}

/*!
 * \brief QmlAlbumCollectionModel::createOrphan
 * \return
 */
QVariant QmlAlbumCollectionModel::createOrphan()
{
    return QVariant::fromValue(new Album(GalleryManager::instance()->album_default_template()));
}

/*!
 * \brief QmlAlbumCollectionModel::destroyOrphan
 * \param valbum
 */
void QmlAlbumCollectionModel::destroyOrphan(QVariant valbum)
{
    Album* album = VariantToObject<Album*>(valbum);

    if (album != NULL) {
        album->destroyOrphan(true);
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
        GalleryManager::instance()->album_collection()->add(album);
}

/*!
 * \brief QmlAlbumCollectionModel::toVariant
 * \param object
 * \return
 */
QVariant QmlAlbumCollectionModel::toVariant(DataObject* object) const
{
    Album* album = qobject_cast<Album*>(object);

    return (album != NULL) ? QVariant::fromValue(album) : QVariant();
}

/*!
 * \brief QmlAlbumCollectionModel::fromVariant
 * \param var
 * \return
 */
DataObject* QmlAlbumCollectionModel::fromVariant(QVariant var) const
{
    return UncheckedVariantToObject<Album*>(var);
}

/*!
 * \brief QmlAlbumCollectionModel::onAlbumCurrentPageContentsChanged
 * \param album
 */
void QmlAlbumCollectionModel::onAlbumCurrentPageContentsChanged(Album* album)
{
    notifyElementChanged(backingViewCollection()->indexOf(album), SubclassRole);
}
