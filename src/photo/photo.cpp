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
  : MediaSource(file), metadata_(PhotoMetadata::FromFile(file)), exposure_date_time_(NULL) {
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
