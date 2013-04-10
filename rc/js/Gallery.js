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
 * Charles Lindsay <chaz@yorba.org>
 */

// Animation durations, in milliseconds.  For SNAP_DURATION, use linear easing;
// others should use some form of quint easing.  Note that these names are
// based on names from animation guidelines, but the values are made up and
// should eventually come from the SDK.
var SNAP_DURATION = 175;
var FAST_DURATION = 300;
var SLOW_DURATION = 700;
var SLEEPY_DURATION = 1500;

var EVENT_TIMELINE_MEDIA_SOURCE_OPACITY = 0.8;
var EVENT_TIMELINE_EVENT_CARD_OPACITY = 1.0;

var HIGHLIGHT_BUTTON_COLOR = "#c94212"

function isSmallFormFactor(form_factor) {
  if (!form_factor)
    form_factor = FORM_FACTOR; // From C++.

  return (form_factor === 'phone' || form_factor === 'sidebar');
}

function isDesktop(form_factor) {
  if (!form_factor)
    form_factor = FORM_FACTOR; // From C++.

  return (form_factor === 'desktop');
}
