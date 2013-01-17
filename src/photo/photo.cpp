/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 * Clint Rogers <clinton@yorba.org>
 */

#include "photo/photo.h"

#include "database/database.h"
#include "media/preview-manager.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include "util/imaging.h"
#include "core/gallery-manager.h"

#include <QFileInfo>
#include <QImage>

const char* Photo::SIZE_KEY = "size_level";

bool Photo::IsValid(const QFileInfo& file) {
  QImageReader reader(file.filePath());
  QByteArray format = reader.format();

  if (QString(format).toLower() == "tiff") {
    // QImageReader.canRead() will detect some raw files as readable TIFFs,
    // though QImage will fail to load them.
    QString extension = file.suffix().toLower();
    if (extension != "tiff" && extension != "tif")
      return false;
  }

  PhotoMetadata* tmp = PhotoMetadata::FromFile(file);
  if (tmp == NULL)
    return false;

  delete tmp;
  return reader.canRead() &&
      QImageWriter::supportedImageFormats().contains(reader.format());
}

Photo* Photo::Load(const QFileInfo& file) {
  bool needs_update = false;
  PhotoEditState edit_state;
  QDateTime timestamp;
  QDateTime exposure_time;
  QSize size;
  Orientation orientation;
  qint64 filesize;
  
  // Look for photo in the database.
  qint64 id = GalleryManager::GetInstance()->database()->get_media_table()->get_id_for_media(
    file.absoluteFilePath());

  if (id == INVALID_ID && !IsValid(file))
    return NULL;
  
  Photo* p = new Photo(file);
  
  // Check for legacy rows.
  if (id != INVALID_ID)
    needs_update = GalleryManager::GetInstance()->database()->get_media_table()->row_needs_update(id);
  
  // If we don't have the photo, add it to the DB.  If we have the photo but the
  // row is from a previous version of the DB, update the row.
  if (id == INVALID_ID || needs_update) {
    PhotoMetadata* metadata = PhotoMetadata::FromFile(p->caches_.pristine_file());
    if (metadata == NULL) {
      delete p;
      return NULL;
    }

    timestamp = p->caches_.pristine_file().lastModified();
    orientation = p->file_format_has_orientation()
      ? metadata->orientation() : TOP_LEFT_ORIGIN;
    filesize = p->caches_.pristine_file().size();
    exposure_time = metadata->exposure_time().isValid() ?
      QDateTime(metadata->exposure_time()) : timestamp;

    if (needs_update) {
      // Update DB.
      GalleryManager::GetInstance()->database()->get_media_table()->update_media(id,
        file.absoluteFilePath(), timestamp, exposure_time, orientation, filesize);
    } else {
      // Add to DB.
      id = GalleryManager::GetInstance()->database()->get_media_table()->create_id_for_media(
        file.absoluteFilePath(), timestamp, exposure_time, orientation, filesize);
    }

    delete metadata;
  } else {
    // Load metadata from DB.
    GalleryManager::GetInstance()->database()->get_media_table()->get_row(id, size, orientation,
      timestamp, exposure_time);
    edit_state = GalleryManager::GetInstance()->database()->get_photo_edit_table()->get_edit_state(id);
  }
  
  // Populate photo object.
  if (size.isValid())
    p->set_size(size);
  p->set_base_edit_state(edit_state);
  p->set_original_orientation(orientation);
  p->set_file_timestamp(timestamp);
  p->set_exposure_date_time(exposure_time);
  
  // We set the id last so we don't save the info we just read in back out to
  // the DB.
  p->set_id(id);
  
  return p;
}

Photo* Photo::Fetch(const QFileInfo& file) {
  GalleryManager* gallery_mgr = GalleryManager::GetInstance();

  Photo* p = gallery_mgr->media_collection()->photoFromFileinfo(file);
  if (p == NULL) {
    p = Load(file);
  }

  return p;
}

Photo::Photo(const QFileInfo& file)
  : MediaSource(file),
    exposure_date_time_(),
    edit_revision_(0),
    edits_(),
    saved_state_(),
    caches_(file),
    original_size_(),
    original_orientation_(TOP_LEFT_ORIGIN) {
  QByteArray format = QImageReader(file.filePath()).format();
  file_format_ = QString(format).toLower();
  if (file_format_ == "jpg") // Why does Qt expose two different names here?
    file_format_ = "jpeg";
}

Photo::~Photo() {
}

QImage Photo::Image(bool respect_orientation) {
  QImage image(file().filePath(), file_format_.toStdString().c_str());
  if (!image.isNull() && respect_orientation && file_format_has_orientation()) {
    image = image.transformed(
        OrientationCorrection::FromOrientation(orientation())
        .to_transform());

    // Cache this here since the image is already loaded.
    if (!is_size_set())
      set_size(image.size());
  }

  return image;
}

Orientation Photo::orientation() const {
  return (current_state().orientation_ == PhotoEditState::ORIGINAL_ORIENTATION) ? 
    original_orientation_ : current_state().orientation_;
}

QDateTime Photo::exposure_date_time() const {
  return exposure_date_time_;
}

QUrl Photo::gallery_path() const {
  QUrl url = MediaSource::gallery_path();
  // We don't pass the orientation in if we saved the file already rotated,
  // which is the case if the file format can't store rotation metadata.
  append_path_params(&url, (file_format_has_orientation() ? orientation() : TOP_LEFT_ORIGIN), 0);
  
  return url;
}

QUrl Photo::gallery_preview_path() const {
  QUrl url = MediaSource::gallery_preview_path();
  // previews are always stored fully transformed

  append_path_params(&url, TOP_LEFT_ORIGIN, 1);
  
  return url;
}

QUrl Photo::gallery_thumbnail_path() const {
  QUrl url = MediaSource::gallery_thumbnail_path();
  // same as in append_path_params() this is needed to trigger an update of the image in QML
  // so the URL is changed by adding/chageing the edit parameter
  QUrlQuery query;
  if (edit_revision_ != 0) {
    query.addQueryItem(GalleryThumbnailImageProvider::REVISION_PARAM_NAME,
      QString::number(edit_revision_));
  }
  url.setQuery(query);
  return url;
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

  QSize size = get_original_size(orientation());
  
  // A PhotoEditState object with an invalid orientation value (i.e. <
  // MIN_ORIENTATION) means "use the existing (original) orientation", so
  // set the current edit state's orientation to this photo object's
  // orientation
  PhotoEditState curr_state = current_state();
  if (curr_state.orientation_ < MIN_ORIENTATION)
    curr_state.orientation_ = orientation();
  
  PhotoEditState next_state = curr_state.rotate(new_orientation, size.width(),
    size.height());

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

  QRectF ratio_crop_rect(0.0, 0.0, 1.0, 1.0);
  if (current_state().crop_rectangle_.isValid()) {
    QSize image_size = get_original_size(current_state().orientation_);

    QRect rect = current_state().crop_rectangle_;
    qreal x = (qreal)rect.x() / image_size.width();
    qreal y = (qreal)rect.y() / image_size.height();
    qreal width = (qreal)rect.width() / image_size.width();
    qreal height = (qreal)rect.height() / image_size.height();

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

  QSize image_size = get_original_size(current_state().orientation_);

  // Integer truncation is good enough here.
  int x = ratio_crop_rect.x() * image_size.width();
  int y = ratio_crop_rect.y() * image_size.height();
  int width = ratio_crop_rect.width() * image_size.width();
  int height = ratio_crop_rect.height() * image_size.height();

  if (x < 0 || y < 0 || width <= 0 || height <= 0
    || x + width > image_size.width() || y + height > image_size.height()) {
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

QSize Photo::get_original_size(Orientation orientation) {
  if (!original_size_.isValid()) {
    QImage original(caches_.pristine_file().filePath(),
                    file_format_.toStdString().c_str());
    if (file_format_has_orientation()) {
      original =
          original.transformed(OrientationCorrection::FromOrientation(
            original_orientation_).to_transform());
    }
    
    original_size_ = original.size();
  }

  QSize rotated_size = original_size_;

  if (orientation != PhotoEditState::ORIGINAL_ORIENTATION) {
    OrientationCorrection original_correction =
        OrientationCorrection::FromOrientation(original_orientation_);
    OrientationCorrection out_correction =
        OrientationCorrection::FromOrientation(orientation);
    int degrees_rotation =
        original_correction.get_normalized_rotation_difference(out_correction);

    if (degrees_rotation == 90 || degrees_rotation == 270)
      rotated_size.transpose();
  }

  return rotated_size;
}

void Photo::make_undoable_edit(const PhotoEditState& state) {
  Orientation old_orientation = orientation();

  edits_.push_edit(state);
  save(state, old_orientation);
}

void Photo::save(const PhotoEditState& state, Orientation old_orientation) {
  edit_file(state);
 GalleryManager::GetInstance()->database()->get_photo_edit_table()->set_edit_state(get_id(), state);

  if (orientation() != old_orientation)
    emit orientation_altered();
  notify_data_altered();

  ++edit_revision_;

  emit gallery_path_altered();
  emit gallery_preview_path_altered();
  emit gallery_thumbnail_path_altered();
}

// Handler for the case of an image whose only change is to its 
// orientation; used to skip re-encoding of JPEGs.
void Photo::handle_simple_metadata_rotation(const PhotoEditState& state) {
  PhotoMetadata* metadata = PhotoMetadata::FromFile(file());
  metadata->set_orientation(state.orientation_);
  
  metadata->save();
  delete(metadata);

  OrientationCorrection orig_correction = 
    OrientationCorrection::FromOrientation(original_orientation_);
  OrientationCorrection dest_correction = 
    OrientationCorrection::FromOrientation(state.orientation_);

  QSize new_size = original_size_;
  int angle = dest_correction.get_normalized_rotation_difference(orig_correction); 

  if ((angle == 90) || (angle == 270)) {
    new_size = original_size_.transposed();
  }

  set_size(new_size);
}

void Photo::edit_file(const PhotoEditState& state) {
  // As a special case, if editing to the original version, we simply restore
  // from the original and call it a day.
  if (state.is_original()) {
    if (!caches_.restore_original())
      qDebug("Error restoring original for %s", qPrintable(file().filePath()));
    else
      set_size(get_original_size(PhotoEditState::ORIGINAL_ORIENTATION));

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

  // Have we been rotated and _not_ cropped?
  if (file_format_has_orientation() && (!state.crop_rectangle_.isValid()) &&
      (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)) { 
    // Yes; skip out on decoding and re-encoding the image.
    handle_simple_metadata_rotation(state);
    return;
  }

  // TODO: we might be able to avoid reading/writing pixel data (and other
  // more general optimizations) under certain conditions here.  Might be worth
  // doing if it doesn't make the code too much worse.
  // 
  // At the moment, we are skipping at least one decode and one encode in cases
  // where a .jpeg file has been rotated, but not cropped, since rotation can be 
  // controlled by manipulating its metadata without having to modify pixel data;
  // please see the method handle_simple_metadata_rotation() for details.

  QImage image(file().filePath(), file_format_.toStdString().c_str());
  if (image.isNull()) {
    qDebug("Error loading %s for editing", qPrintable(file().filePath()));
    return;
  }
  PhotoMetadata* metadata = PhotoMetadata::FromFile(file());

  if (file_format_has_orientation() &&
      state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)
    metadata->set_orientation(state.orientation_);

  if (file_format_has_orientation() &&
      metadata->orientation() != TOP_LEFT_ORIGIN)
    image = image.transformed(metadata->orientation_transform());
  else if (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION &&
      state.orientation_ != TOP_LEFT_ORIGIN)
    image = image.transformed(
        OrientationCorrection::FromOrientation(state.orientation_).to_transform());

  // Cache this here so we may be able to avoid another JPEG decode later just
  // to find the dimensions.
  if (!original_size_.isValid())
    original_size_ = image.size();

  if (state.crop_rectangle_.isValid())
    image = image.copy(state.crop_rectangle_);

  QSize new_size = image.size();

  // We need to apply the reverse transformation so that when we reload the
  // file and reapply the transformation it comes out correctly.
  if (file_format_has_orientation() &&
      metadata->orientation() != TOP_LEFT_ORIGIN)
    image = image.transformed(metadata->orientation_transform().inverted());

  bool saved = image.save(file().filePath(), file_format_.toStdString().c_str(), 90);
  if (saved && file_format_has_metadata())
    saved = metadata->save();
  if (!saved)
    qDebug("Error saving edited %s", qPrintable(file().filePath()));

  delete metadata;

  set_size(new_size);
}

void Photo::create_cached_enhanced() {
  if (!caches_.cache_enhanced_from_original()) {
    qDebug("Error creating enhanced file for %s", qPrintable(file().filePath()));
    return;
  }
  
  set_busy(true);
  
  QFileInfo to_enhance = caches_.enhanced_file();
  PhotoMetadata* metadata = PhotoMetadata::FromFile(to_enhance);

  QImage unenhanced_img(to_enhance.filePath(), file_format_.toStdString().c_str());
  int width = unenhanced_img.width();
  int height = unenhanced_img.height();

  QImage sample_img = (unenhanced_img.width() > 400) ? 
      unenhanced_img.scaledToWidth(400) : unenhanced_img;
 
  AutoEnhanceTransformation enhance_txn = AutoEnhanceTransformation(sample_img);

  QImage::Format dest_format = unenhanced_img.format();

  // Can't write into indexed images, due to a limitation in Qt.
  if (dest_format == QImage::Format_Indexed8)
    dest_format = QImage::Format_RGB32;
  
  QImage enhanced_image(width, height, dest_format);

  for (int j = 0; j < height; j++) {
    QApplication::processEvents();
    for (int i = 0; i < width; i++) {
      QColor px = enhance_txn.transform_pixel(
        QColor(unenhanced_img.pixel(i, j)));
      enhanced_image.setPixel(i, j, px.rgb());
    }
  }

  bool saved = enhanced_image.save(to_enhance.filePath(),
                                   file_format_.toStdString().c_str(), 99);
  if (saved && file_format_has_metadata())
    saved = metadata->save();
  if (!saved) {
    qDebug("Error saving enhanced file for %s", qPrintable(file().filePath()));
    caches_.discard_cached_enhanced();
  }
  
  set_busy(false);
  
  delete metadata;
}

/*!
 * \brief Photo::append_path_params is called by either gallery_path or gallery_preview_path depending on what kind of photo.
 * \brief This sets our size_level parameter which will dictate what sort of image is eventually created.
 * \param url is the picture's url.
 * \param orientation of the image.
 * \param size_level dictates whether or not the image is a full sized picture or a thumbnail. 0 == full sized, 1 == preview.
 */

void Photo::append_path_params(QUrl* url, Orientation orientation, const int size_level) const {
  QUrlQuery query;
  query.addQueryItem(SIZE_KEY, QString::number(size_level));
  query.addQueryItem(GalleryStandardImageProvider::ORIENTATION_PARAM_NAME, QString::number(orientation));
  
  // Because of QML's aggressive, opaque caching of loaded images, we need to
  // add an arbitrary URL parameter to gallery_path and gallery_preview_path so
  // that loading the same image after an edit will go back to disk instead of
  // just hitting the cache.
  if (edit_revision_ != 0) {
    query.addQueryItem(GalleryStandardImageProvider::REVISION_PARAM_NAME,
      QString::number(edit_revision_));
  }
  
  url->setQuery(query);
}

bool Photo::file_format_has_metadata() const {
  return (file_format_ == "jpeg" || file_format_ == "tiff" ||
          file_format_ == "png");
}

bool Photo::file_format_has_orientation() const {
  return (file_format_ == "jpeg");
}

void Photo::set_original_orientation(Orientation orientation) {
  original_orientation_ = orientation;
}

void Photo::set_file_timestamp(const QDateTime& timestamp) {
  file_timestamp_ = timestamp;
}

void Photo::set_exposure_date_time(const QDateTime& exposure_time) {
  if (exposure_date_time_ == exposure_time)
    return;
  
  exposure_date_time_ = exposure_time;
  emit exposure_date_time_altered();
}
