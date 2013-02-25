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

#ifndef GALLERY_UTIL_VARIANTS_H_
#define GALLERY_UTIL_VARIANTS_H_

#include <QObject>
#include <QVariant>

// For casting a QVariant to a QObject-based object.
//
// NOTE: This uses Q_ASSERT to verify that the QVariant properly casts.
template <class T>
T VariantToObject(QVariant var)
{
    QObject* obj = qvariant_cast<QObject*>(var);
    Q_ASSERT(obj != NULL);

    T to = qobject_cast<T>(obj);
    Q_ASSERT(to != NULL);

    return to;
}

// Like VariantToObject, but no assertions (returns NULL if not a proper
// cast)
template <class T>
T UncheckedVariantToObject(QVariant var)
{
    QObject* obj = qvariant_cast<QObject*>(var);

    return (obj != NULL) ? qobject_cast<T>(obj) : NULL;
}

#endif  // GALLERY_UTIL_VARIANTS_H_
