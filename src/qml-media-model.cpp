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

#include "qml-media-model.h"

#include <QObject>
#include <QAbstractListModel>
#include <QtDeclarative>
#include <QModelIndex>
#include <QVariant>
#include <QHash>

QmlMediaModel::QmlMediaModel(QObject* parent = NULL)
  : QAbstractListModel(parent) {
  QHash<int, QByteArray> roles;
  roles[MediaNumberRole] = "media_number";
  roles[PreviewPathRole] = "preview_path";
  roles[PathRole] = "path";
  roles[SelectionRole] = "is_selected";
  setRoleNames(roles);
}

void QmlMediaModel::RegisterType() {
  qmlRegisterType<QmlMediaModel>("org.yorba.qt.qmlmediamodel",
    1, 0, "QmlMediaModel");
}

void QmlMediaModel::Init(SelectableViewCollection* view) {
  Q_ASSERT(!IsInited());
  
  view_ = view;
  
  QObject::connect(view_, SIGNAL(selection_altered(QList<DataObject*>*, QList<DataObject*>*)), this,
    SLOT(on_selection_altered(QList<DataObject*>*, QList<DataObject*>*)));
}

bool QmlMediaModel::IsInited() const {
  return (view_ != NULL);
}

int QmlMediaModel::rowCount(const QModelIndex& parent) const {
  Q_ASSERT(IsInited());
  
  return (view_ != NULL) ? view_->Count() : 0;
}

QVariant QmlMediaModel::data(const QModelIndex& index, int role) const {
  Q_ASSERT(IsInited());
  
  if (index.row() >= view_->Count())
    return QVariant();

  MediaObject* media_object = qobject_cast<MediaObject*>(view_->GetAt(index.row()));
  if (media_object == NULL)
    return QVariant();

  switch (role) {
    case MediaNumberRole:
      return QVariant(media_object->number());
    
    case PreviewPathRole:
      return QVariant(media_object->preview_file().absoluteFilePath());
    
    case PathRole:
      return QVariant(media_object->file().absoluteFilePath());
    
    case SelectionRole:
      return QVariant(view_->IsSelected(media_object));
    
    default:
      return QVariant();
  }
}

SelectableViewCollection* QmlMediaModel::BackingViewCollection() const {
  Q_ASSERT(IsInited());
  
  return view_;
}

void QmlMediaModel::on_selection_altered(QList<DataObject*>* selected,
  QList<DataObject*>* unselected) {
  if (selected != NULL)
    ReportDataChanged(selected);
  
  if (unselected != NULL)
    ReportDataChanged(unselected);
}

void QmlMediaModel::ReportDataChanged(QList<DataObject*>* list) {
  for (int ctr = 0; ctr < list->count(); ctr++) {
    int index = view_->IndexOf(*list->at(ctr));
    if (index >= 0) {
      QModelIndex model_index = createIndex(index, SelectionRole);
      emit dataChanged(model_index, model_index);
    }
  }
}
