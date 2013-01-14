/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Nicolas d'Offay <nicolas.doffay@canonical.com>
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
 *
 */

#include "core/gallery-manager.h"

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

GalleryManager::GalleryManager()
    : collections_initialised(false),
      startup_timer_(false),
      is_fullscreen_(false),
      is_portrait_(false),
      form_factors_(NULL),
      form_factor_(NULL),
      log_image_loading_(false),
      resource_(NULL),
      gallery_standard_image_provider_(NULL),
      gallery_thumbnail_image_provider_(NULL),
      database_(NULL),
      default_template_(NULL),
      media_collection_(NULL),
      album_collection_(NULL),
      event_collection_(NULL),
      preview_manager_(NULL)
{

}

GalleryManager::~GalleryManager()
{
    delete gallery_mgr_;
    gallery_mgr_ = NULL;
}

GalleryManager* GalleryManager::GetInstance()
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager();

    return gallery_mgr_;
}
