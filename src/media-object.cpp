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

#include "media-object.h"

#include <QFileInfo>
#include <QDir>

MediaObject::MediaObject(const QFileInfo& file)
  : file_(file) {
  preview_file_ = new QFileInfo(file.dir(), "thumbs/" +
    file.completeBaseName() + "_th." + file.completeSuffix());
}

void MediaObject::Init() {
  if (!preview_file_->exists())
    MakePreview(file_, *preview_file_);
}

MediaObject::~MediaObject() {
  delete preview_file_;
}

const QFileInfo& MediaObject::file() const {
  return file_;
}

const QFileInfo& MediaObject::preview_file() const {
  return *preview_file_;
}
