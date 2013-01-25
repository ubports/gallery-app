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
#include <QDateTime>
#include <QSet>
#include <QVariant>
#include <QtQml>

#include "qml/qml-media-collection-model.h"

class DataObject;

/*!
 * \brief The QmlEventOverviewModel class
 */
class QmlEventOverviewModel : public QmlMediaCollectionModel
{
  Q_OBJECT
  Q_PROPERTY(bool ascending READ ascending_order WRITE set_ascending_order
    NOTIFY ordering_altered)
  
 signals:
  void ordering_altered();
  
 public:
  QmlEventOverviewModel(QObject* parent = NULL);
  
  static void RegisterType();
  
  bool ascending_order() const;
  void set_ascending_order(bool ascending);
  
 protected:
  virtual void notify_backing_collection_changed();
  
  virtual QVariant VariantFor(DataObject *object) const;
  virtual DataObject* FromVariant(QVariant var) const;
  
 private slots:
  void on_events_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  void on_event_overview_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  void on_event_overview_selection_altered(const QSet<DataObject*>* selected,
    const QSet<DataObject*>* unselected);
  
 private:
  bool ascending_order_;
  bool syncing_media_;
  
  static bool AscendingComparator(DataObject* a, DataObject* b);
  static bool DescendingComparator(DataObject* a, DataObject* b);
  static bool EventComparator(DataObject* a, DataObject* b, bool desc);
  static QDateTime ObjectDateTime(DataObject* object, bool desc);
  
  void MonitorNewViewCollection();
  void SyncSelectedMedia(const QSet<DataObject*>* toggled, bool selected);
};

QML_DECLARE_TYPE(QmlEventOverviewModel)

#endif  // GALLERY_QML_EVENT_OVERVIEW_MODEL_H_
