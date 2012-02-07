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
#include "media/preview-manager.h"
#include "qml/gallery-standard-image-provider.h"

MediaSource::MediaSource() {
}

MediaSource::MediaSource(const QFileInfo& file) {
  file_ = file;
  
  SetInternalName(file_.completeBaseName());
}

void MediaSource::RegisterType() {
  qmlRegisterType<MediaSource>("Gallery", 1, 0, "MediaSource");
}

QFileInfo MediaSource::file() const {
  return file_;
}

QUrl MediaSource::path() const {
  return QUrl::fromLocalFile(file_.absoluteFilePath());
}

QUrl MediaSource::gallery_path() const {
  return GalleryStandardImageProvider::ToURL(file_);
}

QFileInfo MediaSource::preview_file() const {
  return PreviewManager::instance()->PreviewFileFor(this);
}

QUrl MediaSource::preview_path() const {
  return QUrl::fromLocalFile(preview_file().absoluteFilePath());
}

QUrl MediaSource::gallery_preview_path() const {
  return GalleryStandardImageProvider::ToURL(preview_file());
}

QImage MediaSource::Image(bool respect_orientation) const {
  // QML data types cannot be abstract, so return a null image
  return QImage();
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
  // TODO: Delete backing file
}
