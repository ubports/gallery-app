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

import QtQuick 1.1
import Gallery 1.0
import "../Capetown"
import "GalleryUtility.js" as GalleryUtility

Item {
  id: orchestrator

  signal initialized();

  //
  // public properties
  //
  property GalleryOverviewNavigationBar navigationBar
  property int sweepThresholdPixels: gu(6)

  //
  // private properties
  //
  property int movementStartY: 0
  property int viewContentY: 0
  property bool isInitialized: false
  property int lastSampleY: 0
  property int lastDirectionChangeY: 0
  // "movements" vs. "sweeps" -- a "movement" is any user interaction with the
  //     Flickable surface that is wired to this orchestrator, whereas a "sweep"
  //     is a coherent movement of the Flickable surface in a single direction
  //     over many pixels (how many pixels is configurable by setting the
  //     sweepThresholdPixels property). Put another way, every sweep is a
  //     movement, but only some movements are sweeps. Only sweeps will cause
  //     this orchestrator to show or hide the navigation bar it manages.
  property bool isMovementInProgress: false
  property bool isSweepInProgress: false
  property bool isContentSweepingUp: false
  property int topProximityPixels: navigationBar.height - gu(1);

  states: [
    State { name: "scrolledIn";
      PropertyChanges { target: navigationBar; y: 0 }
    },

    State { name: "scrolledOut";
      PropertyChanges { target: navigationBar; y: -navigationBar.height }
    }
  ]

  transitions: [
    Transition {
      from: "scrolledIn"
      to: "scrolledOut"
      reversible: true

      PropertyAnimation {
        target: navigationBar; properties: "y"; duration: 350;
        easing.type: Easing.OutQuad
      }
    }
  ]

  state: "scrolledIn"

  Timer {
    id: sweepSampler

    interval: 33 /* 33 msec == 30 frames/sec */

    onTriggered: {
      if (!orchestrator.isSweepInProgress)
        return;

      // if the user's finger or mouse hasn't traveled some reasonable number of
      // pixels since we made our last sweep direction change, then just take
      // a sample and do a short-circuit return to prevent hysteresis
      var directionDelta =
          orchestrator.viewContentY - orchestrator.lastDirectionChangeY;
      if (Math.abs(directionDelta) < gu(3)) {
        orchestrator.lastSampleY = orchestrator.viewContentY;
        return;
      }

      var sampleDelta = orchestrator.viewContentY - orchestrator.lastSampleY;

      if (orchestrator.isContentSweepingUp && sampleDelta < 0) {
        orchestrator.isContentSweepingUp = false;
        orchestrator.state = "scrolledIn"
        lastDirectionChangeY = viewContentY;
      } else if (!orchestrator.isContentSweepingUp && sampleDelta > 0) {
        orchestrator.isContentSweepingUp = true;
        orchestrator.state = "scrolledOut"
        lastDirectionChangeY = viewContentY;
      }

      orchestrator.lastSampleY = orchestrator.viewContentY;
    }
  }

  function viewScrolled(contentY) {
    if (contentY < topProximityPixels) {
      state = "scrolledIn";
      return;
    }

    viewContentY = contentY

    if (isMovementInProgress && !isSweepInProgress) {
      // CASE 1: The user has started mouse or finger movement to scroll the
      //         view, but we don't yet know whether this finger movement
      //         constitutes a sweep. Our task here is to see if we can detect
      //         a sweep.

      var movementStartDelta = contentY - movementStartY;

      if (Math.abs(movementStartDelta) > sweepThresholdPixels) {
        isSweepInProgress = true;

        if (movementStartDelta > 0) {
          state = "scrolledOut"
          isContentSweepingUp = true;
        } else {
          state = "scrolledIn"
          isContentSweepingUp = false;
        }

        lastDirectionChangeY = lastSampleY = movementStartY;
        sweepSampler.restart();
      }
    } else if (isSweepInProgress) {
      // CASE 2: The user is sweeping the view, but the user's sweep direction
      //         could change during the interaction, so restart the sweep
      //         sample timer to check for this case

      if (!sweepSampler.running)
        sweepSampler.restart();
    }
  }

  function viewMovementStarted(contentY) {
    movementStartY = contentY;
    isMovementInProgress = true;
  }

  function viewMovementEnded(contentY) {
    reset();
  }

  function reset() {
    isMovementInProgress = false;
    isSweepInProgress = false;
  }

  onStateChanged: {
    if (!isInitialized) {
      isInitialized = true;
      initialized()
    }
  }
}
