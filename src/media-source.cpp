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

#include "media-source.h"

#include "media-collection.h"

MediaSource::MediaSource(const QFileInfo& file)
  : DataSource(file.completeBaseName()), file_(file) {
  preview_file_ = new QFileInfo(file.dir(),
    MediaCollection::THUMBNAIL_DIR + "/" +
    file.completeBaseName() + "_th." + file.completeSuffix());
}

MediaSource::~MediaSource() {
  delete preview_file_;
}

void MediaSource::Init() {
  if (!preview_file_->exists())
    MakePreview(file_, *preview_file_);
}

const QFileInfo& MediaSource::file() const {
  return file_;
}

const QFileInfo& MediaSource::preview_file() const {
  return *preview_file_;
}

Orientation MediaSource::orientation() const {
  return TOP_LEFT_ORIGIN;
}
