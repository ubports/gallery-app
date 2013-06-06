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

#ifndef GALLERY_UTIL_COLLECTIONS_H_
#define GALLERY_UTIL_COLLECTIONS_H_

#include <QList>
#include <QObject>
#include <QSet>

template <class F, class T>
QList<T> CastListToType(const QList<F>& from)
{
    QList<T> to;

    F from_element;
    foreach (from_element, from) {
        T to_element = qobject_cast<T>(from_element);
        Q_ASSERT(to_element != NULL);

        to.append(to_element);
    }

    return to;
}

template <class F, class T>
QSet<T> CastSetToType(const QSet<F>& from)
{
    QSet<T> to;

    F from_element;
    foreach (from_element, from) {
        T to_element = qobject_cast<T>(from_element);
        Q_ASSERT(to_element != NULL);

        to.insert(to_element);
    }

    return to;
}

template <class T, class A>
QSet<T> FilterSetOnlyType(const QSet<T>& from)
{
    QSet<T> to;

    T element;
    foreach (element, from) {
        A casted_element = qobject_cast<A>(element);
        if (casted_element != NULL)
            to.insert(element);
    }

    return to;
}

template <class T, class A>
QList<T> FilterListOnlyType(const QList<T>& from)
{
    QList<T> to;

    T element;
    foreach (element, from) {
        A casted_element = qobject_cast<A>(element);
        if (casted_element != NULL)
            to.append(element);
    }

    return to;
}

#endif  // GALLERY_UTIL_COLLECTIONS_H_
