/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Guenter Schwann <guenter.schwann@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GALLERYAPPLICATION_H
#define GALLERYAPPLICATION_H

#include "gallery-thumbnail-image-provider.h"

class GalleryApplication {
public:
  static GalleryApplication* instance() {
    if (instance_ == 0)
      instance_ = new GalleryApplication();
    return instance_;
  }

  bool log_image_loading() {
    return false;
  }

private:
  GalleryApplication() {}
  static GalleryApplication* instance_;

};

#endif // GALLERYAPPLICATION_H
