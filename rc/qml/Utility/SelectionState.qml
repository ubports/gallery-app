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

// Holds all the state for selection between the Organic views, and some other
// places.
Item {
    id: organicSelectionState

    /// Is true if the selection mode is active
    property bool inSelectionMode: false
    /// If false, none of the selection functions has an effect
    property bool allowSelectionModeChange: true
    /// It true, only one item can be selected
    property bool singleSelect: false
    /// If true, a mix of videos and pictures is selected.
    /// Should be ignored if only one item is selected.
    property bool isMixed: false
    /// The type of all the selected media.
    /// Should be ignored if isMixed is true.
    property var mediaType

    /*!
    */
    property var model: EventOverviewModel {
        monitored: true
    }

    Connections {
        target: model
        onSelectionChanged: {
            var data = model.selectedMediasQML;
            var photos = 0;
            var videos = 0;
            var medias = 0;

            for (var i in data) {
                if (data[i].hasOwnProperty('type')) {
                    medias++;
                    (data[i].type === MediaSource.Photo) ? photos++ : videos++;
                }
            }

            organicSelectionState.mediaType = (photos > videos) ? MediaSource.Photo : MediaSource.Video;
            organicSelectionState.isMixed = (photos > 0 && videos > 0);
            organicSelectionState.selectedMediaCount = medias;

            organicSelectionState.selectedPhotosCount = photos;
            organicSelectionState.selectedVideosCount = videos;
        }
    }

    // readonly
    /// The number of currently selected items
    property int selectedCount: model.selectedCount

    // readonly
    /// The number of currently selected media items
    property int selectedMediaCount: 0

    // readonly
    /// The number of currently selected photos and videos items
    property int selectedPhotosCount: 0
    property int selectedVideosCount: 0

    //internal
    // HACK: this is used as a spurious extra QML condition in our isSelected
    // check so we can cause the function to be reevaluated whenever the
    // selection changes.  I couldn't see any easier way to trigger the model's
    // isSelected function to get reevaluated in QML whenever its internal
    // selection state changes.
    property int refresh: 1

    /*!
    */
    function isSelected(item) {
        return refresh > 0 && model.isSelected(item);
    }

    /*!
    */
    function toggleSelection(item) {
        if (tryEnterSelectionMode()) {
            if (singleSelect) {
                var select = !isSelected(item);
                unselectAll();
                if (select)
                    model.toggleSelection(item);
            }
            else
                model.toggleSelection(item);
        }
    }

    /*!
    */
    function selectAll() {
        if (tryEnterSelectionMode() && !singleSelect)
            model.selectAll();
    }

    /*!
    */
    function unselectAll() {
        model.unselectAll();
    }

    /*!
    */
    function tryEnterSelectionMode() {
        if (allowSelectionModeChange)
            inSelectionMode = true;
        return inSelectionMode;
    }

    /*!
    */
    function leaveSelectionMode() {
        if (allowSelectionModeChange) {
            inSelectionMode = false;
            model.unselectAll();
        }
    }

    Connections {
        target: model
        onSelectedCountChanged: ++refresh
    }
}
