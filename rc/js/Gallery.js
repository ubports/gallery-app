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

var EVENT_TIMELINE_MEDIA_SOURCE_OPACITY = 0.8;
var EVENT_TIMELINE_EVENT_CARD_OPACITY = 1.0;

function getDeviceSpecific(key, form_factor, is_portrait) {
  // Big list of form factor-specific values.  'default' is used if the key
  // can't be found under any other form_factor (otherwise, form_factors should
  // match the FORM_FACTOR values passed from main.cpp).  Just add Portrait to
  // the end of a key to make it portrait-mode specific.
  var specifics = {
    'default': {
      photoThumbnailWidth: gu(24),
      photoThumbnailHeight: gu(18),

      photoGridTopMargin: gu(2),
      photoGridLeftMargin: gu(2),
      photoGridRightMargin: gu(2),

      photoGridGutterWidth: gu(2),
      photoGridGutterHeight: gu(2),

      photoThumbnailWidthTimeline: gu(24),
      photoThumbnailHeightTimeline: gu(18),
      timelineFirstPhotoDistance: gu(25), // width + 1
      timelineSecondPhotoDistance: gu(25), // width + 1

      albumThumbnailWidth: gu(28),
      albumThumbnailHeight: gu(33),

      albumGridTopMargin: gu(2),
      albumGridLeftMargin: gu(2),
      albumGridRightMargin: gu(2),

      albumGridGutterWidth: gu(6),
      albumGridGutterHeight: gu(8),
    },

    phone: {
      photoThumbnailWidth: gu(17),
      photoThumbnailHeight: gu(13),

      photoGridTopMargin: gu(2),
      photoGridLeftMargin: gu(4),
      photoGridRightMargin: gu(4),
      photoGridTopMarginPortrait: gu(1),
      photoGridLeftMarginPortrait: gu(1),
      photoGridRightMarginPortrait: gu(1),

      photoGridGutterWidth: gu(4),
      photoGridGutterHeight: gu(4),
      photoGridGutterWidthPortrait: gu(2),
      photoGridGutterHeightPortrait: gu(2),

      photoThumbnailWidthTimeline: gu(13),
      photoThumbnailHeightTimeline: gu(10),
      timelineFirstPhotoDistance: gu(10.5),
      timelineSecondPhotoDistance: gu(10),
      timelineFirstPhotoDistancePortrait: gu(3),
      timelineSecondPhotoDistancePortrait: gu(2),

      albumThumbnailWidth: gu(17),
      albumThumbnailHeight: gu(20),

      albumGridTopMargin: gu(1),
      albumGridLeftMargin: gu(4),
      albumGridRightMargin: gu(4),
      albumGridTopMarginPortrait: -gu(1), // gets added to a bigger positive value
      albumGridLeftMarginPortrait: gu(1),
      albumGridRightMarginPortrait: gu(1),

      albumGridGutterWidth: gu(4),
      albumGridGutterHeight: gu(6),
      albumGridGutterWidthPortrait: gu(2),
      albumGridGutterHeightPortrait: gu(6),
    },
  };

  if (is_portrait === undefined)
    is_portrait = isPortrait; // From GalleryApplication.

  if (!form_factor)
    form_factor = FORM_FACTOR; // From C++.
  if (form_factor === 'sidebar')
    form_factor = 'phone'; // Equivalent; saves typing above.
  if (specifics[form_factor] === undefined)
    form_factor = 'default';

  var portrait_key = key + 'Portrait';
  if (is_portrait && specifics[form_factor][portrait_key] !== undefined)
    return specifics[form_factor][portrait_key];
  if (is_portrait && specifics['default'][portrait_key] !== undefined)
    return specifics['default'][portrait_key];

  if (specifics[form_factor][key] !== undefined)
    return specifics[form_factor][key];
  if (specifics['default'][key] !== undefined)
    return specifics['default'][key];

  console.debug("Invalid key '" + key + "' passed to Gallery.getDeviceSpecific()");
  return undefined;
}
