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

#include "photo/photo.h"

#include <QFileInfo>
#include <QImage>

bool Photo::IsValid(const QFileInfo& file) {
  QString extension = file.suffix().toLower();
  // TODO: be extension-agnostic.  This check is here because
  // QImageReader.canRead() will return true for raw files named *.srw, but
  // then QImage will fail to load them.
  if (extension != "jpg" && extension != "jpeg" && extension != "jpe")
    return false;

  QImageReader reader(file.filePath());
  return reader.canRead();
}

Photo::Photo(const QFileInfo& file)
  : MediaSource(file), metadata_(PhotoMetadata::FromFile(file)),
    exposure_date_time_(NULL), edit_revision_(0) {
}

QImage Photo::Image(bool respect_orientation) const {
  QImage master(file().filePath());
  if (!master.isNull() && respect_orientation)
    master = master.transformed(metadata_->orientation_transform());
  
  return master;
}

void Photo::DestroySource(bool destroy_backing, bool as_orphan) {
  // TODO: destroy the database entry
  
  MediaSource::DestroySource(destroy_backing, as_orphan);
}

Orientation Photo::orientation() const {
  return metadata_->orientation();
}

QDateTime Photo::exposure_date_time() const {
  if (exposure_date_time_ == NULL) {
    exposure_date_time_ = (metadata_->exposure_time().isValid()
      ? new QDateTime(metadata_->exposure_time())
      : new QDateTime(file().created()));
  }
  
  return *exposure_date_time_;
}

QUrl Photo::gallery_path() const {
  QUrl url = MediaSource::gallery_path();
  append_edit_revision(url);
  return url;
}

QUrl Photo::gallery_preview_path() const {
  QUrl url = MediaSource::gallery_preview_path();
  append_edit_revision(url);
  return url;
}

void Photo::append_edit_revision(QUrl& url) const {
  // Because of QML's aggressive, opaque caching of loaded images, we need to
  // add an arbitrary URL parameter to gallery_path and gallery_preview_path so
  // that loading the same image after an edit will go back to disk instead of
  // just hitting the cache.
  if (edit_revision_ != 0)
    url.addQueryItem("edit", QString::number(edit_revision_));
}

void Photo::rotateRight() {
  set_orientation(PhotoMetadata::rotate_orientation(orientation(), false));
}

void Photo::rotateLeft() {
  set_orientation(PhotoMetadata::rotate_orientation(orientation(), true));
}

void Photo::set_orientation(Orientation new_orientation) {
  if (new_orientation == orientation())
    return;

  metadata_->set_orientation(new_orientation);
  metadata_->save();

  emit orientation_altered();

  finish_edit();
}

void Photo::finish_edit() {
  notify_data_altered();

  ++edit_revision_;

  emit gallery_path_altered();
  emit gallery_preview_path_altered();
}
