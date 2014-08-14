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

#ifndef GALLERY_QML_MEDIA_COLLECTION_MODEL_H_
#define GALLERY_QML_MEDIA_COLLECTION_MODEL_H_

#include <QObject>
#include <QVariant>
#include <QtQml>

#include "qml-view-collection-model.h"

class DataObject;

/*!
 * \brief The QmlMediaCollectionModel class
 */
class QmlMediaCollectionModel : public QmlViewCollectionModel
{
    Q_OBJECT
    Q_PROPERTY(bool monitored READ monitored WRITE setMonitored
               NOTIFY monitoringChanged)

signals:
    void monitoringChanged();

public:
    QmlMediaCollectionModel(QObject* parent = NULL);
    QmlMediaCollectionModel(QObject* parent, DataObjectComparator defaultComparator);

    Q_INVOKABLE QVariant createAlbumFromSelected();
    Q_INVOKABLE void destroySelectedMedia();
    Q_INVOKABLE void destroyMedia(QVariant vmedia, bool destroy_backing);
    Q_INVOKABLE void removeMediaFromAlbum(QVariant valbum, QVariant vmedia);

    bool monitored() const;
    void setMonitored(bool monitor);
    bool isAccepted(DataObject *item);

protected:
    virtual QVariant toVariant(DataObject* object) const;
    virtual DataObject* fromVariant(QVariant var) const;
};

QML_DECLARE_TYPE(QmlMediaCollectionModel)

#endif  // GALLERY_QML_MEDIA_COLLECTION_MODEL_H_
