/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../Components"
import "../../js/Gallery.js" as Gallery

/*!
*/
Item {
    id: photoViewerDelegate

    /*!
    */
    property MediaSource mediaSource
    /*!
    */
    property bool useInteractivePreview
    /*!
    */
    property alias isLoaded: fullPhotoComponentLoader.isLoaded

    /*!
    */
    signal clicked()
    /*!
    */
    signal zoomed()
    /*!
    */
    signal unzoomed()

    /// Resets the view to it's initial state
    function reset() {
        if (!isLoaded)
            return;

        fullPhotoComponentLoader.item.unzoom();
    }

    // Note that we haven't defined a state set in this component, even though
    // we bind the "state" property here. This is intentional and is necessary
    // to work around a binding issue on Qt 5.0 beta 1. Given a later version
    // of Qt, this treatment might not be necessary.
    state: (fullPhotoComponentLoader.item &&
            !fullPhotoComponentLoader.item.fullyUnzoomed) ? "zoomed" :
                                                            "unzoomed";

    Image {
        id: interactivePreviewImage

        z: 0

        anchors.fill: parent

        visible: photoViewerDelegate.state == "unzoomed"

        fillMode: Image.PreserveAspectFit

        source: mediaSource.galleryPreviewPath
    }

    Loader {
        id: fullPhotoComponentLoader

        property bool isLoaded: status == Loader.Ready

        z: 1
        asynchronous: true
        anchors.fill: parent
        visible: isLoaded && !useInteractivePreview

        sourceComponent: (!useInteractivePreview && !sourceComponent) ?
                             fullPhotoComponent : sourceComponent;

        Component {
            id: fullPhotoComponent

            ZoomablePhotoComponent {
                id: galleryPhotoComponent
                objectName: "openedPhoto" + index

                anchors.fill: parent
                color: "transparent"

                mediaSource: photoViewerDelegate.mediaSource
                load: true;

                ownerName: "photoViewerDelegate"

                onZoomed: photoViewerDelegate.zoomed()
                onUnzoomed: photoViewerDelegate.unzoomed()
                onClicked: photoViewerDelegate.clicked()
            }
        }
    }
}
