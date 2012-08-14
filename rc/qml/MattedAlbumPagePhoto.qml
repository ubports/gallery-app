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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1

CompositeMattedPhoto {
  id: mattedAlbumPagePhoto

  backingBorderImage: "../img/album-page-frame-border.sci"
  insideShadowBorderImage: "../img/album-page-frame-inside-shadow-border.sci"

  backingShadowPixelsLeft: 2
  backingShadowPixelsRight: 2
  backingShadowPixelsTop: 2
  backingShadowPixelsBottom: 2

  backingFramePixelsLeft: 5
  backingFramePixelsRight: 5
  backingFramePixelsTop: 5
  backingFramePixelsBottom: 5
}
