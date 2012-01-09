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

#ifndef GALLERY_QML_MEDIA_SOURCE_H_
#define GALLERY_QML_MEDIA_SOURCE_H_

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QtDeclarative>

#include "media/media-source.h"
#include "photo/photo-metadata.h"

class QmlMediaSource : public QObject {
  Q_OBJECT
  Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
  Q_PROPERTY(QUrl previewPath READ preview_path NOTIFY previewPathChanged)
  Q_PROPERTY(int orientation READ orientation NOTIFY orientationChanged)
  Q_PROPERTY(float orientationRotation READ orientation_rotation NOTIFY orientationRotationChanged)
  Q_PROPERTY(bool orientationMirrored READ orientation_mirrored NOTIFY orientationMirroredChanged)
  Q_PROPERTY(bool isRotated READ is_rotated NOTIFY orientationRotationChanged)
  
 signals:
  void pathChanged();
  void previewPathChanged();
  void orientationChanged();
  void orientationRotationChanged();
  void orientationMirroredChanged();
  
 public:
  QmlMediaSource();
  virtual ~QmlMediaSource();
  
  static void RegisterType();
  static QVariant AsVariant(MediaSource* source);
  
  // Required because QML types must have default constructors
  void Init(MediaSource* source);
  
  QUrl path() const;
  QUrl preview_path() const;
  
  Orientation orientation() const;
  float orientation_rotation() const;
  bool orientation_mirrored() const;
  bool is_rotated() const;
  
 private:
  MediaSource* source_;
};

QML_DECLARE_TYPE(QmlMediaSource);

#endif  // GALLERY_QML_MEDIA_SOURCE_H_
