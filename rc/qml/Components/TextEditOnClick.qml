/*
 * Copyright (C) 2012-2015 Canonical Ltd
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

import QtQuick 2.4

// Editable text area that becomes editable when clicked. Editing ends when
// enter is pressed or when done() is called. The isEditing property indicates
// when the text is being edited.
//
// Note that this will switch between a Text and TextEdit for performance
// reasons, as TextEdit's performance is quite sluggish.
FocusScope {
    id: textEditOnClick

    // Fired when the user hits enter to complete the text.
    signal enterPressed()

    // Fired when the user has changed the text.
    signal textUpdated(string text)

    // Text properties.
    /*!
    */
    property string text
    /*!
    */
    property int textFormat: Text.AutoText
    /*!
    */
    property int horizontalAlignment
    /*!
    */
    property int wrapMode: Text.NoWrap
    /*!
    */
    property variant color: "#000000"
    /*!
    */
    property real fontPixelSize
    /*!
    */
    property string fontFamily: "sans-serif"

    // Set this to false if you don't want this field to become
    // editable when clicked.
    property bool editable: true

    // Maximum number of lines for this element.
    property int maximumLineCount: 2

    // Indicates when we're in editing mode.
    readonly property bool isEditing: state == "editable"

    height: childrenRect.height

    state: "readOnly"

    // Internal properties
    property int lastClickX
    property int lastClickY;

    // Call this to force editing to begin.
    // X and Y is a mouse cursor position, which affects the placement of the
    // text cursor.  Set both to -1 to avoid this behavior.
    function start(x, y) {
        if (state == "readOnly") {
            lastClickX = x;
            lastClickY = y;
            state = "editable";
        }
    }

    // Call this when editing is finished.
    function done() {
        if (state == "editable") {
            state = "readOnly";
        }
    }

    states: [
        State {
            name: "readOnly";
        },
        State {
            name: "editable";
        }
    ]

    Loader {
        focus: true
        sourceComponent: isEditing ? editableTextContainer : staticTextContainer
    }

    Component {
        id: staticTextContainer

        Text {
            id: staticText

            text: textEditOnClick.text
            textFormat: textEditOnClick.textFormat
            horizontalAlignment: textEditOnClick.horizontalAlignment
            wrapMode: textEditOnClick.wrapMode
            color: textEditOnClick.color
            font.pixelSize: textEditOnClick.fontPixelSize
            font.family: textEditOnClick.fontFamily
            maximumLineCount: textEditOnClick.maximumLineCount

            width: textEditOnClick.width
            smooth: true

            MouseArea {
                anchors.fill: parent
                enabled: editable && textEditOnClick.state == "readOnly"

                onPressed: {
                    // Setting mouse.accepted = false here triggers an assertion, so
                    // we're stuck passing mouse coordinates in and calculating the
                    // cursor position based on that.
                    start(mouse.x, mouse.y);
                }
            }
        }
    }

    Component {
        id: editableTextContainer

        TextEdit {
            id: editableText

            text: textEditOnClick.text
            textFormat: textEditOnClick.textFormat
            horizontalAlignment: textEditOnClick.horizontalAlignment
            wrapMode: textEditOnClick.wrapMode
            color: textEditOnClick.color
            font.pixelSize: textEditOnClick.fontPixelSize
            font.family: textEditOnClick.fontFamily
            inputMethodHints: Qt.ImhNoPredictiveText

            readOnly: false
            smooth: true

            width: textEditOnClick.width

            // Stores previous string for maintaining max line count.
            property string previousText: textEditOnClick.text

            Component.onCompleted: {
                // Force cursor position and focus.
                if (lastClickX !== -1 && lastClickY !== -1)
                    cursorPosition = positionAt(lastClickX, lastClickY);
                forceActiveFocus();
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Return) {
                    done();
                    enterPressed();
                    event.accepted = true;
                }
            }

            // Ugly hack to allow a max line count.
            // Idea taken from: https://bugreports.qt-project.org/browse/QTBUG-12304
            onTextChanged: {
                if (lineCount > maximumLineCount) {
                    var cursor = cursorPosition;
                    text = previousText;
                    cursorPosition = cursor > text.length ? text.length : cursor - 1;
                }

                // Save text so we can revert if necessary.
                previousText = text;
            }

            // Fire signal when editing completes.
            onActiveFocusChanged: {
                if (!activeFocus)
                    textUpdated(text)
            }

            onPaintedHeightChanged: height = paintedHeight;
        }
    }
}
