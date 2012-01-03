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

#ifndef GALLERY_MEDIA_SOURCE_H_
#define GALLERY_MEDIA_SOURCE_H_

#include <QObject>
#include <QFileInfo>

#include "data-source.h"
#include "photo-metadata.h"

typedef DataObjectNumber MediaNumber;

class MediaSource : public DataSource {
  Q_OBJECT
  
 public:
  explicit MediaSource(const QFileInfo& file);
  virtual ~MediaSource();
  
  // TODO: Currently Init() is used to create (or verify existance of)
  // thumbnails ... when we have a thumbnail manager, this use will probably
  // go away
  void Init();
  
  const QFileInfo& file() const;
  const QFileInfo& preview_file() const;
  
  virtual Orientation orientation() const;
  
 protected:
  virtual bool MakePreview(const QFileInfo& original, const QFileInfo& dest) = 0;
  
 private:
  QFileInfo file_;
  QFileInfo* preview_file_;
};

#endif  // GALLERY_MEDIA_SOURCE_H_
