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

#ifndef GALLERY_QML_MEDIA_COLLECTION_MODEL_H_
#define GALLERY_QML_MEDIA_COLLECTION_MODEL_H_

#include <QObject>
#include <QVariant>
#include <QtDeclarative>

#include "core/data-object.h"
#include "core/selectable-view-collection.h"
#include "qml/qml-view-collection-model.h"

class QmlMediaCollectionModel : public QmlViewCollectionModel {
  Q_OBJECT
  
public:
  enum Role {
    MediaSourceRole = QmlViewCollectionModel::LastCommonRole,
  };
  
  QmlMediaCollectionModel(QObject* parent = NULL);
  
  static void RegisterType();
  
  // Init() required because QmlMediaModel is a QML Declarative Type which
  // has restrictions on its ctor signature
  void Init(SelectableViewCollection* view);
  
protected:
  virtual QVariant DataForRole(DataObject* object, int role) const;
};

QML_DECLARE_TYPE(QmlMediaCollectionModel);

#endif  // GALLERY_QML_MEDIA_COLLECTION_MODEL_H_
