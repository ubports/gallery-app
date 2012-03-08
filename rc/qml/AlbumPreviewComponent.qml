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

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  id: albumPreviewComponent
  
  property Album album
  property alias bookmarkOpacity: albumPageViewer.bookmarkOpacity
  property alias nameHeight: textContainer.height

  property alias topGutter: albumPageViewer.topGutter
  property alias bottomGutter: albumPageViewer.bottomGutter
  property alias leftGutter: albumPageViewer.leftGutter
  property alias rightGutter: albumPageViewer.rightGutter
  property alias spineGutter: albumPageViewer.spineGutter
  property alias insideGutter: albumPageViewer.insideGutter

  // Read-only, please.
  property real openHorizontalMargin: gu(1)
  property real titleHeight: gu(7)
  
  function open() {
    if (album.closed)
      openAnimation.start();
  }

  function close() {
    if (!album.closed)
      closeAnimation.start();
  }

  width: gu(50)
  height: gu(40)

  Rectangle {
    id: textContainer

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: titleHeight
    
    visible: album != undefined && !album.closed
    
    Column {
      anchors.centerIn: parent

      Text {
        anchors.horizontalCenter: parent.horizontalCenter

        text: (album) ? album.name : "";
        color: "#818285"
        font.pointSize: 18 // Straight from the spec.
        smooth: true
      }

      Item { // Vertical spacer.
        width: 1
        height: gu(1)
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter

        // TODO: subtitle, not creation date.
        text: (album) ? Qt.formatDateTime(album.creationDateTime, "MM/dd/yy") : ""
        color: "#a7a9ac"
        font.pointSize: 14 // From the spec.
        smooth: true
      }
    }
  }

  AlbumPageViewer {
    id: albumPageViewer

    property real topMargin: (album && album.closed ? 0 : openTopMargin)
    property real bottomMargin: (album && album.closed ? 0 : openBottomMargin)

    property real openTopMargin: openHorizontalMargin
    property real openBottomMargin: openHorizontalMargin + titleHeight
    property real closedTopMargin: 0
    property real closedBottomMargin: 0

    album: albumPreviewComponent.album

    anchors.fill: parent
    anchors.topMargin: topMargin
    anchors.bottomMargin: bottomMargin

    topGutter: gu(2)
    bottomGutter: gu(2)
    leftGutter: gu(1)
    rightGutter: gu(1)
    spineGutter: gu(1)
    insideGutter: gu(0.5)

    isPreview: true
  }

  SequentialAnimation {
    id: openAnimation

    ScriptAction { script: albumPageViewer.turnTo(album.currentPageNumber); }

    ParallelAnimation {
      NumberAnimation { target: albumPageViewer; property: "coverTitleOpacity";
        to: 0; easing.type: Easing.OutQuad; duration: 100;
      }

      NumberAnimation { target: albumPageViewer; property: "topMargin";
        to: albumPageViewer.openTopMargin; easing.type: Easing.InQuad; duration: 200;
      }
      NumberAnimation { target: albumPageViewer; property: "bottomMargin";
        to: albumPageViewer.openBottomMargin; easing.type: Easing.InQuad; duration: 200;
      }
    }

    FadeInAnimation { target: textContainer; duration: 500; }
  }

  SequentialAnimation {
    id: closeAnimation

    ScriptAction { script: albumPageViewer.close(); }

    FadeOutAnimation { target: textContainer; duration: 500; }

    ParallelAnimation {
      NumberAnimation { target: albumPageViewer; property: "topMargin";
        to: albumPageViewer.closedTopMargin; duration: 200;
      }
      NumberAnimation { target: albumPageViewer; property: "bottomMargin";
        to: albumPageViewer.closedBottomMargin; duration: 200;
      }
    }
    NumberAnimation { target: albumPageViewer; property: "coverTitleOpacity";
      to: 1; easing.type: Easing.InQuad; duration: 200;
    }
  }
}
