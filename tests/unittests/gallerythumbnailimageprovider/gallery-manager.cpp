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
#include "core/command-line-parser.h"

#include <cstddef>
#include <QStandardPaths>
#include <QQuickView>

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

GalleryManager::GalleryManager(const QDir& pictures_dir, QQuickView*, const bool)
    : collections_initialised(false),
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
    Q_UNUSED(pictures_dir);
}

GalleryManager::~GalleryManager()
{
    delete gallery_mgr_;
    gallery_mgr_ = NULL;
}

GalleryManager* GalleryManager::instance(const QDir& pictures_dir,
                                         QQuickView* view, const bool log_image_loading)
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager(pictures_dir, view, log_image_loading);

    return gallery_mgr_;
}
