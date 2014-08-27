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

#ifndef GALLERY_QML_EVENT_COLLECTION_MODEL_H_
#define GALLERY_QML_EVENT_COLLECTION_MODEL_H_

#include <QObject>
#include <QVariant>
#include <QtQml>

#include "qml-view-collection-model.h"

class DataObject;

/*!
 * \brief The QmlEventCollectionModel class
 */
class QmlEventCollectionModel : public QmlViewCollectionModel
{
    Q_OBJECT

public:
    QmlEventCollectionModel(QObject* parent = NULL);

protected:
    virtual QVariant toVariant(DataObject *object) const;
    virtual DataObject* fromVariant(QVariant var) const;
    bool isAccepted(DataObject* item);
};

QML_DECLARE_TYPE(QmlEventCollectionModel)

#endif  // GALLERY_QML_EVENT_COLLECTION_MODEL_H_
