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

#ifndef GALLERY_CORE_UTILS_H_
#define GALLERY_CORE_UTILS_H_

#include "core/data-collection.h"
#include "core/data-object.h"

// For casting the contents of a DataCollection into a QList of subclass of
// DataObject
template <class T>
QList<T> CastDataCollectionToList(const DataCollection *collection) {
  QList<T> to;
  
  DataObject* from_element;
  foreach (from_element, collection->GetAll()) {
    T to_element = qobject_cast<T>(from_element);
    Q_ASSERT(to_element != NULL);
    
    to.append(to_element);
  }
  
  return to;
}

#endif  // GALLERY_CORE_UTILS_H_
