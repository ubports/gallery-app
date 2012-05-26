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

  static bool IsValid(const QFileInfo& file);
  
  explicit Photo(const QFileInfo& file);
  
  virtual QImage Image(bool respect_orientation) const;
  virtual Orientation orientation() const;
  virtual QDateTime exposure_date_time() const;

  virtual QUrl gallery_path() const;
  virtual QUrl gallery_preview_path() const;

  Q_INVOKABLE void rotateRight();
  Q_INVOKABLE void rotateLeft();
  Q_INVOKABLE bool revertToLastSavePoint();
  Q_INVOKABLE void discardSavePoints();

 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
 private:
  void set_orientation(Orientation new_orientation);
  void append_edit_revision(QUrl& url) const;
  QFileInfo get_save_point_file(int index) const;
  bool create_save_point();
  void start_edit();
  void finish_edit();

  // Go ahead and cache the photo's metadata object inside the photo. Insofar
  // as we know, Gallery will be the only application on the device mutating
  // photo files, so we won't have to worry about the cache going stale.
  PhotoMetadata* metadata_;
  mutable QDateTime *exposure_date_time_;
  int edit_revision_; // How many times the pixel data has been modified by us.
  QStack<QFileInfo> save_points_; // Edits we've saved as files.
};

#endif  // GALLERY_PHOTO_H_
