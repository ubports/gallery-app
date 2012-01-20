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

#include "qml/qml-view-collection-model.h"

#include "util/variants.h"

QmlViewCollectionModel::QmlViewCollectionModel(QObject* parent, const QString& objectTypeName)
  : QAbstractListModel(parent), view_(NULL), default_comparator_(NULL) {
  QHash<int, QByteArray> roles;
  roles.insert(ObjectRole, "object");
  roles.insert(SelectionRole, "isSelected");
  roles.insert(TypeNameRole, "typeName");
  
  // allow for subclasses to give object a semantically-significant name
  if (!objectTypeName.isEmpty())
    roles.insert(SubclassRole, objectTypeName.toAscii());
  
  setRoleNames(roles);
}

QmlViewCollectionModel::~QmlViewCollectionModel() {
  delete view_;
}

QVariant QmlViewCollectionModel::for_collection() const {
  return collection_;
}

void QmlViewCollectionModel::set_for_collection(QVariant var) {
  QObject* obj = qvariant_cast<QObject*>(var);
  if (obj == NULL) {
    if (var.isValid())
      qDebug("Unable to set collection of type %s", var.typeName());
    
    return;
  }
  
  ContainerSource* container = qobject_cast<ContainerSource*>(obj);
  if (container != NULL) {
    collection_ = var;
    MonitorContainerSource(container);
    
    return;
  }
  
  SourceCollection* source = qobject_cast<SourceCollection*>(obj);
  if (source != NULL) {
    collection_ = var;
    MonitorSourceCollection(source);
    
    return;
  }
  
  qDebug("Unable to set collection that is neither ContainerSource nor SourceCollection");
}

int QmlViewCollectionModel::indexOf(QVariant var) {
  return (view_ != NULL) ? view_->IndexOf(VariantToObject<DataObject*>(var)) : -1;
}

QVariant QmlViewCollectionModel::getAt(int index) {
  if (view_ == NULL || index < 0)
    return QVariant();
  
  DataObject* object = view_->GetAt(index);
  
  return (object != NULL) ? VariantFor(object) : QVariant();
}

void QmlViewCollectionModel::unselectAll() {
  if (view_ != NULL)
    view_->UnselectAll();
}

void QmlViewCollectionModel::toggleSelection(QVariant var) {
  if (view_ != NULL)
    view_->ToggleSelect(VariantToObject<DataObject*>(var));
}

int QmlViewCollectionModel::rowCount(const QModelIndex& parent) const {
  return (view_ != NULL) ? view_->Count() : 0;
}

QVariant QmlViewCollectionModel::data(const QModelIndex& index, int role) const {
  if (view_ == NULL)
    return QVariant();
  
  DataObject* object = view_->GetAt(index.row());
  if (object == NULL)
    return QVariant();
  
  switch (role) {
    case ObjectRole:
    case SubclassRole:
      return VariantFor(object);
    
    case SelectionRole:
      return QVariant(view_->IsSelected(object));
    
    case TypeNameRole:
      // Return type name with the pointer ("*") removed
      return QVariant(QString(VariantFor(object).typeName()).remove('*'));
    
    default:
      return QVariant();
  }
}

int QmlViewCollectionModel::count() const {
  return (view_ != NULL) ? view_->Count() : 0;
}

int QmlViewCollectionModel::selected_count() const {
  return (view_ != NULL) ? view_->GetSelectedCount() : 0;
}

void QmlViewCollectionModel::SetBackingViewCollection(SelectableViewCollection* view) {
  DisconnectBackingViewCollection();
  
  view_ = view;
  
  QObject::connect(view_,
    SIGNAL(selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::connect(view_,
    SIGNAL(contents_to_be_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_to_be_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::connect(view_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  notify_backing_collection_changed();
  emit count_changed();
  emit selected_count_changed();
}

void QmlViewCollectionModel::DisconnectBackingViewCollection() {
  if (view_ == NULL)
    return;
  
  QObject::disconnect(view_,
    SIGNAL(selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::disconnect(view_,
    SIGNAL(contents_to_be_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_to_be_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::disconnect(view_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  int last = view_->Count();
  
  delete view_;
  view_ = NULL;
  
  beginRemoveRows(QModelIndex(), 0, last);
  endRemoveRows();
}

SelectableViewCollection* QmlViewCollectionModel::BackingViewCollection() const {
  return view_;
}

void QmlViewCollectionModel::SetDefaultComparator(DataObjectComparator comparator) {
  default_comparator_ = comparator;
  if (view_ != NULL)
    view_->SetComparator(default_comparator_);
}

void QmlViewCollectionModel::MonitorSourceCollection(SourceCollection* sources) {
  SelectableViewCollection* view = new SelectableViewCollection("MonitorSourceCollection");
  if (default_comparator_ != NULL)
    view->SetComparator(default_comparator_);
  view->MonitorDataCollection(sources, NULL, false);
  
  SetBackingViewCollection(view);
}

void QmlViewCollectionModel::MonitorContainerSource(ContainerSource* container) {
  SelectableViewCollection* view = new SelectableViewCollection("MonitorContainerSource");
  if (default_comparator_ != NULL)
    view->SetComparator(default_comparator_);
  view->MonitorDataCollection(container->contained(), NULL, false);
  
  SetBackingViewCollection(view);
}

void QmlViewCollectionModel::notify_backing_collection_changed() {
  emit backing_collection_changed();
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
  
  emit selected_count_changed();
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
  
  emit count_changed();
}

bool QmlViewCollectionModel::IntReverseLessThan(int a, int b) {
  return b < a;
}
