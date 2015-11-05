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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4
import Gallery 1.0
import "../Utility"
import "../../js/GalleryUtility.js" as GalleryUtility

// An "organic" vertically-scrollable view of media in a particular album.
// This view doesn't have "trays" that scroll horizontally.
OrganicView {
    id: organicAlbumView

    /*!
    */
    property var album
    /*!
    */
    property var albumModel: MediaCollectionModel {
        forCollection: album
    }

    // internal
    /*!
    */
    property int patternsPerRow: Math.max(1, Math.floor(
                                              (width - organicMediaListMargin) / organicMediaListPatternWidth))
    /*!
    */
    property int photosPerRow: organicMediaListMediaPerPattern * patternsPerRow
    /*!
    */
    property real photosWidth: organicMediaListPatternWidth * patternsPerRow +
                               organicMediaListMargin

    selection: SelectionState {
        model: albumModel
    }

    Component.onCompleted: chunkingModel.reset()
    onAlbumChanged: chunkingModel.reset()
    onPhotosPerRowChanged: chunkingModel.reset()
    Connections {
        target: album
        ignoreUnknownSignals: true
        onContentPagesChanged: chunkingModel.reset()
    }

    model: ListModel {
        id: chunkingModel

        function reset() {
            clear();

            if (!album)
                return;

            var rows = Math.ceil(album.containedCount / photosPerRow);
            for (var i = 0; i < rows; ++i)
                append({"head": i * photosPerRow, "limit": photosPerRow});
        }
    }

    delegate: OrganicMediaList {
        id: photosList

        anchors.left: parent.left
        anchors.leftMargin: (parent.width - photosWidth) / 2

        selection: organicAlbumView.selection

        mediaModel: MediaCollectionModel {
            forCollection: album
            head: model.head
            limit: model.limit
        }

        onPressed: {
            var rect = GalleryUtility.translateRect(thumbnailRect, photosList,
                                                    organicAlbumView);
            organicAlbumView.mediaSourcePressed(mediaSource, rect);
        }
    }
}
