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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import "../../js/Gallery.js" as Gallery
import "../Utility"

// Some custom components and animations that we want to invoke whenever we
// bring up the album viewer.
Item {
    id: albumViewerTransition

    /*!
    */
    signal transitionToAlbumViewerCompleted()
    /*!
    */
    signal transitionFromAlbumViewerCompleted()
    /*!
    */
    signal dissolveCompleted(variant fadeOutTarget, variant fadeInTarget)

    /*!
    */
    property Album album
    /*!
    */
    property Rectangle backgroundGlass
    /*!
    */
    property int duration: Gallery.SLOW_DURATION
    /*!
    */
    property int easing: Easing.InQuint
    /*!
    */
    property bool isPortrait

    // Read-only
    /*!
    */
    property bool animationRunning: showAlbumViewerAnimation.running ||
                                    hideAlbumViewerAnimation.running || dissolveAlbumViewerTransition.running

    // internal
    /*!
    */
    property bool hideStayingOpen
    /*!
    */
    property variant expandAlbum: albumOpenerLandscape
    /*!
    */
    property bool flipOnClose: isPortrait

    /*!
    */
    function transitionToAlbumViewer(album, thumbnailRect) {
        albumViewerTransition.album = album;
        expandAlbum = albumOpenerLandscape;

        expandAlbum.insideLeftPage = (isPortrait
                                      ? album.currentPage // Anything -- invisible.
                                      : expandAlbum.defaultInsideLeftPage);
        expandAlbum.insideRightPage = (isPortrait
                                       ? album.currentPage
                                       : expandAlbum.defaultInsideRightPage);

        expandAlbum.x = thumbnailRect.x;
        expandAlbum.y = thumbnailRect.y;
        expandAlbum.width = thumbnailRect.width;
        expandAlbum.height = thumbnailRect.height;

        expandAlbum.showCover = album.closed;

        showAlbumViewerAnimation.screenRect = getFullscreenRect(album.closed);
        showAlbumViewerAnimation.start();
    }

    /*!
    */
    function transitionFromAlbumViewer(album, thumbnailRect, stayOpen, viewingPage) {
        // Set up portrait mode even-numbered page close transition.
        albumOpenerPortrait.viewingPage = viewingPage;
        flipOnClose = isPortrait && viewingPage !== album.currentPage && stayOpen &&
                      !viewingPage == 0;

        albumViewerTransition.album = album;
        expandAlbum = flipOnClose ? albumOpenerPortrait : albumOpenerLandscape;

        expandAlbum.insideLeftPage = (isPortrait
                                      ? viewingPage // Anything -- invisible.
                                      : expandAlbum.defaultInsideLeftPage);
        expandAlbum.insideRightPage = (isPortrait
                                       ? viewingPage
                                       : expandAlbum.defaultInsideRightPage);

        var rect = getFullscreenRect(!stayOpen);
        expandAlbum.x = rect.x;
        expandAlbum.y = rect.y;
        expandAlbum.width = rect.width;
        expandAlbum.height = rect.height;

        hideStayingOpen = stayOpen;
        albumOpenerLandscape.showCover = !stayOpen || viewingPage == 0;

        /* The Album preview doesn't like when the album is set to page zero,
           it actually expects its property closed to be set to true in that case.
           But we cant set that during the animation or it will think we're
           flipping to closed, so we set it at the end */
        hideAlbumViewerAnimation.setAlbumClosedAtEnd = viewingPage == 0;
        hideAlbumViewerAnimation.thumbnailRect = thumbnailRect;
        hideAlbumViewerAnimation.start();
    }

    /*!
    */
    function dissolve(fadeOutTarget, fadeInTarget) {
        dissolveAlbumViewerTransition.fadeOutTarget = fadeOutTarget || dissolveDummy;
        dissolveAlbumViewerTransition.fadeInTarget = fadeInTarget || dissolveDummy;
        dissolveAlbumViewerTransition.start();
    }

    // internal
    function getFullscreenRect(portraitHalfOpen) {
        var rect = {"x": 0, "y": 0, "width": 0, "height": 0};

        // This code is kind of hacky.  There's just lots of special casing that we
        // need, and it didn't seem worth it to come up with a cleaner abstraction
        // for what amounts to one-off animation code.

        // We have to compensate for the frame, present in the animation but not in
        // the album viewer.
        var frameWidth = width * expandAlbum.frameToContentWidth;
        // Normally, the spread's width is half of how it appears when open.
        if (!isPortrait)
            frameWidth /= 2;
        var frameHeight = height * expandAlbum.frameToContentHeight;

        // Normally, we center it.  For portrait, we set it flush on one side or
        // the other (if it'll be half-open, flush left, otherwise right; this is
        // because of our thumbnail image having a border on only one side).
        if (isPortrait)
            rect.x = (portraitHalfOpen ? 0 : width - frameWidth);
        else
            rect.x = (width - frameWidth) / 2;
        rect.y = (height - frameHeight) / 2; // Centered.
        rect.width = frameWidth;
        rect.height = frameHeight;

        // Move the thing left 1/4 of the thing's width.  This is to match the
        // opener, which slides right 1/4 of the way at its "half-open" state
        // (actually, 0.25 openFraction).
        if (isPortrait && portraitHalfOpen)
            rect.x -= frameWidth / 4;

        return rect;
    }

    AlbumOpener {
        id: albumOpenerLandscape

        album: parent.album
        isPreview: true
        contentHasPreviewFrame: true

        visible: false
        load: visible
    }

    AlbumViewerTransitionPortraitPage {
        id: albumOpenerPortrait

        album: parent.album
        isPreview: true
        contentHasPreviewFrame: true

        visible: false
        load: visible
    }

    SequentialAnimation {
        id: showAlbumViewerAnimation

        property var screenRect: {"x": 0, "y": 0, "width": 0, "height": 0}

        PropertyAction { target: expandAlbum; property: "visible"; value: true; }

        ParallelAnimation {
            ExpandAnimation {
                target: expandAlbum
                endX: showAlbumViewerAnimation.screenRect.x
                endY: showAlbumViewerAnimation.screenRect.y
                endWidth: showAlbumViewerAnimation.screenRect.width
                endHeight: showAlbumViewerAnimation.screenRect.height
                duration: albumViewerTransition.duration
                easingType: albumViewerTransition.easing
            }

            NumberAnimation {
                target: expandAlbum
                property: "openFraction"
                from: (album && album.closed ? 0 : 1)
                to: isPortrait ? (album && album.closed ? 0.25 : 1) : 0.5
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }

            NumberAnimation {
                target: expandAlbum
                property: "topMargin"
                from: expandAlbum.previewTopMargin
                to: expandAlbum.pageTopMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "bottomMargin"
                from: expandAlbum.previewBottomMargin
                to: expandAlbum.pageBottomMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "gutterMargin"
                from: expandAlbum.previewGutterMargin
                to: expandAlbum.pageGutterMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "outerMargin"
                from: expandAlbum.previewOuterMargin
                to: expandAlbum.pageOuterMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "insideMargin"
                from: expandAlbum.previewInsideMargin
                to: expandAlbum.pageInsideMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }

            FadeInAnimation {
                target: backgroundGlass
                duration: albumViewerTransition.duration
                easingType: albumViewerTransition.easing
            }
        }

        PropertyAction { target: expandAlbum; property: "visible"; value: false; }
        PropertyAction { target: backgroundGlass; property: "visible"; value: false; }

        onRunningChanged: {
            if (running)
                return;

            album.closed = false;
            if (album.currentPage == album.firstValidCurrentPage)
                album.currentPage = album.firstContentPage;

            transitionToAlbumViewerCompleted();
        }
    }

    SequentialAnimation {
        id: hideAlbumViewerAnimation

        property variant thumbnailRect: {"x": 0, "y": 0, "width": 0, "height": 0}
        property bool setAlbumClosedAtEnd: false

        PropertyAction { target: expandAlbum; property: "visible"; value: true; }

        ParallelAnimation {
            ExpandAnimation {
                target: expandAlbum
                endX: hideAlbumViewerAnimation.thumbnailRect.x
                endY: hideAlbumViewerAnimation.thumbnailRect.y
                endWidth: hideAlbumViewerAnimation.thumbnailRect.width
                endHeight: hideAlbumViewerAnimation.thumbnailRect.height
                duration: albumViewerTransition.duration
                easingType: albumViewerTransition.easing
            }

            NumberAnimation {
                target: expandAlbum
                property: "openFraction"

                from: {
                    if (flipOnClose)
                        return 0;
                    else if (isPortrait)
                        return (hideStayingOpen ? 1 : 0.25); // same as "to" property
                    else
                        return 0.5;
                }

                to: flipOnClose ? 1 : (hideStayingOpen ? 1 : 0)

                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }

            NumberAnimation {
                target: expandAlbum
                property: "topMargin"
                from: expandAlbum.pageTopMargin
                to: expandAlbum.previewTopMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "bottomMargin"
                from: expandAlbum.pageBottomMargin
                to: expandAlbum.previewBottomMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "gutterMargin"
                from: expandAlbum.pageGutterMargin
                to: expandAlbum.previewGutterMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "outerMargin"
                from: expandAlbum.pageOuterMargin
                to: expandAlbum.previewOuterMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }
            NumberAnimation {
                target: expandAlbum
                property: "insideMargin"
                from: expandAlbum.pageInsideMargin
                to: expandAlbum.previewInsideMargin
                duration: albumViewerTransition.duration
                easing.type: albumViewerTransition.easing
            }

            FadeOutAnimation {
                target: backgroundGlass
                duration: albumViewerTransition.duration
                easingType: albumViewerTransition.easing
            }
        }

        PropertyAction { target: expandAlbum; property: "visible"; value: false; }

        onRunningChanged: {
            if (running)
                return;

            if (album)
                album.closed = !hideStayingOpen || setAlbumClosedAtEnd;

            transitionFromAlbumViewerCompleted();
        }
    }

    DissolveAnimation {
        id: dissolveAlbumViewerTransition

        fadeOutTarget: dissolveDummy
        fadeInTarget: dissolveDummy
        easingType: albumViewerTransition.easing

        onRunningChanged: {
            if (!running)
                dissolveCompleted(fadeOutTarget, fadeInTarget);
        }
    }

    Item {
        id: dissolveDummy
    }
}
