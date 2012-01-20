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

#include "util/time.h"

uint qHash(const QDate& date) {
  // although all years don't have 366 days, this function isn't truly trying
  // to determine the exact day since epoch, but rather calculate a unique day
  // for each possible QDate ... using 366 because that's the max
  // value dayOfYear() would ever return for any year, so this avoids collisions
  // (for example, Year 1 Day 366 == Year 2 Day 1 if 365 is used as the
  // multiplier)
  return (date.year() * 366) + date.dayOfYear();
}
