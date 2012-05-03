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
import Gallery 1.0
import "../Capetown"

// Provides a smooth transition between the album overview and album editor.
Item {
  id: albumEditorTransition

  signal editorEntered(variant album)
  signal editorExited(variant album)

  property alias album: thumbnail.album

  property Rectangle backgroundGlass
  property AlbumEditor editor

  property int duration: 500

  function enterEditor(album, thumbnailRect) {
    albumEditorTransition.album = album || null;

    if (thumbnailRect) {
      thumbnail.x = thumbnailRect.x;
      thumbnail.y = thumbnailRect.y;
      thumbnail.width = thumbnailRect.width;
      thumbnail.height = thumbnailRect.height;

      enterExpandAnimation.restart();
    } else {
      enterFadeAnimation.restart();
    }
  }

  function exitEditor(album, thumbnailRect) {
    albumEditorTransition.album = album || null;

    if (thumbnailRect) {
      exitExpandAnimation.thumbnailRect = thumbnailRect;

      thumbnail.x = editor.editorX;
      thumbnail.y = editor.editorY;
      thumbnail.width = editor.editorWidth;
      thumbnail.height = editor.editorHeight;

      exitExpandAnimation.restart();
    } else {
      exitFadeAnimation.restart();
    }
  }

  // internal
  function onExitFinished() {
    editorExited(album);
    album = null;
  }

  AlbumOpener {
    id: thumbnail

    isPreview: false
    contentHasPreviewFrame: true

    visible: false
  }

  ParallelAnimation {
    id: enterFadeAnimation

    onCompleted: editorEntered(albumEditorTransition.album)

    FadeInAnimation {
      target: backgroundGlass
      endOpacity: 0.75
      duration: albumEditorTransition.duration
    }

    FadeInAnimation {
      target: editor
      duration: albumEditorTransition.duration
    }
  }

  ParallelAnimation {
    id: exitFadeAnimation

    onCompleted: onExitFinished()

    FadeOutAnimation {
      target: backgroundGlass
      startOpacity: 0.75
      duration: albumEditorTransition.duration
    }

    FadeOutAnimation {
      target: editor
      duration: albumEditorTransition.duration
    }
  }

  SequentialAnimation {
    id: enterExpandAnimation

    onCompleted: editorEntered(albumEditorTransition.album)

    PropertyAction { target: thumbnail; property: "visible"; value: true; }

    ParallelAnimation {
      ExpandAnimation {
        target: thumbnail
        endX: editor.editorX
        endY: editor.editorY
        endWidth: editor.editorWidth
        endHeight: editor.editorHeight
        duration: albumEditorTransition.duration
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: thumbnail
        property: "openFraction"
        from: (!album || album.closed ? 0 : 1)
        to: 0
        duration: albumEditorTransition.duration
        easing.type: Easing.OutQuad
      }

      FadeInAnimation {
        target: backgroundGlass
        endOpacity: 0.75
        duration: albumEditorTransition.duration
      }
    }

    PropertyAction { target: thumbnail; property: "visible"; value: false; }
    PropertyAction { target: editor; property: "visible"; value: true; }
  }

  SequentialAnimation {
    id: exitExpandAnimation

    property variant thumbnailRect: {"x": 0, "y": 0, "width": 0, "height": 0}

    onCompleted: onExitFinished()

    PropertyAction { target: editor; property: "visible"; value: false; }
    PropertyAction { target: thumbnail; property: "visible"; value: true; }

    ParallelAnimation {
      ExpandAnimation {
        target: thumbnail
        endX: exitExpandAnimation.thumbnailRect.x
        endY: exitExpandAnimation.thumbnailRect.y
        endWidth: exitExpandAnimation.thumbnailRect.width
        endHeight: exitExpandAnimation.thumbnailRect.height
        duration: albumEditorTransition.duration
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: thumbnail
        property: "openFraction"
        from: 0
        to: (!album || album.closed ? 0 : 1)
        duration: albumEditorTransition.duration
        easing.type: Easing.InQuad
      }

      FadeOutAnimation {
        target: backgroundGlass
        startOpacity: 0.75
        duration: albumEditorTransition.duration
      }
    }

    PropertyAction { target: thumbnail; property: "visible"; value: false; }
  }
}
