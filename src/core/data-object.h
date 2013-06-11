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

#ifndef GALLERY_DATA_OBJECT_H_
#define GALLERY_DATA_OBJECT_H_

#include <QObject>

typedef int DataObjectNumber;

/**
  * A DataObject is the base class for the primary and core data structures in
  * the system.  DataObjects are held and maintained by DataCollections.
  * Like DataCollection, DataObject is fully signalled so all subsystems may
  * monitor it, thus, it's not appropriate as a subclass for secondary and
  * tertiary classes.
  *
  * DataObjects by contract should not be copied.  Thus, when determining
  * equality of two DataObjects, identity should be sufficient.
  *
  * Each DataObject is assigned a DataObjectNumber which may be used to
  * identify the DataObject in memory if, for whatever reason, the number may
  * be used to index it out of the DataCollection it belongs to.  However,
  * the DataObjectNumber should not be persisted outside the application as it
  * may change from session to session.
  */
class DataObject : public QObject
{
    Q_OBJECT

public:
    DataObject(QObject * parent = 0);

    DataObjectNumber number() const;

private:
    static DataObjectNumber m_nextNumber;

    DataObjectNumber m_number;
};

#endif  // GALLERY_DATA_OBJECT_H_
