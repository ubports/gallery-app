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

#include "qml-media-source.h"

#include <QtDeclarative>

QmlMediaSource::QmlMediaSource() {
}

QmlMediaSource::~QmlMediaSource() {
  qDebug("DTOR: QmlMediaSource");
}

void QmlMediaSource::Init(MediaSource* source) {
  source_ = source;
}

void QmlMediaSource::RegisterType() {
  qmlRegisterType<QmlMediaSource>("org.yorba.qt.qmlmediasource", 1, 0,
    "QmlMediaSource");
}

QVariant QmlMediaSource::AsVariant(MediaSource* source) {
  QmlMediaSource* qml = new QmlMediaSource();
  qml->Init(source);
  
  QVariant var;
  var.setValue(qml);
  
  return var;
}

QUrl QmlMediaSource::path() const {
  return QUrl::fromLocalFile(source_->file().absoluteFilePath());
}

QUrl QmlMediaSource::preview_path() const {
  return QUrl::fromLocalFile(source_->preview_file().absoluteFilePath());
}

Orientation QmlMediaSource::orientation() const {
  return source_->orientation();
}

float QmlMediaSource::orientation_rotation() const {
  return OrientationCorrection::FromOrientation(orientation()).rotation_angle_;
}

bool QmlMediaSource::orientation_mirrored() const {
  return OrientationCorrection::FromOrientation(orientation()).horizontal_scale_factor_ < 0.0;
}

bool QmlMediaSource::is_rotated() const {
  return (orientation_rotation() == 90.0) || (orientation_rotation() == -90.0);
}
