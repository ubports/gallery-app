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

#include "album-template-page.h"

#include <cstdarg>

AlbumTemplatePage::AlbumTemplatePage(const char* name, const char* qml_rc,
  int frame_count, ...)
  : name_(name), qml_rc_(qml_rc) {
  Q_ASSERT(frame_count > 0);
  
  va_list valist;
  va_start(valist, frame_count);
  for (int ctr = 0; ctr < frame_count; ctr++)
    layout_.append(static_cast<PageOrientation>(va_arg(valist, int)));
  va_end(valist);
}

const QString& AlbumTemplatePage::name() const {
  return name_;
}

const QString& AlbumTemplatePage::qml_rc() const {
  return qml_rc_;
}

int AlbumTemplatePage::FrameCount() const {
  return layout_.count();
}

int AlbumTemplatePage::FramesFor(PageOrientation orientation) const {
  int count = 0;
  PageOrientation o;
  foreach (o, layout_) {
    if (o == orientation)
      count++;
  }
  
  return count;
}

const QList<PageOrientation>& AlbumTemplatePage::Layout() const {
  return layout_;
}
