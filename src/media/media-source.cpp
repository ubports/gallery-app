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

#include "event/event-collection.h"
#include "media/media-collection.h"
#include "media/preview-manager.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include "database/database.h"
#include "core/gallery-manager.h"

MediaSource::MediaSource() : id_(INVALID_ID) {
}

MediaSource::MediaSource(const QFileInfo& file) : id_(INVALID_ID) {
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
  return GalleryManager::GetInstance()->preview_manager()->PreviewFileFor(file_);
}

QUrl MediaSource::preview_path() const {
  return QUrl::fromLocalFile(preview_file().absoluteFilePath());
}

QUrl MediaSource::gallery_preview_path() const {
  return GalleryStandardImageProvider::ToURL(preview_file());
}

QFileInfo MediaSource::thumbnail_file() const {
  return GalleryManager::GetInstance()->preview_manager()->ThumbnailFileFor(file_);
}

QUrl MediaSource::thumbnail_path() const {
  return QUrl::fromLocalFile(thumbnail_file().absoluteFilePath());
}

QUrl MediaSource::gallery_thumbnail_path() const{
  return GalleryThumbnailImageProvider::ToURL(thumbnail_file());
}

QImage MediaSource::Image(bool respect_orientation) {
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

const QSize& MediaSource::size() {
  if (!is_size_set()) {
    // This is potentially very slow, so you should set the size as early as
    // possible to avoid this.
    QImage image = Image();
    set_size(image.size());
  }

  return size_;
}

void MediaSource::set_size(const QSize& size) {
  if (size_ == size)
    return;

  size_ = size;
  notify_size_altered();
}

bool MediaSource::is_size_set() const {
  return size_.isValid();
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

Event* MediaSource::FindEvent() {
  return GalleryManager::GetInstance()->event_collection()->EventForMediaSource(this);
}

QVariant MediaSource::QmlFindEvent() {
  return QVariant::fromValue(FindEvent());
}

bool MediaSource::busy() {
  return busy_;
}

void MediaSource::set_id(qint64 id) {
  id_ = id;
}

qint64 MediaSource::get_id() {
  return id_;
}

void MediaSource::set_busy(bool busy) {
  if (busy == busy_)
    return;
  
  busy_ = busy;
  emit busyChanged();
}


void MediaSource::DestroySource(bool delete_backing, bool as_orphan) {
  if (delete_backing) {
    if (!QFile::remove(file_.absoluteFilePath()))
      qDebug("Unable to delete media file %s", qPrintable(file_.absoluteFilePath()));
  }
}

void MediaSource::notify_data_altered() {
  emit data_altered();
}

void MediaSource::notify_size_altered() {
  emit size_altered();

  if (id_ != INVALID_ID)
    GalleryManager::GetInstance()->database()->get_media_table()->set_media_size(id_, size_);
}
