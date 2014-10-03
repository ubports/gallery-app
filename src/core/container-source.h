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

#ifndef GALLERY_CONTAINER_SOURCE_H_
#define GALLERY_CONTAINER_SOURCE_H_

#include "data-object.h"
#include "data-source.h"
#include "view-collection.h"

#include <QObject>
#include <QSet>

/**
  * A ContainerSource is a DataSource that holds other DataSources.  Generally
  * these are used for organizational features.
  */
class ContainerSource : public DataSource
{
    Q_OBJECT
    Q_PROPERTY(int containedCount READ containedCount NOTIFY containerContentsChanged)

signals:
    void containerContentsChanged(const QSet<DataObject*>* added,
                                  const QSet<DataObject*>* removed);

public:
    ContainerSource(QObject * parent, const QString &name, DataObjectComparator comparator);

    void attach(DataObject* object);
    void attachMany(const QSet<DataObject*>& objects);

    void detach(DataObject* object, bool notify);
    void detachMany(const QSet<DataObject*>& objects);

    bool contains(DataObject* object) const;
    bool containsAll(ContainerSource* collection) const;
    int containedCount() const;
    const ViewCollection* contained() const;

protected:
    virtual void notifyContainerContentsChanged(const QSet<DataObject*>* added,
                                                const QSet<DataObject*>* removed);

private slots:
    void onContentsChanged(const QSet<DataObject*>* added,
                           const QSet<DataObject*>* removed,
                           bool notify);

private:
    ViewCollection m_contained;
};

#endif  // GALLERY_CONTAINER_SOURCE_H_
