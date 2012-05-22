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

#ifndef GALLERY_MEDIA_SOURCE_H_
#define GALLERY_MEDIA_SOURCE_H_

#include <QObject>
#include <QDate>
#include <QDateTime>
#include <QFileInfo>
#include <QImage>
#include <QTime>
#include <QUrl>
#include <QVariant>
#include <QtDeclarative>

#include "core/data-source.h"
#include "photo/photo-metadata.h"

class Event;

typedef DataObjectNumber MediaNumber;

class MediaSource : public DataSource {
  Q_OBJECT
  Q_PROPERTY(QUrl path READ path NOTIFY path_altered)
  Q_PROPERTY(QUrl previewPath READ preview_path NOTIFY preview_path_altered)
  Q_PROPERTY(QUrl galleryPath READ gallery_path NOTIFY gallery_path_altered)
  Q_PROPERTY(QUrl galleryPreviewPath READ gallery_preview_path NOTIFY gallery_preview_path_altered)
  Q_PROPERTY(int orientation READ orientation NOTIFY orientation_altered)
  Q_PROPERTY(QDate exposureDate READ exposure_date NOTIFY exposure_date_time_altered)
  Q_PROPERTY(QTime exposureTimeOfDay READ exposure_time_of_day NOTIFY exposure_date_time_altered)
  Q_PROPERTY(int exposure_time_t READ exposure_time_t NOTIFY exposure_date_time_altered)
  Q_PROPERTY(QVariant event READ QmlFindEvent NOTIFY event_changed)
  
 signals:
  void path_altered();
  void preview_path_altered();
  void gallery_path_altered();
  void gallery_preview_path_altered();
  void orientation_altered();
  void exposure_date_time_altered();
  void event_changed();
  void data_altered();
  
 public:
  MediaSource();
  explicit MediaSource(const QFileInfo& file);
  
  static void RegisterType();
  
  QFileInfo file() const;
  QUrl path() const;
  virtual QUrl gallery_path() const;
  
  QFileInfo preview_file() const;
  QUrl preview_path() const;
  virtual QUrl gallery_preview_path() const;
  
  virtual QImage Image(bool respect_orientation = true) const;
  
  virtual Orientation orientation() const;
  
  virtual QDateTime exposure_date_time() const;
  QDate exposure_date() const;
  QTime exposure_time_of_day() const;
  int exposure_time_t() const;
  
  Event* FindEvent();
  QVariant QmlFindEvent();
  
 protected:
  virtual void DestroySource(bool delete_backing, bool as_orphan);

  virtual void notify_data_altered();
  
 private:
  QFileInfo file_;
};

QML_DECLARE_TYPE(MediaSource)

#endif  // GALLERY_MEDIA_SOURCE_H_
