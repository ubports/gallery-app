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

#ifndef GALLERY_UTIL_TIME_H_
#define GALLERY_UTIL_TIME_H_

#include <QDate>

// Hopefully will be supplanted in future version of Qt.  See:
// https://bugreports.qt.nokia.com/browse/QTBUG-23079
uint qHash(const QDate& date);

#endif  // GALLERY_UTIL_TIME_H_
