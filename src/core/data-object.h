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

#ifndef GALLERY_DATA_OBJECT_H_
#define GALLERY_DATA_OBJECT_H_

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QVariant>

typedef int DataObjectNumber;

class DataObject : public QObject {
  Q_OBJECT
  
 public:
  DataObject(QObject * parent = 0, const QString& name = "");
  
  // TODO: number() should return the same value for the same DataObject across
  //       invocations of Gallery. Right now, this API contract is maintained
  //       implicitly and in a particularly fragile way. We should fix this.
  //       See https://bugs.launchpad.net/goodhope/+bug/1087084.
  DataObjectNumber number() const;
  void SetInternalName(const QString& name);
  
  Q_INVOKABLE bool equals(QVariant vobject) const;
  
  virtual const char* ToString() const;
  
 private:
  static DataObjectNumber next_number_;
  
  QByteArray name_;
  DataObjectNumber number_;
};

#endif  // GALLERY_DATA_OBJECT_H_
