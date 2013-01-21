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
 * Jim Nelson <jim@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

#include "album/album-default-template.h"

AlbumDefaultTemplate::AlbumDefaultTemplate()
  : AlbumTemplate("Default Album Template"),
    next_decision_page_type_(LANDSCAPE) {
  AddPage(new AlbumTemplatePage("Template A Left",
      "qml/Components/AlbumInternals/AlbumPageLayoutLeftPortrait.qml",
      true, 1, PORTRAIT));
  AddPage(new AlbumTemplatePage("Template A Right",
      "qml/Components/AlbumInternals/AlbumPageLayoutRightPortrait.qml",
      false, 1, PORTRAIT));
  AddPage(new AlbumTemplatePage("Template B Left",
      "qml/Components/AlbumInternals/AlbumPageLayoutLeftDoubleLandscape.qml",
      true, 2, LANDSCAPE, LANDSCAPE));
  AddPage(new AlbumTemplatePage("Template B Right",
      "qml/Components/AlbumInternals/AlbumPageLayoutRightDoubleLandscape.qml",
      false, 2, LANDSCAPE, LANDSCAPE));
}

void AlbumDefaultTemplate::reset_best_fit_data() {
  next_decision_page_type_ = LANDSCAPE;
}

AlbumTemplatePage* AlbumDefaultTemplate::get_best_fit_page(bool is_left,
    int num_photos, PageOrientation photo_orientations[]) {

  QList<AlbumTemplatePage*> candidates;
  foreach(AlbumTemplatePage* page, pages()) {
    if (page->is_left() == is_left)
      candidates.append(page);
  }

  // If the first photo is portrait (or there are no photos to place), we use
  // the page with a portrait slot.
  if (num_photos < 1 || photo_orientations[0] == PORTRAIT) {
    foreach(AlbumTemplatePage* page, candidates) {
      if (page->FramesFor(PORTRAIT) > 0)
        return page;
    }
  }

  // If two landscapes (or just one landscape at the end of the list), use the
  // page with multiple landscape slots.
  if (num_photos == 1 || photo_orientations[1] != PORTRAIT) {
    foreach(AlbumTemplatePage* page, candidates) {
      if (page->FramesFor(LANDSCAPE) > 1)
        return page;
    }
  }

  // In this case, we've got a landscape followed by a portrait.  There's no
  // easy way to handle this, so we flip-flop returning the two possible pages.
  AlbumTemplatePage* selected = NULL;
  foreach(AlbumTemplatePage* page, candidates) {
    if (page->FramesFor(next_decision_page_type_) > 0) {
      selected = page;
      break;
    }
  }

  next_decision_page_type_ = (next_decision_page_type_ == PORTRAIT
                              ? LANDSCAPE : PORTRAIT);
  Q_ASSERT(selected != NULL);
  return selected;
}
