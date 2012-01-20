/*
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef GALLERY_QML_EVENT_OVERVIEW_MODEL_H_
#define GALLERY_QML_EVENT_OVERVIEW_MODEL_H_

#include <QObject>
#include <QDate>
#include <QDateTime>
#include <QSet>
#include <QVariant>
#include <QtDeclarative>

#include "core/data-object.h"
#include "core/data-source.h"
#include "core/source-collection.h"
#include "qml/qml-media-collection-model.h"

class QmlEventOverviewModel : public QmlMediaCollectionModel {
  Q_OBJECT
  
 public:
  QmlEventOverviewModel(QObject* parent = NULL);
  virtual ~QmlEventOverviewModel();
  
  static void RegisterType();
  
  virtual void notify_backing_collection_changed();
  
 protected:
  virtual QVariant VariantFor(DataObject *object) const;
  
 private slots:
  void on_contents_altered(const QSet<DataObject *> *add,
    const QSet<DataObject *> *removed);
  void on_selection_altered(const QSet<DataObject *> *add,
    const QSet<DataObject *> *removed);
  
 private:
  SourceCollection markers_;
  
  static bool Comparator(DataObject* a, DataObject* b);
  static QDateTime ObjectDateTime(DataObject* object);
  
  void MonitorNewViewCollection();
  void SelectUnselectEvent(const QSet<DataObject*>* toggled, bool doSelect);
};

QML_DECLARE_TYPE(QmlEventOverviewModel);

#endif  // GALLERY_QML_EVENT_OVERVIEW_MODEL_H_
