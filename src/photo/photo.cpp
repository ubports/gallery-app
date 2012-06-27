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
#include "util/imaging.h"

#include <QFileInfo>
#include <QImage>

const QString Photo::SAVE_POINT_DIR = ".temp";
const QString Photo::ORIGINAL_DIR = ".original";
const QString Photo::ENHANCED_DIR = ".enhanced";

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
    exposure_date_time_(NULL), edit_revision_(0), save_points_(), crop_rect_(),
    is_enhanced_(false) {
}

QImage Photo::Image(bool respect_orientation) const {
  return load_image(file(), respect_orientation);
}

void Photo::DestroySource(bool destroy_backing, bool as_orphan) {
  MediaSource::DestroySource(destroy_backing, as_orphan);

  discard_cached_editing_files();
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

void Photo::set_is_enhanced(bool is_enhanced) {
  is_enhanced_ = is_enhanced;

  if (get_id() != INVALID_ID)
    Database::instance()->get_photo_edit_table()->set_is_enhanced(
          get_id(), is_enhanced);
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

  cache_original();
  start_edit(false, false);

  set_orientation(new_orientation);

  finish_edit();
}

void Photo::autoEnhance() {
  if (is_enhanced_)
    return;

  cache_original();
  start_edit(true, true);
  swap_in_original();

  QImage sample_img = QImage(get_original_file().filePath());
  sample_img = sample_img.scaledToWidth(400);
  AutoEnhanceTransformation enhance_txn = AutoEnhanceTransformation(sample_img);

  QImage enhanced_image(file().filePath());
  for (int j = 0; j < enhanced_image.height(); j++) {
    for (int i = 0; i < enhanced_image.width(); i++) {
      QColor px = enhance_txn.transform_pixel(
        QColor(enhanced_image.pixel(i, j)));
      enhanced_image.setPixel(i, j, px.rgb());
    }
  }

  enhanced_image.save(file().filePath(), 0, 99);
  metadata_->save();
  cache_enhanced();

  // if cropped, re-apply the crop after enhancement to make sure that we're
  // cropping on enhanced pixels, not original pixels
  internal_crop(crop_rectangle(),enhanced_image);

  set_is_enhanced(true);

  finish_edit();
}

QVariant Photo::prepareForCropping() {
  cache_original();

  start_edit(true, false);

  swap_in_crop_source();

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

  start_edit(false, false);

  internal_crop(QRect(x, y, width, height), image);

  finish_edit();
}

// Replace the working file with the crop source file, which could either come
// from the .original directory (if the image hasn't been enhanced) or from the
// .enhanced directory (if the image has been enhanced)
void Photo::swap_in_crop_source() {
  if ((!get_original_file().exists()) && (!get_enhanced_file().exists())) {
    // if neither the original file nor the cached enhanced file exists, then
    // we can't do anything, so short-circuit return
    return;
  }

  QString crop_source_path = (is_enhanced_) ? get_enhanced_file().filePath() :
    get_original_file().filePath();
  QFile::remove(file().filePath());
  QFile::copy(crop_source_path, file().filePath());
}

// Replace the working file with original file
void Photo::swap_in_original() {
  if (!get_original_file().exists()) {
    // if the original file doesn't exist, then we can't do anything, so
    // short-circuit return
    return;
  }

  QFile::remove(file().filePath());
  QFile::copy(get_original_file().filePath(), file().filePath());
}

// Save a copy of the working file into the .enhanced directory, creating the
// .enhanced directory if necessary.
void Photo::cache_enhanced() {
  file().dir().mkdir(ENHANCED_DIR);
  QFile::remove(get_enhanced_file().filePath());
  QFile::copy(file().filePath(), get_enhanced_file().filePath());
}

// Save a copy of the working file into the .original directory, creating the
// .original directory if necessary. If a valid original already exists, do
// nothing.
void Photo::cache_original() {
  if (get_original_file().exists())
    return;

  file().dir().mkdir(ORIGINAL_DIR);
  QFile::copy(file().filePath(), get_original_file().filePath());
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

  discard_cached_editing_files();
  set_is_enhanced(false);
  set_crop_rectangle(QRect());

  return true;
}

bool Photo::revertToLastSavePoint() {
  if (save_points_.isEmpty())
    return false;

  SavePoint save_point = save_points_.pop();
  QFileInfo snapshot_file = save_point.snapshot_file_;

  if (save_point.enhance_performed_) {
    discard_enhanced();
    set_is_enhanced(false);
  }

  if (!restore(snapshot_file))
    return false;

  // Don't need to start_edit(), since this is a revert.
  // TODO: we may be able to optimize this, so instead of reloading the file
  // from disk again we'd instead go back to a prior edit_version_ that may
  // still be in memory.  This seems good enough for now.
  finish_edit();
  return true;
}

void Photo::discard_cached_editing_files() {
  discard_enhanced();
  QFile::remove(get_original_file().filePath());
}

void Photo::discard_enhanced() {
  QFile::remove(get_enhanced_file().filePath());
}

void Photo::discardLastSavePoint() {
  if (!save_points_.isEmpty())
    QFile::remove(save_points_.pop().snapshot_file_.filePath());
}

void Photo::discardSavePoints() {
  while (!save_points_.isEmpty()) {
    QFile::remove(save_points_.pop().snapshot_file_.filePath());
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

QFileInfo Photo::get_enhanced_file() const {
  return QFileInfo(file().dir(), QString("%1/%2")
    .arg(ENHANCED_DIR).arg(file().fileName()));
}

QFileInfo Photo::get_save_point_file(int index) const {
  return QFileInfo(file().dir(), QString("%1/%2_sv%3.%4")
    .arg(SAVE_POINT_DIR).arg(file().completeBaseName()).arg(index).arg(file().completeSuffix()));
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

bool Photo::create_save_point(bool is_pre_enhance) {
  // TODO: this system of savepoints might be able to go away when we get a
  // proper undo stack.  Then, instead of saving the actual file at particular
  // times, we can just always recreate it from the list of edits that should
  // be applied to the original at that point in time.

  file().dir().mkdir(SAVE_POINT_DIR);

  int save_point_index = save_points_.count();
  QFileInfo save_point_file = get_save_point_file(save_point_index);

  QFile::remove(save_point_file.filePath());
  if (!QFile::rename(file().filePath(), save_point_file.filePath())) {
    qDebug("Unable to create save point %s",
      qPrintable(save_point_file.filePath()));
    return false;
  }

  // TODO: this may not be necessary with a better framework in place for
  // editing.  Right now, with rotation it only writes the metadata out to the
  // filename it originally read, so we need to make sure the file is still
  // available where it's expecting.  Other operations overwrite the whole
  // file, so this is wasteful in those cases.
  if (!QFile::copy(save_point_file.filePath(), file().filePath())) {
    qDebug("Unable to recreate file %s after save point creation",
      qPrintable(file().filePath()));
    return false;
  }

  save_points_.push(SavePoint(save_point_file, is_pre_enhance));
  return true;
}

void Photo::start_edit(bool always_create_save_point, bool is_enhance) {
  // Only allow one save point unless we're forcing a new one.
  if (always_create_save_point || save_points_.isEmpty())
    create_save_point(is_enhance);
}

void Photo::finish_edit() {
  notify_data_altered();

  ++edit_revision_;

  emit gallery_path_altered();
  emit gallery_preview_path_altered();
}
