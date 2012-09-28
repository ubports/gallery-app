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

#include "core/data-object.h"

#include "util/variants.h"

DataObjectNumber DataObject::next_number_ = 0;

DataObject::DataObject(QObject * parent, const QString& name)
  : QObject(parent), name_(name.toUtf8()), number_(next_number_++) {
}

DataObjectNumber DataObject::number() const {
  return number_;
}

void DataObject::SetInternalName(const QString& name) {
  name_ = name.toUtf8();
}

bool DataObject::equals(QVariant vobject) const {
  // DataObjects are unique and can be compared through identity alone
  return UncheckedVariantToObject<DataObject*>(vobject) == this;
}

const char* DataObject::ToString() const {
  return name_.data();
}
