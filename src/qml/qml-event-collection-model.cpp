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

#include "qml-event-collection-model.h"
#include "event/event.h"
#include "event/event-collection.h"
#include "util/variants.h"
#include "gallery-manager.h"

/*!
 * \brief QmlEventCollectionModel::QmlEventCollectionModel
 * \param parent
 */
QmlEventCollectionModel::QmlEventCollectionModel(QObject* parent)
    : QmlViewCollectionModel(parent, "event", NULL)
{
    MonitorSourceCollection(GalleryManager::instance()->event_collection());
}

/*!
 * \brief QmlEventCollectionModel::RegisterType
 */
void QmlEventCollectionModel::RegisterType()
{
    qmlRegisterType<QmlEventCollectionModel>("Gallery", 1, 0, "EventCollectionModel");
}

/*!
 * \brief QmlEventCollectionModel::VariantFor
 * \param object
 * \return
 */
QVariant QmlEventCollectionModel::VariantFor(DataObject *object) const
{
    Event* event = qobject_cast<Event*>(object);

    return (event != NULL) ? QVariant::fromValue(event) : QVariant();
}

/*!
 * \brief QmlEventCollectionModel::FromVariant
 * \param var
 * \return
 */
DataObject* QmlEventCollectionModel::FromVariant(QVariant var) const
{
    return UncheckedVariantToObject<Event*>(var);
}
