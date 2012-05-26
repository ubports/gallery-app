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

const QString Photo::SAVE_POINT_DIR = ".savepoints";

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
    exposure_date_time_(NULL), edit_revision_(0), save_points_() {
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

bool Photo::revertToLastSavePoint() {
  if (save_points_.isEmpty())
    return false;

  QFileInfo save_point = save_points_.pop();

  if (!QFile::remove(file().filePath())) {
    qDebug("Unable to remove edited file %s to revert to a save point",
      qPrintable(file().filePath()));
    return false;
  }

  if (!QFile::rename(save_point.filePath(), file().filePath())) {
    qDebug("Unable to restore save point %s",
      qPrintable(save_point.filePath()));
    return false;
  }

  // Since we aren't keeping track of what we've applied to the metadata since
  // the save point, we just reload it from the file we're restoring.  Note
  // that we don't reset exposure_date_time_ so that in case we're using the
  // file modified time, it won't change for the duration of the app.  Not sure
  // if that's ultimately desirable.
  delete metadata_;
  metadata_ = PhotoMetadata::FromFile(file().filePath());

  // TODO: we may be able to optimize this, so instead of reloading the file
  // from disk again we'd instead go back to a prior edit_version_ that may
  // still be in memory.  This seems good enough for now.
  finish_edit();
  return true;
}

void Photo::discardSavePoints() {
  while(!save_points_.isEmpty()) {
    QFile::remove(save_points_.pop().filePath());
  }
}

void Photo::set_orientation(Orientation new_orientation) {
  if (new_orientation == orientation())
    return;

  start_edit();

  metadata_->set_orientation(new_orientation);
  metadata_->save();

  emit orientation_altered();

  finish_edit();
}

QFileInfo Photo::get_save_point_file(int index) const {
  return QFileInfo(file().dir(), QString("%1/%2_sv%3.%4")
    .arg(SAVE_POINT_DIR).arg(file().completeBaseName()).arg(index).arg(file().completeSuffix()));
}

bool Photo::create_save_point() {
  file().dir().mkdir(SAVE_POINT_DIR);

  int index = save_points_.count();
  QFileInfo save_point = get_save_point_file(index);

  if (!QFile::rename(file().filePath(), save_point.filePath())) {
    qDebug("Unable to create save point %s",
      qPrintable(save_point.filePath()));
    return false;
  }

  // TODO: this may not be necessary with a better framework in place for
  // editing.  Right now, with rotation being the only supported operation, it
  // only writes the metadata out to the filename it originally read, so we
  // need to make sure the file is still available where it's expecting.
  if (!QFile::copy(save_point.filePath(), file().filePath())) {
    qDebug("Unable to recreate file %s after save point creation",
      qPrintable(file().filePath()));
    return false;
  }

  save_points_.push(save_point);
  return true;
}

void Photo::start_edit() {
  // For now, we only allow one save point to exist at a time.
  if (save_points_.isEmpty())
    create_save_point();
}

void Photo::finish_edit() {
  notify_data_altered();

  ++edit_revision_;

  emit gallery_path_altered();
  emit gallery_preview_path_altered();
}
