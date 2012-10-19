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

import QtQuick 2.0
import Gallery 1.0
import "../js/Gallery.js" as Gallery
import "../js/GalleryUtility.js" as GalleryUtility
import "Utility"
import "Widgets"

Item {
  id: eventTimelineTransition
  
  signal transitionFinished(bool toTimeline)

  property real timelineFraction

  property EventCheckerboard checkerboard
  property EventTimeline timeline
  
  property alias duration: animator.duration
  
  // readonly
  property bool isFlipping: (timelineFraction !== 0 && timelineFraction !== 1)
  property alias isRunning: animator.running
  property real itemWidth: checkerboard.itemWidth
  property real itemHeight: checkerboard.itemHeight
  property real gutterWidth: checkerboard.gutterWidth
  property real gutterHeight: checkerboard.gutterHeight
  property real delegateWidth: checkerboard.delegateWidth
  property real delegateHeight: checkerboard.delegateHeight
  property real topExtraGutter: checkerboard.topExtraGutter
  property real bottomExtraGutter: checkerboard.bottomExtraGutter
  property real leftExtraGutter: checkerboard.leftExtraGutter
  property real rightExtraGutter: checkerboard.rightExtraGutter
  
  // internal
  property EventOverviewModel model: EventOverviewModel {}
  property int xMax: width / delegateWidth
  property int gridWidth: getDeviceSpecific('photoThumbnailWidth')
  property int gridHeight: getDeviceSpecific('photoThumbnailHeight')
  property int timelineWidth: getDeviceSpecific('photoThumbnailWidthTimeline')
  property int timelineHeight: getDeviceSpecific('photoThumbnailHeightTimeline')
  property Event event
  property real endFraction

  // public
  function prepare(event, toTimeline) {
    // Because it's desireable for the selected event to be visible in the timeline
    // view when the transition occurs, need to ensure it's ready.  However,
    // can't do it and start the transition because the event loop has
    // not had a chance to run.  By using a Timer, we give the loop a chance to
    // process the request before starting the transition
    if (toTimeline)
      timeline.ensureIndexVisible(timeline.model.indexOf(event), true);
    else
      checkerboard.ensureIndexVisible(checkerboard.model.indexOf(event), true);

    eventTimelineTransition.event = event;
    model.clear();

    delayedTimer.start();
  }

  function finish(toTimeline) {
    eventTimelineTransition.endFraction = (toTimeline ? 1 : 0);
    animator.restart();
  }

  // internal
  function delayedPrepare() {
    if (!checkerboard || !timeline)
      return;
    
    // Only need to fetch MediaSources; the EventOverviewModel will automatically
    // create Event Cards
    var vc = checkerboard.getVisibleMediaSources();
    var vt = timeline.getVisibleMediaSources();
    
    // populate with items visible in the checkerboard; their locations will be
    // determined as the delegates are created
    for (var ctr = 0; ctr < vc.length; ctr++)
      model.add(vc[ctr]);
    
    // add items visible in the timline but not in the checkerboard; they
    // will emerge from their appropiate event card and dissolve in/out as they
    // fly in to/out of position
    for (var ctr = 0; ctr < vt.length; ctr++) {
      if (vc.indexOf(vt[ctr]) == -1)
        model.add(vt[ctr]);
    }
  }
  
  Timer {
    id: delayedTimer
    
    interval: 20
    repeat: false
    
    onTriggered: eventTimelineTransition.delayedPrepare()
  }
  
  NumberAnimation {
    id: animator

    target: eventTimelineTransition
    property: "timelineFraction"
    to: endFraction
    duration: Gallery.SLOW_DURATION
    easing.type: Easing.InQuint

    onRunningChanged: {
      if (!running)
        transitionFinished(endFraction === 1);
    }
  }

  Repeater {
    id: repeater
    
    anchors.fill: parent

    anchors.topMargin: topExtraGutter
    anchors.bottomMargin: bottomExtraGutter
    anchors.leftMargin: leftExtraGutter
    anchors.rightMargin: rightExtraGutter
    
    model: eventTimelineTransition.model
    
    delegate: Item {
      id: cell

      x: GalleryUtility.interpolate(gridX, timelineX, timelineFraction)
      y: GalleryUtility.interpolate(gridY, timelineY, timelineFraction)
      width: GalleryUtility.interpolate(gridWidth, timelineWidth, timelineFraction)
      height: GalleryUtility.interpolate(gridHeight, timelineHeight, timelineFraction)

      opacity: GalleryUtility.interpolate(gridOpacity, timelineOpacity, timelineFraction)

      // This is necessary to expose the index properties externally
      property int index: model.index
      property int gridX
      property int gridY
      property int timelineX
      property int timelineY
      property bool isEvent: model.typeName == "Event"
      property bool hidesUnderEvent: false
      property bool disappears: false
      property real gridOpacity: 1.0
      property real timelineOpacity
      timelineOpacity: { // Parse error unless split onto two lines.
        if (disappears || hidesUnderEvent)
          return 0.0;
        else if (isEvent)
          return Gallery.EVENT_TIMELINE_EVENT_CARD_OPACITY;
        else
          return Gallery.EVENT_TIMELINE_MEDIA_SOURCE_OPACITY;
      }

      Component.onCompleted: {
        if (!checkerboard || !timeline)
          return;
        
        // although could use checkerboard.getRectOfObject(), that method
        // uses a potentially expensive searching algorithm; plus, if the
        // object is not loaded into the GridView, still need to know where
        // it would be located *if* it was.  Its index in the model and the
        // dimensions of the delegates and viewing area means that is
        // deterministic for all objects

        // Note that we have to compensate for the photo being slightly smaller
        // than the GridView delegate (the gutter), and the
        // top/leftExtraGutters.
        var widthOffset = gutterWidth / 2 + leftExtraGutter;
        var heightOffset = gutterHeight / 2 + topExtraGutter;

        var cindex = checkerboard.model.indexOf(model.object);
        if (cindex >= 0) {
          // As view scrolls vertically, x coordinate needs no translation
          gridX = (cindex % xMax) * delegateWidth + widthOffset;
          
          // translate y coordinate according to checkerboard viewport
          gridY = (Math.floor(cindex / xMax) * delegateHeight) - checkerboard.contentY + heightOffset;
        } else {
          console.log("Unable to find index for", model.object);
          disappers = true;
        }
        
        // Formula for deriving timeline location:
        // 1. If Event, fly to EventCard position
        // 2. If Photo and visible in Event Timeline, fly to PhotoComponent position
        // 3. If Photo and not visible in Event Timeline, fly to assoc. EventCard
        
        var geom = !isEvent ? timeline.getGeometryOfMediaSource(model.mediaSource) : undefined;
        if (!geom && !isEvent) {
          geom = timeline.getGeometryOfEvent(model.mediaSource.event);
          hidesUnderEvent = true;
        } else if (isEvent) {
          geom = timeline.getGeometryOfEvent(model.object);
        }
        
        if (geom) {
          if (!hidesUnderEvent)
            z = geom.z;
          geom = GalleryUtility.translateRect(geom, timeline, eventTimelineTransition);
          timelineX = geom.x;
          timelineY = geom.y;
        } else {
          // this can happen if the event line hasn't been instantiated yet
          // in the timeline (due to ListView only instantiating delegates
          // on-demand) ... this is okay, not vital for this transition to
          // look good
          disappears = true;
          timelineX = gridX;
          timelineY = gridY;
        }
      }
      
      EventCheckerboardPreview {
        anchors.fill: parent

        ownerName: "EventTimelineTransition"

        mediaSource: (model.typeName == "MediaSource") ? model.mediaSource : undefined
        event: (model.typeName == "Event") ? model.object : undefined
      }
    }
  }
}
