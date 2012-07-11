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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "photo/photo.h"
#include "database/database.h"
#include "util/imaging.h"

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
  : MediaSource(file),
    exposure_date_time_(NULL),
    edit_revision_(0),
    edits_(),
    saved_state_(),
    caches_(file),
    original_width_(0),
    original_height_(0),
    width_(0),
    height_(0) {
  original_metadata_ = PhotoMetadata::FromFile(caches_.pristine_file());
}

Photo::~Photo() {
  delete original_metadata_;
  delete exposure_date_time_;
}

QImage Photo::Image(bool respect_orientation) const {
  QImage image(file().filePath());
  if (!image.isNull() && respect_orientation)
    image = image.transformed(
        OrientationCorrection::FromOrientation(orientation())
        .to_transform());

  return image;
}

int Photo::width() {
  if (width_ <= 0 || height_ <= 0) {
    QImage image = Image(true);
    width_ = image.width();
    height_ = image.height();
  }

  return width_;
}

int Photo::height() {
  width(); // Ensures width_ and height_ are set.

  return height_;
}

Orientation Photo::orientation() const {
  if (current_state().orientation_ == PhotoEditState::ORIGINAL_ORIENTATION)
    return original_metadata_->orientation();

  return current_state().orientation_;
}

QDateTime Photo::exposure_date_time() const {
  if (exposure_date_time_ == NULL) {
    exposure_date_time_ = (original_metadata_->exposure_time().isValid()
      ? new QDateTime(original_metadata_->exposure_time())
      : new QDateTime(file().created()));
  }
  
  return *exposure_date_time_;
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

Orientation Photo::get_base_orientation() const {
  // If we've cached the original, we assume the photo has been edited.
  if (caches_.has_cached_original())
    return orientation();

  return PhotoEditState::ORIGINAL_ORIENTATION;
}

void Photo::set_base_edit_state(const PhotoEditState& base) {
  edits_.set_base(base);
}

void Photo::saveState() {
  saved_state_ = current_state();
}

void Photo::revertToSavedState() {
  if (saved_state_ != current_state())
    make_undoable_edit(saved_state_);
}

void Photo::revertToOriginal() {
  if (!current_state().is_original())
    make_undoable_edit(PhotoEditState());
}

void Photo::undo() {
  Orientation old_orientation = orientation();

  PhotoEditState prev = edits_.current();
  PhotoEditState next = edits_.undo();
  if (next != prev)
    save(next, old_orientation);
}

void Photo::redo() {
  Orientation old_orientation = orientation();

  PhotoEditState prev = edits_.current();
  PhotoEditState next = edits_.redo();
  if (next != prev)
    save(next, old_orientation);
}

void Photo::rotateRight() {
  Orientation new_orientation =
      PhotoMetadata::rotate_orientation(orientation(), false);

  int width, height;
  get_original_dimensions(&width, &height, orientation());
  PhotoEditState next_state =
      current_state().rotate(new_orientation, width, height);

  make_undoable_edit(next_state);
}

void Photo::autoEnhance() {
  if (current_state().is_enhanced_)
    return;

  PhotoEditState next_state = current_state();
  next_state.is_enhanced_ = true;

  make_undoable_edit(next_state);
}

QVariant Photo::prepareForCropping() {
  // We COULD optimize this out if it hasn't been cropped yet, but I rely on
  // this creating an undoable edit (to make the cancel button in the crop tool
  // function correctly) so we do an edit anyway.

  QRectF ratio_crop_rect(0.25, 0.25, 0.5, 0.5);
  if (current_state().crop_rectangle_.isValid()) {
    int image_width, image_height;
    get_original_dimensions(&image_width, &image_height,
                            current_state().orientation_);

    QRect rect = current_state().crop_rectangle_;
    qreal x = (qreal)rect.x() / image_width;
    qreal y = (qreal)rect.y() / image_height;
    qreal width = (qreal)rect.width() / image_width;
    qreal height = (qreal)rect.height() / image_height;

    if (x >= 0.0 && y >= 0.0 && width > 0.0 && height > 0.0 &&
        x + width <= 1.0 && y + height <= 1.0)
      ratio_crop_rect = QRectF(x, y, width, height);
  }

  PhotoEditState next_state = current_state();
  next_state.crop_rectangle_ = QRect();

  make_undoable_edit(next_state);

  return QVariant::fromValue(ratio_crop_rect);
}

void Photo::cancelCropping() {
  undo();
  edits_.clear_redos();
}

void Photo::crop(QVariant vrect) {
  QRectF ratio_crop_rect = vrect.toRectF();

  int image_width, image_height;
  get_original_dimensions(&image_width, &image_height,
                          current_state().orientation_);

  // Integer truncation is good enough here.
  int x = ratio_crop_rect.x() * image_width;
  int y = ratio_crop_rect.y() * image_height;
  int width = ratio_crop_rect.width() * image_width;
  int height = ratio_crop_rect.height() * image_height;

  if (x < 0 || y < 0 || width <= 0 || height <= 0
    || x + width > image_width || y + height > image_height) {
    qDebug("Invalid cropping rectangle");
    undo(); // Go back to the state before prepareForCropping() was called.
    return;
  }

  PhotoEditState next_state = current_state();
  next_state.crop_rectangle_ = QRect(x, y, width, height);

  // We replace the top of the undo stack (which came from prepareForCropping)
  // with the cropped version.
  edits_.undo();
  make_undoable_edit(next_state);
}

void Photo::DestroySource(bool destroy_backing, bool as_orphan) {
  MediaSource::DestroySource(destroy_backing, as_orphan);

  caches_.discard_all();
}

const PhotoEditState& Photo::current_state() const {
  return edits_.current();
}

void Photo::get_original_dimensions(int* width, int* height,
                                    Orientation orientation) {
  if (original_width_ <= 0 || original_height_ <= 0) {
    QImage original(caches_.pristine_file().filePath());
    original =
        original.transformed(original_metadata_->orientation_transform());

    original_width_ = original.width();
    original_height_ = original.height();
  }

  int rotated_width = original_width_;
  int rotated_height = original_height_;

  OrientationCorrection original_correction =
      OrientationCorrection::FromOrientation(original_metadata_->orientation());
  OrientationCorrection out_correction =
      OrientationCorrection::FromOrientation(orientation);
  int degrees_rotation =
      original_correction.get_normalized_rotation_difference(out_correction);

  if (degrees_rotation == 90 || degrees_rotation == 270)
    std::swap(rotated_width, rotated_height);

  if (width)
    *width = rotated_width;
  if (height)
    *height = rotated_height;
}

void Photo::make_undoable_edit(const PhotoEditState& state) {
  Orientation old_orientation = orientation();

  edits_.push_edit(state);
  save(state, old_orientation);
}

void Photo::save(const PhotoEditState& state, Orientation old_orientation) {
  edit_file(state);
  Database::instance()->get_photo_edit_table()->set_edit_state(get_id(), state);

  if (orientation() != old_orientation)
    emit orientation_altered();
  notify_data_altered();

  ++edit_revision_;

  emit gallery_path_altered();
  emit gallery_preview_path_altered();
}

void Photo::edit_file(const PhotoEditState& state) {
  // As a special case, if editing to the original version, we simply restore
  // from the original and call it a day.
  if (state.is_original()) {
    if (!caches_.restore_original())
      qDebug("Error restoring original for %s", qPrintable(file().filePath()));
    // As a courtesy, when the original goes away, we get rid of the other
    // cached files too.
    caches_.discard_cached_enhanced();
    return;
  }

  if (!caches_.cache_original())
    qDebug("Error caching original for %s", qPrintable(file().filePath()));

  if (state.is_enhanced_ && !caches_.has_cached_enhanced())
    create_cached_enhanced();

  if (!caches_.overwrite_from_cache(state.is_enhanced_))
    qDebug("Error overwriting %s from cache", qPrintable(file().filePath()));

  // TODO: we might be able to avoid reading/writing pixel data (and other
  // more general optimizations) under certain conditions here.  Might be worth
  // doing if it doesn't make the code too much worse.

  QImage image(file().filePath());
  if (image.isNull()) {
    qDebug("Error loading %s for editing", qPrintable(file().filePath()));
    return;
  }
  PhotoMetadata* metadata = PhotoMetadata::FromFile(file());

  if (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)
    metadata->set_orientation(state.orientation_);
  if (metadata->orientation() != TOP_LEFT_ORIGIN)
    image = image.transformed(metadata->orientation_transform());

  // Cache these here so we may be able to avoid another JPEG decode later just
  // to find the dimensions.
  if (original_width_ <= 0 || original_height_ <= 0) {
    original_width_ = image.width();
    original_height_ = image.height();
  }

  if (state.crop_rectangle_.isValid())
    image = image.copy(state.crop_rectangle_);

  width_ = image.width();
  height_ = image.height();

  // We need to apply the reverse transformation so that when we reload the
  // file and reapply the transformation it comes out correctly.
  if (metadata->orientation() != TOP_LEFT_ORIGIN)
    image = image.transformed(metadata->orientation_transform().inverted());

  if (!image.save(file().filePath()) || !metadata->save())
    qDebug("Error saving edited %s", qPrintable(file().filePath()));

  delete metadata;
}

void Photo::create_cached_enhanced() {
  if (!caches_.cache_enhanced_from_original()) {
    qDebug("Error creating enhanced file for %s", qPrintable(file().filePath()));
    return;
  }

  QFileInfo to_enhance = caches_.enhanced_file();
  PhotoMetadata* metadata = PhotoMetadata::FromFile(to_enhance);

  QImage sample_img(to_enhance.filePath());
  int width = sample_img.width();
  int height = sample_img.height();

  if (sample_img.width() > 400)
    sample_img = sample_img.scaledToWidth(400);
  AutoEnhanceTransformation enhance_txn = AutoEnhanceTransformation(sample_img);

  // TODO: may be able to avoid doing another JPEG load here by creating the
  // enhanced_image from scratch here with a different constructor.
  QImage enhanced_image(to_enhance.filePath());
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      QColor px = enhance_txn.transform_pixel(
        QColor(enhanced_image.pixel(i, j)));
      enhanced_image.setPixel(i, j, px.rgb());
    }
  }

  if (!enhanced_image.save(to_enhance.filePath(), 0, 99) ||
      !metadata->save()) {
    qDebug("Error saving enhanced file for %s", qPrintable(file().filePath()));
    caches_.discard_cached_enhanced();
  }

  delete metadata;
}

void Photo::append_edit_revision(QUrl* url) const {
  // Because of QML's aggressive, opaque caching of loaded images, we need to
  // add an arbitrary URL parameter to gallery_path and gallery_preview_path so
  // that loading the same image after an edit will go back to disk instead of
  // just hitting the cache.
  if (edit_revision_ != 0)
    url->addQueryItem("edit", QString::number(edit_revision_));
}
