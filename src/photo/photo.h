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
 * Lucas Beeler <lucas@yorba.org>
 */

#ifndef GALLERY_PHOTO_H_
#define GALLERY_PHOTO_H_

#include <QObject>
#include <QDateTime>
#include <QFileInfo>

#include "media/media-source.h"
#include "photo/photo-metadata.h"

class Photo : public MediaSource {
  Q_OBJECT
  
 public:
  static const int PREVIEW_WIDTH_MAX = 198;
  static const int PREVIEW_HEIGHT_MAX = 148;
  
  explicit Photo(const QFileInfo& file);
  
  virtual Orientation orientation() const;
  virtual QDateTime exposure_date_time() const;
  
 protected:
  virtual bool MakePreview(const QFileInfo& original, const QFileInfo& dest);
  
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
 private:
  // Go ahead and cache the photo's metadata object inside the photo. Insofar
  // as we know, Gallery will be the only application on the device mutating
  // photo files, so we won't have to worry about the cache going stale.
  PhotoMetadata* metadata_;
  mutable QDateTime *exposure_date_time_;
};

#endif  // GALLERY_PHOTO_H_
