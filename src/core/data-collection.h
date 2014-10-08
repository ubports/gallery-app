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

#ifndef GALLERY_DATA_COLLECTION_H_
#define GALLERY_DATA_COLLECTION_H_

// core
#include "collections.h"

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QSet>
#include <QString>

class DataObject;

// Defined as a LessThan comparator (return true if a is less than b)
typedef bool (*DataObjectComparator)(DataObject* a, DataObject* b);

/**
  * A DataCollection is a heavyweight, fully signalled collection class.  It is
  * not intended for general use but rather to hold core data structures that
  * are monitored and widely used throughout the system.  Those data structures
  * must inherit from DataObject, which is the only kind of object DataCollection
  * will hold.
  */
class DataCollection : public QObject
{
    Q_OBJECT

signals:
    // fired *before* the DataObjects have been added or removed from the collection
    void contentsAboutToBeChanged(const QSet<DataObject*>* added,
                                  const QSet<DataObject*>* removed);

    // fired *after* the DataObjects have been added or removed from the collection
    void contentsChanged(const QSet<DataObject*>* added,
                          const QSet<DataObject*>* removed,
                          bool notify);

    // fired after the the DataCollection has been reordered due to a new
    // DataObjectComparator being installed; if the new comparator doesn't
    // actually affect the ordering, this signal will still be called
    void orderingChanged();

public:
    static bool defaultDataObjectComparator(DataObject* a, DataObject* b);

    DataCollection(const QString& name);

    int count() const;

    void add(DataObject* object);
    virtual void addMany(const QSet<DataObject*>& objects);

    void remove(DataObject* object, bool notify);
    void removeAt(int index);
    void removeMany(const QSet<DataObject*>& objects, bool notify);
    void clear();

    bool contains(DataObject* object) const;
    bool containsAll(DataCollection* collection) const;
    const QList<DataObject*>& getAll() const;
    const QSet<DataObject*>& getAsSet() const;
    DataObject* getAt(int index) const;
    int indexOf(DataObject* media) const;

    template<class T>
    QList<T> getAllAsType() const {
        return CastListToType<DataObject*, T>(getAll());
    }

    template<class T>
    QSet<T> GetAsSetAsType() const {
        return CastSetToType<DataObject*, T>(getAsSet());
    }

    template <class T>
    T getAtAsType(int index) const {
        return qobject_cast<T>(getAt(index));
    }

    void setComparator(DataObjectComparator comparator);
    DataObjectComparator comparator() const;

    void setInternalName(const QString& name);

    virtual const char* toString() const;

protected:
    virtual void notifyContentsToBeChanged(const QSet<DataObject*>* added,
                                           const QSet<DataObject*>* removed);

    virtual void notifyContentsChanged(const QSet<DataObject*>* added,
                                       const QSet<DataObject*>* removed,
                                       bool notify);

    virtual void notifyOrderingChanged();

private:
    void sanity() const;
    void binaryListInsert(DataObject* object);
    void resort(bool fire_signal);

    QByteArray m_name;
    QList<DataObject*> m_list;
    QSet<DataObject*> m_set;
    DataObjectComparator m_comparator;
};

#endif  // GALLERY_DATA_COLLECTION_H_
