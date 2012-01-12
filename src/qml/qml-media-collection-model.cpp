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

#include "qml/qml-media-collection-model.h"

#include <QHash>
#include <QtDeclarative>

#include "media/media-source.h"

QmlMediaCollectionModel::QmlMediaCollectionModel(QObject* parent)
  : QmlViewCollectionModel(parent) {
}

void QmlMediaCollectionModel::RegisterType() {
  qmlRegisterType<QmlMediaCollectionModel>("org.yorba.qt.qmlmediacollectionmodel",
    1, 0, "QmlMediaCollectionModel");
}

void QmlMediaCollectionModel::Init(SelectableViewCollection* view) {
  QHash<int, QByteArray> roles;
  roles[ObjectNumberRole] = "object_number";
  roles[SelectionRole] = "is_selected";
  roles[MediaSourceRole] = "media_source";
  
  QmlViewCollectionModel::Init(view, roles);
}

QVariant QmlMediaCollectionModel::DataForRole(DataObject *object, int role) const {
  MediaSource* media_source = qobject_cast<MediaSource*>(object);
  if (media_source == NULL)
    return QVariant();
  
  switch (role) {
    case MediaSourceRole:
      return QVariant::fromValue(media_source);
  
    default:
      return QVariant();
  }
}
