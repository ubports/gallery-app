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
 */

#include "album/album-default-template.h"

#include "album/album.h"

AlbumDefaultTemplate* AlbumDefaultTemplate::instance_ = NULL;

AlbumDefaultTemplate::AlbumDefaultTemplate()
  : AlbumTemplate("Default Album Template") {
  // TODO: For now, album layouts are hardcoded here, which is a bit fragile
  // as they must exactly descrive their respective templates, and changes
  // there must be reflected here.  For now, this will have to do, although
  // in the future a more robust solution will be implemented.
  AddPage(new AlbumTemplatePage("Template A Left",
    "qrc:/rc/qml/AlbumPageLayoutLeftPortrait.qml", 1, PORTRAIT));
  AddPage(new AlbumTemplatePage("Template A Right",
    "qrc:/rc/qml/AlbumPageLayoutRightPortrait.qml", 1, PORTRAIT));
  AddPage(new AlbumTemplatePage("Template B Left",
    "qrc:/rc/qml/AlbumPageLayoutLeftDoubleLandscape.qml", 2, LANDSCAPE, LANDSCAPE));
  AddPage(new AlbumTemplatePage("Template B Right",
    "qrc:/rc/qml/AlbumPageLayoutRightPortrait.qml", 1, PORTRAIT));
}

void AlbumDefaultTemplate::Init() {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new AlbumDefaultTemplate();
}

AlbumDefaultTemplate* AlbumDefaultTemplate::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}
