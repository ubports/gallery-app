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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0

ListModel {
  id: albumCoverList
  
  // Returns an element given the nickname.
  function elementForActionName(name) {
    // TODO: optimize.
    for (var i = 0; i < albumCoverList.count; i++) {
      if (albumCoverList.get(i).action === name)
        return albumCoverList.get(i);
    }
    
    return albumCoverList.get(0); // default
  }
  
  function getDefault() {
    return albumCoverList.get(0);
  }
  
  // A note about the image paths here: the imageFull and imagePreview images
  // are used in qml/Components/AlbumCover.qml (and the actual cover images are
  // only needed there, so they're in qml/Components/img).  So, our path in
  // those cases is relative to the Components directory.  However, the
  // iconFilename is actually used by qml/Capetown/Widgets/SomethingOrOther
  // (and thus its images live in the shared img directory).  So, our path in
  // that case is relative to the Capetown Widgets directory.
  // TODO: fix this so images don't have such strict requirements; maybe make
  // a js utility function to give us the path to any image from wherever it's
  // being used.

  // First item in the list must be the default.
  ListElement {
    title: "Default"
    action: "default"
    imageFull: "img/album-cover-default-large.png"
    imagePreview: "img/album-cover-default.png"
    iconFilename: "../../img/album-cover-default-icon.png"
    hasBottomBorder: true
  }
  
  ListElement {
    title: "Alligator"
    action: "alligator"
    imageFull: "img/album-cover-alligator-large.png"
    imagePreview: "img/album-cover-alligator.png"
    iconFilename: "../../img/album-cover-alligator-icon.png"
    hasBottomBorder: true
  }
  
  ListElement {
    title: "Army"
    action: "army"
    imageFull: "img/album-cover-army-large.png"
    imagePreview: "img/album-cover-army.png"
    iconFilename: "../../img/album-cover-army-icon.png"
    hasBottomBorder: true
  }
  
  ListElement {
    title: "Floral"
    action: "floral"
    imageFull: "img/album-cover-floral-large.png"
    imagePreview: "img/album-cover-floral.png"
    iconFilename: "../../img/album-cover-floral-icon.png"
    hasBottomBorder: true
  }
  
  ListElement {
    title: "Leather"
    action: "leather"
    imageFull: "img/album-cover-leather-large.png"
    imagePreview: "img/album-cover-leather.png"
    iconFilename: "../../img/album-cover-leather-icon.png"
    hasBottomBorder: true
  }
  
  ListElement {
    title: "Leopard"
    action: "leopard"
    imageFull: "img/album-cover-leopard-large.png"
    imagePreview: "img/album-cover-leopard.png"
    iconFilename: "../../img/album-cover-leopard-icon.png"
    hasBottomBorder: true
  }
  
  ListElement {
    title: "Rave"
    action: "rave"
    imageFull: "img/album-cover-rave-large.png"
    imagePreview: "img/album-cover-rave.png"
    iconFilename: "../../img/album-cover-rave-icon.png"
    hasBottomBorder: true
  }
}
