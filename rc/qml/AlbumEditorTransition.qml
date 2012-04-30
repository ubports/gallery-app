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

  signal editorEntered()
  signal editorExited()

  property Rectangle backgroundGlass
  property AlbumEditor editor

  function enterEditor() {
    enterAnimation.restart();
  }

  function exitEditor() {
    exitAnimation.restart();
  }

  ParallelAnimation {
    id: enterAnimation

    onCompleted: albumEditorTransition.editorEntered()

    FadeInAnimation {
      target: backgroundGlass
      endOpacity: 0.75
    }

    FadeInAnimation {
      target: editor
    }
  }

  ParallelAnimation {
    id: exitAnimation

    onCompleted: albumEditorTransition.editorExited()

    FadeOutAnimation {
      target: backgroundGlass
      startOpacity: 0.75
    }

    FadeOutAnimation {
      target: editor
    }
  }
}
