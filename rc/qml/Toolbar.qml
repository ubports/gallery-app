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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1

Rectangle {
  property string background: "white"
  property bool isTranslucent: false;

  width: parent.width
  height: 48

  color: {
    if (background == "darkBlue") {
      return "#2384c6"; /* used for Edit Photo toolbar only */
    } else if (background == "lightBlue") {
      return "#adc5e6"; /* used for inside album view, photo overview, and
                           album overview, when content is selected */
    } else if (background == "mediumBlue") {
      return "#95b5de"; /* used for most navigation bars */
    } else {
      return "white";   /* used for single photo/video view and album view */
    }
  }

  opacity: (isTranslucent) ? 0.9 : 1.0
}
