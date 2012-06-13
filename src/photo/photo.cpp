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
#include "database/database.h"

#include <QFileInfo>
#include <QImage>

const QString Photo::SAVE_POINT_DIR = ".temp";
const QString Photo::ORIGINAL_DIR = ".original";

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
    exposure_date_time_(NULL), edit_revision_(0), save_points_(), crop_rect_() {
}

QImage Photo::Image(bool respect_orientation) const {
  return load_image(file(), respect_orientation);
}

void Photo::DestroySource(bool destroy_backing, bool as_orphan) {
  MediaSource::DestroySource(destroy_backing, as_orphan);

  discardOriginal();
}

QImage Photo::load_image(const QFileInfo &file,
    bool respect_orientation) const {
  QImage image(file.filePath());
  if (!image.isNull() && respect_orientation)
    image = image.transformed(metadata_->orientation_transform());

  return image;
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

QRect Photo::crop_rectangle() const {
  return crop_rect_;
}

QUrl Photo::gallery_path() const {
  QUrl url = MediaSource::gallery_path();
  append_edit_revision(&url);
  return url;
}

QUrl Photo::gallery_preview_path() const {
  QUrl url = MediaSource::gallery_preview_path();
  append_edit_revision(&url);
  return url;
}

void Photo::set_crop_rectangle(const QRect& crop_rectangle) {
  crop_rect_ = crop_rectangle;

  if (get_id() != INVALID_ID)
    Database::instance()->get_photo_edit_table()->set_crop_rectangle(
        get_id(), crop_rectangle);
}

void Photo::append_edit_revision(QUrl* url) const {
  // Because of QML's aggressive, opaque caching of loaded images, we need to
  // add an arbitrary URL parameter to gallery_path and gallery_preview_path so
  // that loading the same image after an edit will go back to disk instead of
  // just hitting the cache.
  if (edit_revision_ != 0)
    url->addQueryItem("edit", QString::number(edit_revision_));
}

void Photo::rotateRight() {
  rotate_crop_rect(false);

  Orientation new_orientation =
      PhotoMetadata::rotate_orientation(orientation(), false);

  start_edit();

  set_orientation(new_orientation);

  finish_edit();
}

QVariant Photo::prepareForCropping() {
  start_edit(true);

  create_edited_original(orientation(), QRect());

  finish_edit();

  QRectF ratio_crop_rect(0.25, 0.25, 0.5, 0.5);
  if (crop_rect_.isValid()) {
    QImage image = Image(true);

    qreal x = (qreal)crop_rect_.x() / image.width();
    qreal y = (qreal)crop_rect_.y() / image.height();
    qreal width = (qreal)crop_rect_.width() / image.width();
    qreal height = (qreal)crop_rect_.height() / image.height();

    if (x >= 0.0 && y >= 0.0 && width > 0.0 && height > 0.0 &&
        x + width <= 1.0 && y + height <= 1.0)
      ratio_crop_rect = QRectF(x, y, width, height);
  }

  return QVariant::fromValue(ratio_crop_rect);
}

void Photo::crop(QVariant vrect) {
  QRectF ratio_crop_rect = vrect.toRectF();
  if (!ratio_crop_rect.isValid())
    return;

  QImage image = Image(true);

  // Integer truncation is good enough here.
  int x = ratio_crop_rect.x() * image.width();
  int y = ratio_crop_rect.y() * image.height();
  int width = ratio_crop_rect.width() * image.width();
  int height = ratio_crop_rect.height() * image.height();

  if (x < 0 || y < 0 || width <= 0 || height <= 0
    || x + width > image.width() || y + height > image.height()) {
    qDebug("Invalid cropping rectangle");
    return;
  }

  start_edit();

  internal_crop(QRect(x, y, width, height), image);

  finish_edit();
}

bool Photo::revertToOriginal() {
  QFileInfo original = get_original_file();

  if (!original.exists())
    return false;

  discardSavePoints();
  if (!restore(original))
    return false;

  // Don't need to start_edit(), since this is a revert.
  finish_edit();
  return true;
}

bool Photo::revertToLastSavePoint() {
  if (save_points_.isEmpty())
    return false;

  QFileInfo save_point = save_points_.pop();

  if (!restore(save_point))
    return false;

  // Don't need to start_edit(), since this is a revert.
  // TODO: we may be able to optimize this, so instead of reloading the file
  // from disk again we'd instead go back to a prior edit_version_ that may
  // still be in memory.  This seems good enough for now.
  finish_edit();
  return true;
}

void Photo::discardOriginal() {
  QFile::remove(get_original_file().filePath());
}

void Photo::discardLastSavePoint() {
  if (!save_points_.isEmpty())
    QFile::remove(save_points_.pop().filePath());
}

void Photo::discardSavePoints() {
  while (!save_points_.isEmpty()) {
    QFile::remove(save_points_.pop().filePath());
  }
}

void Photo::set_orientation(Orientation new_orientation) {
  if (new_orientation == orientation())
    return;

  metadata_->set_orientation(new_orientation);
  metadata_->save();

  emit orientation_altered();
}

void Photo::internal_crop(const QRect& crop_rect, const QImage& original) {
  set_crop_rectangle(crop_rect);
  QImage cropped = original.copy(crop_rect);

  // QImage will always give us data in TOP_LEFT_ORIENTATION, but we want to
  // mimic the orientation of the previous version of the file, so we apply the
  // reverse transform before saving.
  QTransform transform = metadata_->orientation_transform();
  cropped = cropped.transformed(transform.inverted());

  cropped.save(file().filePath());
  metadata_->save();
}

void Photo::rotate_crop_rect(bool left) {
  if (crop_rect_.isValid()) {
    QFileInfo original_file = get_original_file();
    QFileInfo crop_file = (original_file.exists() ? original_file : file());
    QImage image = load_image(crop_file, true);

    int x = (left
      ? crop_rect_.y()
      : image.height() - crop_rect_.y() - crop_rect_.height());
    int y = (left
      ? image.width() - crop_rect_.x() - crop_rect_.width()
      : crop_rect_.x());
    int width = crop_rect_.height();
    int height = crop_rect_.width();

    set_crop_rectangle(QRect(x, y, width, height));
  }
}

QFileInfo Photo::get_original_file() const {
  return QFileInfo(file().dir(), QString("%1/%2")
    .arg(ORIGINAL_DIR).arg(file().fileName()));
}

QFileInfo Photo::get_save_point_file(int index) const {
  return QFileInfo(file().dir(), QString("%1/%2_sv%3.%4")
    .arg(SAVE_POINT_DIR).arg(file().completeBaseName()).arg(index).arg(file().completeSuffix()));
}

bool Photo::create_edited_original(Orientation orientation,
    const QRect& crop_rect) {
  QFileInfo original = get_original_file();
  if (!original.exists())
    return false;

  if (!restore(original, true))
    return false;

  set_orientation(orientation);
  if (crop_rect.isValid())
    internal_crop(crop_rect, Image(true));

  return true;
}

bool Photo::restore(const QFileInfo& source, bool leave_source) {
  if (!QFile::remove(file().filePath())) {
    qDebug("Unable to remove edited file %s", qPrintable(file().filePath()));
    return false;
  }

  bool ok = (leave_source
             ? QFile::copy(source.filePath(), file().filePath())
             : QFile::rename(source.filePath(), file().filePath()));
  if (!ok) {
    qDebug("Unable to restore from %s", qPrintable(source.filePath()));
    return false;
  }

  // Since we aren't keeping track of what we've applied to the metadata since
  // the save point, we just reload it from the file we're restoring.  Note
  // that we don't reset exposure_date_time_ so that in case we're using the
  // file modified time, it won't change for the duration of the app.  Not sure
  // if that's ultimately desirable.
  delete metadata_;
  metadata_ = PhotoMetadata::FromFile(file().filePath());

  return true;
}

bool Photo::create_save_point() {
  // TODO: this system of savepoints might be able to go away when we get a
  // proper undo stack.  Then, instead of saving the actual file at particular
  // times, we can just always recreate it from the list of edits that should
  // be applied to the original at that point in time.

  file().dir().mkdir(SAVE_POINT_DIR);
  file().dir().mkdir(ORIGINAL_DIR);

  QFileInfo original = get_original_file();
  int save_point_index = save_points_.count();
  QFileInfo save_point = get_save_point_file(save_point_index);

  if (QFile::rename(file().filePath(), original.filePath())) {
    if (!QFile::copy(original.filePath(), file().filePath())) {
      qDebug("Unable to recreate file %s after creating original",
        qPrintable(file().filePath()));
      return false;
    }
  }

  QFile::remove(save_point.filePath());
  if (!QFile::rename(file().filePath(), save_point.filePath())) {
    qDebug("Unable to create save point %s",
      qPrintable(save_point.filePath()));
    return false;
  }

  // TODO: this may not be necessary with a better framework in place for
  // editing.  Right now, with rotation it only writes the metadata out to the
  // filename it originally read, so we need to make sure the file is still
  // available where it's expecting.  Other operations overwrite the whole
  // file, so this is wasteful in those cases.
  if (!QFile::copy(save_point.filePath(), file().filePath())) {
    qDebug("Unable to recreate file %s after save point creation",
      qPrintable(file().filePath()));
    return false;
  }

  save_points_.push(save_point);
  return true;
}

void Photo::start_edit(bool always_create_save_point) {
  // Only allow one save point unless we're forcing a new one.
  if (always_create_save_point || save_points_.isEmpty())
    create_save_point();
}

void Photo::finish_edit() {
  notify_data_altered();

  ++edit_revision_;

  emit gallery_path_altered();
  emit gallery_preview_path_altered();
}
