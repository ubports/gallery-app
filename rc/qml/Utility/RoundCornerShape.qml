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
 * Guenter Schwann <guenter.schwann@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1 as Theming

/*!
Faster replacement for UbuntuShape
Works only well for the fixed sizes of 342 or 216 pixel
FIXME: remove it all once UbuntuShape is fast
  */
Item {
    id: root

    width: 216
    height: 216

    Theming.ItemStyle.class: "UbuntuShape-radius-" + radius
    /*!
      The size of the corners among: "small" (default) and "medium".
    */
    property string radius: "medium"

    /*!
      The image used as a border.
    */
    property url borderSource: Theming.ComponentUtils.style(root, "borderIdle", "")

    /*!
      The image used to fill the shape.
    */
    property Image image

    /*! \internal */
    onWidthChanged: __updateImageDimensions()
    /*! \internal */
    onHeightChanged: __updateImageDimensions()
    /*! \internal */
    onImageChanged: __updateImageDimensions()

    /*! \internal */
    function __updateImageDimensions() {
        if (!image) return;
        image.width = root.width;
        image.height = root.height;
    }

    ShaderEffect {
        anchors.fill: parent
        visible: root.image

        property Image mask: Image {
            source: root.width < 280 ? "../../img/small-round-corner-mask.png"
                      : "../../img/big-round-corner-mask.png"
            visible: false
        }

        // The imageScale and imageOffset properties are used in imageMaskingShader to
        // compute the correct TexCoords for the image, depending on its fillmode and alignment.
        // FIXME: Cases where the image fillmode is tiling, padding or PreserveAspectFit
        //  are not covered here. Those cases need more than texture coordinate manipulations.
        property point imageScale: getImageScale()
        property point imageOffset: getImageOffset()

        function getImageScale() {
            var scale = Qt.point(1.0, 1.0);
            if (image) {
                if (image.fillMode === Image.PreserveAspectCrop) {
                    scale.x = image.width / image.paintedWidth;
                    scale.y = image.height / image.paintedHeight;
                }
            }
            return scale;
        }

        function getImageOffset() {
            var offset = Qt.point(0.0, 0.0);
            if (image && image.fillMode === Image.PreserveAspectCrop) {
                if (image.horizontalAlignment === Image.AlignRight) {
                    offset.x = (image.paintedWidth - image.width) / image.paintedWidth;
                } else if (image.horizontalAlignment === Image.AlignHCenter) {
                    offset.x = (image.paintedWidth - image.width) / image.paintedWidth / 2.0;
                }
                if (image.verticalAlignment === Image.AlignBottom) {
                    offset.y = (image.paintedHeight - image.height) / image.paintedHeight;
                } else if (image.verticalAlignment === Image.AlignVCenter) {
                    offset.y = (image.paintedHeight - image.height) / image.paintedHeight / 2.0;
                }
            }
            return offset;
        }

        property Image image: root.image && root.image.status == Image.Ready ? root.image : null

        fragmentShader:
            "
            varying highp vec2 qt_TexCoord0;
            uniform lowp float qt_Opacity;
            uniform sampler2D mask;
            uniform sampler2D image;
            uniform highp vec2 imageScale;
            uniform highp vec2 imageOffset;

            void main(void)
            {
                lowp vec4 maskColor = texture2D(mask, qt_TexCoord0.st);
                lowp vec4 imageColor = texture2D(image, imageOffset + imageScale * qt_TexCoord0.st);
                gl_FragColor = imageColor * maskColor.a * qt_Opacity;
            }
            "
    }

    BorderImage {
        id: border

        anchors.fill: parent
        source: root.borderSource
    }
}
