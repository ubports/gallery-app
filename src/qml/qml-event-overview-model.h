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

#ifndef GALLERY_QML_EVENT_OVERVIEW_MODEL_H_
#define GALLERY_QML_EVENT_OVERVIEW_MODEL_H_

#include <QObject>
#include <QDateTime>
#include <QSet>
#include <QVariant>
#include <QtQml>

#include "qml-media-collection-model.h"

class DataObject;

/*!
 * \brief The QmlEventOverviewModel class
 */
class QmlEventOverviewModel : public QmlMediaCollectionModel
{
    Q_OBJECT
    Q_PROPERTY(bool ascending READ ascendingOrder WRITE setAscendingOrder
               NOTIFY ascendingChanged)

signals:
    void ascendingChanged();

public:
    QmlEventOverviewModel(QObject* parent = NULL);

    bool ascendingOrder() const;
    void setAscendingOrder(bool ascending);
    bool isAccepted(DataObject* item);

protected:
    virtual void notifyBackingCollectionChanged();

    virtual QVariant toVariant(DataObject *object) const;
    virtual DataObject* fromVariant(QVariant var) const;

private slots:
    void onEventsChanged(const QSet<DataObject*>* added,
                         const QSet<DataObject*>* removed,
                         bool notify);
    void onEventOverviewContentsChanged(const QSet<DataObject*>* added,
                                        const QSet<DataObject*>* removed,
                                        bool notify);
    void onEventOverviewSelectionChanged(const QSet<DataObject*>* selected,
                                         const QSet<DataObject*>* unselected);

private:
    static bool ascendingComparator(DataObject* a, DataObject* b);
    static bool descendingComparator(DataObject* a, DataObject* b);
    static bool eventComparator(DataObject* a, DataObject* b, bool desc);
    static QDateTime objectDateTime(DataObject* object, bool desc);

    void monitorNewViewCollection();
    void syncSelectedMedia(const QSet<DataObject*>* toggled, bool selected);

    bool m_ascendingOrder;
    bool m_syncingMedia;
};

QML_DECLARE_TYPE(QmlEventOverviewModel)

#endif  // GALLERY_QML_EVENT_OVERVIEW_MODEL_H_
