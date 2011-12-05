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

#include "media-source.h"

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
  
  QObject::connect(view_,
    SIGNAL(selection_altered(QSet<DataObject*>*, QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(QSet<DataObject*>*, QSet<DataObject*>*)));
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

  MediaSource* media_source = qobject_cast<MediaSource*>(view_->GetAt(index.row()));
  if (media_source == NULL)
    return QVariant();

  switch (role) {
    case MediaNumberRole:
      return QVariant(media_source->number());
    
    case PreviewPathRole:
      return QVariant(media_source->preview_file().absoluteFilePath());
    
    case PathRole:
      return QVariant(media_source->file().absoluteFilePath());
    
    case SelectionRole:
      return QVariant(view_->IsSelected(media_source));
    
    default:
      return QVariant();
  }
}

SelectableViewCollection* QmlMediaModel::BackingViewCollection() const {
  Q_ASSERT(IsInited());
  
  return view_;
}

void QmlMediaModel::on_selection_altered(QSet<DataObject*>* selected,
  QSet<DataObject*>* unselected) {
  if (selected != NULL)
    ReportDataChanged(selected);
  
  if (unselected != NULL)
    ReportDataChanged(unselected);
}

void QmlMediaModel::ReportDataChanged(QSet<DataObject*>* list) {
  DataObject* object;
  foreach (object, *list) {
    int index = view_->IndexOf(*object);
    if (index >= 0) {
      QModelIndex model_index = createIndex(index, SelectionRole);
      emit dataChanged(model_index, model_index);
    }
  }
}
