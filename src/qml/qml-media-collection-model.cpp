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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "qml-media-collection-model.h"

#include "album.h"
#include "album-collection.h"
#include "album-default-template.h"
#include "data-object.h"
#include "selectable-view-collection.h"
#include "media-source.h"
#include "media-collection.h"
#include "variants.h"
#include "gallery-manager.h"

/*!
 * \brief QmlMediaCollectionModel::QmlMediaCollectionModel
 * \param parent
 */
QmlMediaCollectionModel::QmlMediaCollectionModel(QObject* parent)
    : QmlViewCollectionModel(parent, "mediaSource", NULL)
{
}

/*!
 * \brief QmlMediaCollectionModel::QmlMediaCollectionModel
 * \param parent
 * \param default_comparator
 */
QmlMediaCollectionModel::QmlMediaCollectionModel(QObject* parent,
                                                 DataObjectComparator default_comparator)
    : QmlViewCollectionModel(parent, "mediaSource", default_comparator)
{
}

/*!
 * \brief QmlMediaCollectionModel::createAlbumFromSelected
 * \return
 */
QVariant QmlMediaCollectionModel::createAlbumFromSelected()
{
    SelectableViewCollection* view = backingViewCollection();
    if (view->selectedCount() == 0)
        return QVariant();

    Album* album = new Album(GalleryManager::instance()->albumDefaultTemplate());

    // Attach only the MediaSources in the backing collection
    album->attachMany(FilterSetOnlyType<DataObject*, MediaSource*>(view->getSelected()));

    GalleryManager::instance()->albumCollection()->add(album);

    return QVariant::fromValue(album);
}

/*!
 * \brief QmlMediaCollectionModel::destroySelectedMedia
 */
void QmlMediaCollectionModel::destroySelectedMedia()
{
    SelectableViewCollection* view = backingViewCollection();
    if (view->selectedCount() == 0)
        return;

    GalleryManager::instance()->mediaCollection()->destroyMany(
                FilterSetOnlyType<DataObject*, MediaSource*>(view->getSelected()),
                true, true);
}

/*!
 * \brief QmlMediaCollectionModel::destroyMedia
 * \param vmedia
 */
void QmlMediaCollectionModel::destroyMedia(QVariant vmedia)
{
    MediaSource* media = VariantToObject<MediaSource*>(vmedia);

    if (media != NULL)
        GalleryManager::instance()->mediaCollection()->destroy(media, true, true);
}

/*!
 * \brief QmlMediaCollectionModel::monitored
 * \return
 */
bool QmlMediaCollectionModel::monitored() const
{
    return isMonitoring();
}

/*!
 * \brief QmlMediaCollectionModel::setMonitored
 * \param monitor
 */
void QmlMediaCollectionModel::setMonitored(bool monitor)
{
    if (isMonitoring() == monitor)
        return;

    if (monitor)
        monitorSourceCollection(GalleryManager::instance()->mediaCollection());
    else
        stopMonitoring();

    monitoringChanged();
}

/*!
 * \brief QmlMediaCollectionModel::toVariant
 * \param object
 * \return
 */
QVariant QmlMediaCollectionModel::toVariant(DataObject* object) const
{
    MediaSource* media_source = qobject_cast<MediaSource*>(object);

    return (media_source != NULL) ? QVariant::fromValue(media_source) : QVariant();
}

/*!
 * \brief QmlMediaCollectionModel::fromVariant
 * \param var
 * \return
 */
DataObject* QmlMediaCollectionModel::fromVariant(QVariant var) const
{
    return UncheckedVariantToObject<MediaSource*>(var);
}
