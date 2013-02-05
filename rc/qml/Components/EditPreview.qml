/*
 * Copyright (C) 2013 Canonical Ltd
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
 */

import QtQuick 2.0

/*!
  The EditPreview item show the prevew of an edit function.
  As it is using shader functionality for that, it's extrememly fast.
  */
Item {
    id: root

    /// URL to the image that is edited
    property alias source: previewImage.source
    /// Value of the exposure
    property alias exposure:effectItem.exposure

    Image {
        id: previewImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
    }
    ShaderEffect
    {
        id: effectItem
        anchors.centerIn: previewImage
        width: previewImage.paintedWidth
        height: previewImage.paintedHeight
        property var src: previewImage
        property double exposure: 0.0
        vertexShader: "
        uniform mat4 qt_Matrix;
        attribute vec4 qt_Vertex;
        attribute vec2 qt_MultiTexCoord0;
        varying vec2 coord;
        void main()
        {
            coord = qt_MultiTexCoord0;
            gl_Position = qt_Matrix * qt_Vertex;
        }"

        fragmentShader: "
        varying vec2 coord;
        uniform sampler2D src;
        uniform float exposure;
        uniform float qt_Opacity;
        void main()
        {
            float exp = clamp(exposure, -1.0, 1.0);
            vec4 tex = texture2D(src, coord);
            tex.rgb += exp;
            gl_FragColor = tex * qt_Opacity;
        }"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }
}
