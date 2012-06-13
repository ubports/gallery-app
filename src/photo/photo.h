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

#ifndef GALLERY_PHOTO_H_
#define GALLERY_PHOTO_H_

#include <QObject>
#include <QDateTime>
#include <QFileInfo>
#include <QStack>

#include "media/media-source.h"
#include "photo/photo-metadata.h"

class Photo : public MediaSource {
  Q_OBJECT
  
 public:
  static const QString SAVE_POINT_DIR;
  static const QString ORIGINAL_DIR;

  static bool IsValid(const QFileInfo& file);
  
  explicit Photo(const QFileInfo& file);
  
  virtual QImage Image(bool respect_orientation) const;
  virtual Orientation orientation() const;
  virtual QDateTime exposure_date_time() const;
  virtual QRect crop_rectangle() const;

  virtual QUrl gallery_path() const;
  virtual QUrl gallery_preview_path() const;

  void set_crop_rectangle(const QRect& crop_rectangle);

  Q_INVOKABLE void rotateRight();
  Q_INVOKABLE QVariant prepareForCropping(); // Returns crop coords in [0,1].
  Q_INVOKABLE void crop(QVariant vrect); // All coords in [0,1].
  Q_INVOKABLE bool revertToOriginal();
  Q_INVOKABLE bool revertToLastSavePoint();
  Q_INVOKABLE void discardOriginal();
  Q_INVOKABLE void discardLastSavePoint();
  Q_INVOKABLE void discardSavePoints();

 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
 private:
  QImage load_image(const QFileInfo& file, bool respect_orientation) const;
  void set_orientation(Orientation new_orientation);
  void internal_crop(const QRect& crop_rect, const QImage& original);
  void rotate_crop_rect(bool left);
  void append_edit_revision(QUrl* url) const;
  QFileInfo get_original_file() const;
  QFileInfo get_save_point_file(int index) const;
  bool create_edited_original(Orientation orientation, const QRect& crop_rect);
  bool restore(const QFileInfo& source, bool leave_source = false);
  bool create_save_point();
  void start_edit(bool always_create_save_point = false);
  void finish_edit();

  // Go ahead and cache the photo's metadata object inside the photo. Insofar
  // as we know, Gallery will be the only application on the device mutating
  // photo files, so we won't have to worry about the cache going stale.
  PhotoMetadata* metadata_;
  mutable QDateTime *exposure_date_time_;
  int edit_revision_; // How many times the pixel data has been modified by us.
  QStack<QFileInfo> save_points_; // Edits we've saved as files.
  QRect crop_rect_;
};

#endif  // GALLERY_PHOTO_H_
