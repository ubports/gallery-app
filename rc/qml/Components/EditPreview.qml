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
    property alias exposure: effectItem.exposure

    /// brightnes for the color balance
    property alias brightness: effectItem.b
    /// contrast for the color balance
    property alias contrast: effectItem.c
    /// saturation for the color balance
    property alias saturation: effectItem.s
    /// hue for the color balance
    property alias hue: effectItem.h

    /*!
      Sets the exposure as active preview
      */
    function useExposure() {
        effectItem.fragmentShader = root.exposureShader
    }
    /*!
      Sets the color balance as active preview
      */
    function useColorBalance() {
        effectItem.fragmentShader = root.colorBalanceShader
    }

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
        blending: false

        property var src: previewImage

        property double exposure: 0.0

        property real b: 1.0
        property real c: 1.0
        property real s: 1.0
        property real h: 0.0

        property real cos_h: Math.cos(h * (3.1415926 / 180.0))
        property real sin_h: Math.sin(h * (3.1415926 / 180.0))
        property variant h1:
            Qt.vector4d(0.333333 * (1.0 - cos_h) + cos_h,
                        0.333333 * (1.0 - cos_h) + 0.57735 * sin_h,
                        0.333333 * (1.0 - cos_h) - 0.57735 * sin_h,
                        0.0)
        property variant h2:
            Qt.vector4d(0.333333 * (1.0 - cos_h) - 0.57735 * sin_h,
                        0.333333 * (1.0 - cos_h) + cos_h,
                        0.333333 * (1.0 - cos_h) + 0.57735 * sin_h,
                        0.0)
        property variant h3:
            Qt.vector4d(0.333333 * (1.0 - cos_h) + 0.57735 * sin_h,
                        0.333333 * (1.0 - cos_h) - 0.57735 * sin_h,
                        0.333333 * (1.0 - cos_h) + cos_h,
                        0.0)
        property variant s1:
            Qt.vector4d((1.0 - s) * 0.3086 + s,
                        (1.0 - s) * 0.6094,
                        (1.0 - s) * 0.0820,
                        0.0)
        property variant s2:
            Qt.vector4d((1.0 - s) * 0.3086,
                        (1.0 - s) * 0.6094 + s,
                        (1.0 - s) * 0.0820,
                        0.0)
        property variant s3:
            Qt.vector4d((1.0 - s) * 0.3086,
                        (1.0 - s) * 0.6094,
                        (1.0 - s) * 0.0820 + s,
                        0.0)
        property variant b1:
            Qt.vector4d(b, 0.0, 0.0, 0.0)
        property variant b2:
            Qt.vector4d(0.0, b, 0.0, 0.0)
        property variant b3:
            Qt.vector4d(0.0, 0.0, b, 0.0)

        property variant c1:
            Qt.vector4d(c, 0.0, 0.0, c * -0.5 + 0.5)
        property variant c2:
            Qt.vector4d(0.0, c, 0.0, c * -0.5 + 0.5)
        property variant c3:
            Qt.vector4d(0.0, 0.0, c, c * -0.5 + 0.5)

        vertexShader: "
            uniform mat4 qt_Matrix;
            attribute vec4 qt_Vertex;
            attribute vec2 qt_MultiTexCoord0;
            varying vec2 coord;
            void main() {
                coord = qt_MultiTexCoord0;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
    }

    property string exposureShader: "
        varying vec2 coord;
        uniform sampler2D src;
        uniform float exposure;
        uniform float qt_Opacity;
        void main() {
            float exp = clamp(exposure, -1.0, 1.0);
            vec4 tex = texture2D(src, coord);
            tex.rgb += exp;
            gl_FragColor = tex * qt_Opacity;
        }"

    property string colorBalanceShader: "
        varying mediump vec2 coord;
        uniform sampler2D src;
        uniform mediump vec4 h1;
        uniform mediump vec4 h2;
        uniform mediump vec4 h3;
        uniform mediump vec4 s1;
        uniform mediump vec4 s2;
        uniform mediump vec4 s3;
        uniform mediump vec4 b1;
        uniform mediump vec4 b2;
        uniform mediump vec4 b3;
        uniform mediump vec4 c1;
        uniform mediump vec4 c2;
        uniform mediump vec4 c3;
        void main(void) {
            mediump vec4 tmp1;
            mediump vec4 tmp2;
            tmp1 = texture2D(tex, coord);
            tmp2 = vec4(dot(h1, tmp1), dot(h2, tmp1), dot(h3, tmp1), 1.0);
            tmp1 = vec4(dot(s1, tmp2), dot(s2, tmp2), dot(s3, tmp2), 1.0);
            tmp2 = vec4(dot(b1, tmp1), dot(b2, tmp1), dot(b3, tmp1), 1.0);
            tmp1 = vec4(dot(c1, tmp2), dot(c2, tmp2), dot(c3, tmp2), 1.0);
            gl_FragColor = tmp1;
        }"

    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }
}
