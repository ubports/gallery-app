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
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  id: pageIndicator
  
  property Album album
  
  signal selected(int pageNumber)
  
  Row {
    anchors.centerIn: parent

    spacing: gu(5)
    
    Repeater {
      id: pageIndicatorRepeater
      
      // TODO: come up with a formula for this that takes into account
      // orientation, toolbar icons, etc.
      property int maxIndicators: 20
      
      property AlbumPageModel fullModel : AlbumPageModel {
        forAlbum: album
        
        onCountChanged: {
          pageIndicatorRepeater.filterModel();
        }
      }

      property ListModel filteredModel: ListModel {}
      
      function filterModel() {
        filteredModel.clear();

        if(fullModel.count > 1) {
          var indicatorCount = Math.min(fullModel.count, maxIndicators);

          // Breakdown: we may have more pages than space for dots.  At each
          // step, we take the ratio (adjusted so that the last dot always has
          // only the last one page), add it to an accumulator, then assign as
          // many whole pages to each dot as were in the accumulator.  At the
          // end, due to rounding errors we may have a page left over, so we
          // special-case it (and the last, single page).
          var addend = (fullModel.count - 1) / (indicatorCount - 1)
          var accumulator = 0;
          var currentPage = 0;

          for (var i = 0; i < indicatorCount - 2; ++i) {
            accumulator += addend;
            var pageCount = Math.floor(accumulator);
            accumulator -= pageCount;

            filteredModel.append({"firstPageIndex": currentPage, "pageCount": pageCount});
            currentPage += pageCount;
          }
          filteredModel.append({"firstPageIndex": currentPage, "pageCount": fullModel.count - currentPage - 1});
          filteredModel.append({"firstPageIndex": fullModel.count - 1, "pageCount": 1});
        }
        
        model = filteredModel;
      }
      
      Component.onCompleted: {
        filterModel();
      }
      
      onFullModelChanged: {
        filterModel();
      }
      
      delegate: Item {
        property int pageNumber: firstPageIndex
        property bool isCurrent: (!album.closed && album.currentPageNumber >= firstPageIndex
                                  && album.currentPageNumber < firstPageIndex + pageCount)
        
        width: childrenRect.width
        height: childrenRect.height

        Text {
          // bullet character
          text: "\u2022"
          
          color: (isCurrent ? "steelblue" : "lightgray")
          
          font.family: "Ubuntu"
          font.bold: true
          font.pixelSize: gu(3.5)
        }
        
        MouseArea {
          anchors.fill: parent
          
          enabled: !isCurrent
          
          onClicked: {
            selected(pageNumber);
          }
        }
      }
    }
  }
}
