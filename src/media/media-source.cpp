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

#include "media/media-source.h"

#include "media/media-collection.h"
#include "qml/gallery-standard-image-provider.h"

MediaSource::MediaSource() {
}

MediaSource::~MediaSource() {
  delete preview_file_;
}

void MediaSource::Init(const QFileInfo& file) {
  file_ = file;
  preview_file_ = new QFileInfo(file.dir(),
    MediaCollection::THUMBNAIL_DIR + "/" +
    file.completeBaseName() + "_th." + file.completeSuffix());
  
  SetInternalName(file_.completeBaseName());
  
  if (!preview_file_->exists())
    MakePreview(file_, *preview_file_);
}

void MediaSource::RegisterType() {
  qmlRegisterType<MediaSource>("Gallery", 1, 0, "MediaSource");
}

const QFileInfo& MediaSource::file() const {
  return file_;
}

QUrl MediaSource::path() const {
  return QUrl::fromLocalFile(file_.absoluteFilePath());
}

QUrl MediaSource::gallery_path() const {
  return GalleryStandardImageProvider::ToURL(file_);
}

const QFileInfo& MediaSource::preview_file() const {
  return *preview_file_;
}

QUrl MediaSource::preview_path() const {
  return QUrl::fromLocalFile(preview_file_->absoluteFilePath());
}

QUrl MediaSource::gallery_preview_path() const {
  return GalleryStandardImageProvider::ToURL(*preview_file_);
}

Orientation MediaSource::orientation() const {
  // Default is identity orientation; subclasses should parse source's metadata
  // and return proper value
  return TOP_LEFT_ORIGIN;
}

QDateTime MediaSource::exposure_date_time() const {
  return QDateTime();
}

QDate MediaSource::exposure_date() const {
  return exposure_date_time().date();
}

QTime MediaSource::exposure_time_of_day() const {
  return exposure_date_time().time();
}

int MediaSource::exposure_time_t() const {
  return (int) exposure_date_time().toTime_t();
}

void MediaSource::DestroySource(bool delete_backing, bool as_orphan) {
  // required stub (QML types may not be abstract)
}

bool MediaSource::MakePreview(const QFileInfo& original, const QFileInfo& dest) {
  // required stub (QML types may not be abstract)
  
  return false;
}
