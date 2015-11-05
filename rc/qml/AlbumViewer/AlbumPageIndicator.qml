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
 * Jim Nelson <jim@yorba.org>
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.4
import Gallery 1.0
import Ubuntu.Components 1.3

/*!
*/
Rectangle {
    id: pageIndicator

    /*!
    */
    property Album album
    /*!
    */
    property bool isPortrait: false
    /*!
    */
    property int pagesPerSpread: 2

    // Set this to the viewing page of the album spread.
    property int viewingPage

    // Read-only
    property int indicatorDotCount: 0

    /*!
    */
    signal selected(int page)

    onAlbumChanged: pageIndicatorRepeater.filterModel()

    Connections {
        target: album
        ignoreUnknownSignals: true
        onContentPagesChanged: pageIndicatorRepeater.filterModel()
        onPageCountChanged: pageIndicatorRepeater.filterModel()
    }

    onPagesPerSpreadChanged: pageIndicatorRepeater.filterModel();
    onIsPortraitChanged: pageIndicatorRepeater.filterModel();

    Row {
        anchors.centerIn: parent

        spacing: units.gu(5)

        Repeater {
            id: pageIndicatorRepeater

            model: ListModel {}

            // Eyeballed estimate of max indicators/width ratio.
            property int maxIndicators: pageIndicator.parent.width / 90

            // Re-compute when the # of max indicators has changed.
            onMaxIndicatorsChanged: filterModel();

            function filterModel() {
                model.clear();

                if (!album)
                    return;

                // A "spread" is:
                //    Landscape mode: a pair of two adjacent pages
                //    Portrait mode: a single padge
                var spreadCount = Math.ceil(
                            album.populatedContentPageCount / pagesPerSpread);

                indicatorDotCount = spreadCount;

                if(spreadCount > 1) {
                    var indicatorCount = Math.min(spreadCount, maxIndicators);

                    // Breakdown: we may have more pages than space for dots.  At each
                    // step, we take the ratio (adjusted so that the last dot always has
                    // only the last one page), add it to an accumulator, then assign as
                    // many whole pages to each dot as were in the accumulator.  At the
                    // end, due to rounding errors we may have a page left over, so we
                    // special-case it (and the last, single page).
                    var addend = (spreadCount - 1) / (indicatorCount - 1)
                    var accumulator = 0;
                    var currentSpread = 0;

                    for (var i = 0; i < indicatorCount - 2; ++i) {
                        accumulator += addend;
                        var spreads = Math.floor(accumulator);
                        accumulator -= spreads;

                        model.append({"firstPageIndex": currentSpread * pagesPerSpread +
                                                        album.firstContentPage, "pageCount": spreads * pagesPerSpread});

                        currentSpread += spreads;
                    }

                    model.append({"firstPageIndex": currentSpread * pagesPerSpread + album.firstContentPage,
                                     "pageCount": (spreadCount - currentSpread - 1) * pagesPerSpread});

                    var lastSpread = (isPortrait
                                      ? album.lastPopulatedContentPage
                                      : album.lastContentPage - 1);
                    model.append({"firstPageIndex": lastSpread, "pageCount": pagesPerSpread});
                }
            }

            Component.onCompleted: filterModel()

            delegate: Item {
                property int page: firstPageIndex
                property bool isCurrent: (viewingPage >= firstPageIndex
                                          && viewingPage < firstPageIndex + pageCount)

                width: childrenRect.width
                height: childrenRect.height

                Image {
                    source: (isCurrent
                             ? "img/icon-pager-active.png"
                             : "img/icon-pager-inactive.png")
                }

                MouseArea {
                    anchors.fill: parent

                    enabled: !isCurrent

                    onClicked: selected(page);
                }
            }
        }
    }
}
