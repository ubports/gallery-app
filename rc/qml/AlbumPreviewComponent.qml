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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

Item {
  id: albumPreviewComponent

  property Album album
  property alias openFraction: albumPreview.openFraction

  // readonly
  property alias isFlipping: albumPreview.isFlipping

  function open() {
    return albumPreview.open();
  }

  function close() {
    return albumPreview.close();
  }

  width: gu(28)
  height: gu(33)

  Column {
    
    AlbumOpener {
      id: albumPreview

      // Yes, the page takes up the entire region of the preview component.
      // The text below spills out into the gutter.  That's by design.
      width: albumPreviewComponent.width
      height: albumPreviewComponent.height

      album: albumPreviewComponent.album
      isPreview: true
      contentHasPreviewFrame: true
    }

    // Spacer
    Item {
      width: 1
      height: gu(1)
    }
    
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      
      text: (album) ? album.title : ""
      color: "#818285"
      font.family: "Ubuntu"
      font.pointSize: pointUnits(8) // From the spec.
      smooth: true

      visible: (Boolean(album) && albumPreview.openFraction == 1)
    }
    
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      
      text: (album) ? album.subtitle : ""
      color: "#a7a9ac"
      font.family: "Ubuntu"
      font.pointSize: pointUnits(6) // From the spec.
      smooth: true

      visible: (Boolean(album) && albumPreview.openFraction == 1)
    }
  }
}
