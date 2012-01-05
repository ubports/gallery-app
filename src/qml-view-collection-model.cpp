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
  
  QObject::connect(view_,
    SIGNAL(contents_to_be_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_to_be_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
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

int QmlViewCollectionModel::selectedCount() const {
  return view_->GetSelectedCount();
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

void QmlViewCollectionModel::NotifyElementRemoved(int index) {
  if (index >= 0) {
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
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
  
  emit selectedCountChanged();
}

void QmlViewCollectionModel::on_contents_to_be_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  // Gather the indexes of all the elements to be removed before removal,
  // then report their removal when they've actually been removed
  if (removed != NULL) {
    // should already be cleared; either first use or cleared in on_contents_altered()
    Q_ASSERT(to_be_removed_.count() == 0);
    DataObject* object;
    foreach (object, *removed) {
      int index = view_->IndexOf(object);
      Q_ASSERT(index >= 0);
      to_be_removed_.append(index);
    }
  }
}

void QmlViewCollectionModel::on_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  // TODO: Could be more efficient in both loop by spotting runs and reporting
  // the spans rather than one element at a time
  
  // Report inserted items after they've been inserted
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added)
      NotifyElementAdded(view_->IndexOf(object));
  }
  
  // Report removed items using indices gathered from on_contents_to_be_altered()
  if (to_be_removed_.count() > 0) {
    // sort indices in reverse order and walk in descending order so they're
    // always accurate as items are "removed"
    qSort(to_be_removed_.begin(), to_be_removed_.end(), IntReverseLessThan);
    
    int index;
    foreach (index, to_be_removed_)
      NotifyElementRemoved(index);
    
    to_be_removed_.clear();
  }
}

bool QmlViewCollectionModel::IntReverseLessThan(int a, int b) {
  return b < a;
}
