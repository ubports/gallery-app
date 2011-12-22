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

#include "qml-view-collection-model.h"

QmlViewCollectionModel::QmlViewCollectionModel(QObject* parent)
  : QAbstractListModel(parent) {
}

void QmlViewCollectionModel::Init(SelectableViewCollection* view,
  const QHash<int, QByteArray>& roles) {
  Q_ASSERT(!IsInited());
  
  view_ = view;
  
  setRoleNames(roles);
  
  QObject::connect(view_,
    SIGNAL(selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::connect(static_cast<DataCollection*>(view_.data()),
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
}

bool QmlViewCollectionModel::IsInited() const {
  return (view_ != NULL);
}

int QmlViewCollectionModel::rowCount(const QModelIndex& parent) const {
  Q_ASSERT(IsInited());
  
  return view_->Count();
}

QVariant QmlViewCollectionModel::data(const QModelIndex& index, int role) const {
  Q_ASSERT(IsInited());
  
  DataObject* object = view_->GetAt(index.row());
  if (object == NULL)
    return QVariant();
  
  switch (role) {
    case ObjectNumberRole:
      return QVariant(object->number());
  
    case SelectionRole:
      return QVariant(view_->IsSelected(object));
    
    default:
      return DataForRole(object, role);
  }
}

SelectableViewCollection* QmlViewCollectionModel::BackingViewCollection() const {
  Q_ASSERT(IsInited());
  
  return view_;
}

QVariant QmlViewCollectionModel::FilenameVariant(const QFileInfo& file_info) {
  return QVariant("file:" + file_info.absoluteFilePath());
}

void QmlViewCollectionModel::NotifyElementAdded(int index) {
  if (index >= 0) {
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
  }
}

void QmlViewCollectionModel::NotifyElementAltered(int index, int role) {
  if (index >= 0) {
    QModelIndex model_index = createIndex(index, role);
    emit dataChanged(model_index, model_index);
  }
}

void QmlViewCollectionModel::NotifySetAltered(const QSet<DataObject*>* list, int role) {
  DataObject* object;
  foreach (object, *list)
    NotifyElementAltered(view_->IndexOf(object), role);
}

void QmlViewCollectionModel::on_selection_altered(const QSet<DataObject*>* selected,
  const QSet<DataObject*>* unselected) {
  if (selected != NULL)
    NotifySetAltered(selected, SelectionRole);
  
  if (unselected != NULL)
    NotifySetAltered(unselected, SelectionRole);
}

void QmlViewCollectionModel::on_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  // Report inserted items
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added)
      NotifyElementAdded(view_->IndexOf(object));
  }
  
  // TODO: Removed content
}
