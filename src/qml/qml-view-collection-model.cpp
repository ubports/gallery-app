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

QmlViewCollectionModel::QmlViewCollectionModel(QObject* parent, const QString& objectTypeName,
  DataObjectComparator default_comparator)
  : QAbstractListModel(parent), view_(NULL), default_comparator_(default_comparator),
  head_(0), limit_(-1) {
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
    
    StopMonitoring();
    
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
  
  StopMonitoring();
  
  qDebug("Unable to set collection that is neither ContainerSource nor SourceCollection");
}

QVariant QmlViewCollectionModel::monitor_selection() const {
  return monitor_selection_;
}

void QmlViewCollectionModel::set_monitor_selection(QVariant vmodel) {
  // always stop monitoring
  monitor_selection_ = QVariant();
  if (view_ != NULL)
    view_->StopMonitoringSelectionState();
    
  QmlViewCollectionModel* model = UncheckedVariantToObject<QmlViewCollectionModel*>(vmodel);
  if (model == NULL) {
    if (vmodel.isValid())
      qDebug("Unable to monitor selection of type %s", vmodel.typeName());
      
    return;
  }
  
  monitor_selection_ = vmodel;
  if (view_ != NULL)
    view_->MonitorSelectionState(model->view_);
}

int QmlViewCollectionModel::indexOf(QVariant var) {
  DataObject* object = UncheckedVariantToObject<DataObject*>(var);
  if (object == NULL)
    return -1;
  
  return (view_ != NULL) ? view_->IndexOf(object) : -1;
}

QVariant QmlViewCollectionModel::getAt(int index) {
  if (view_ == NULL || index < 0)
    return QVariant();
  
  DataObject* object = view_->GetAt(index);
  
  return (object != NULL) ? VariantFor(object) : QVariant();
}

void QmlViewCollectionModel::clear() {
  StopMonitoring();
}

void QmlViewCollectionModel::add(QVariant var) {
  DataObject* object = FromVariant(var);
  if (object == NULL) {
    qDebug("Unable to convert variant into appropriate DataObject");
    
    return;
  }
  
  if (view_ == NULL) {
    SelectableViewCollection* view = new SelectableViewCollection("Manual ViewCollection");
    if (default_comparator_ != NULL)
      view->SetComparator(default_comparator_);
    
    SetBackingViewCollection(view);
  }
  
  view_->Add(object);
}

void QmlViewCollectionModel::selectAll() {
  if (view_ != NULL)
    view_->SelectAll();
}

void QmlViewCollectionModel::unselectAll() {
  if (view_ != NULL)
    view_->UnselectAll();
}

void QmlViewCollectionModel::toggleSelection(QVariant var) {
  if (view_ != NULL)
    view_->ToggleSelect(VariantToObject<DataObject*>(var));
}

bool QmlViewCollectionModel::isSelected(QVariant var) {
  return (view_ != NULL
    ? view_->IsSelected(VariantToObject<DataObject*>(var))
    : false);
}

int QmlViewCollectionModel::rowCount(const QModelIndex& parent) const {
  return count();
}

QVariant QmlViewCollectionModel::data(const QModelIndex& index, int role) const {
  if (view_ == NULL)
    return QVariant();
  
  if (limit_ == 0)
    return QVariant();
  
  // calculate actual starting index from the head (a negative head means to
  // start from n elements from the tail of the list; relying on negative value
  // for addition in latter part of ternary operator her)
  int real_start = (head_ >= 0) ? head_ : view_->Count() + head_;
  int real_index = index.row() + real_start;
  
  // bounds checking
  if (real_index < 0 || real_index >= view_->Count())
    return QVariant();
  
  // watch for indexing beyond upper limit
  if (limit_ > 0 && real_index >= (real_start + limit_))
    return QVariant();
  
  DataObject* object = view_->GetAt(real_index);
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
  int count = (view_ != NULL) ? view_->Count() : 0;
  
  // account for head (if negative, means head of list starts at tail)
  count -= (head_ > 0) ? head_ : (-head_);
  
  // watch for underflow
  if (count < 0)
    count = 0;
  
  // apply limit, if set
  if (limit_ >= 0 && count > limit_)
    count = limit_;
  
  return count;
}

int QmlViewCollectionModel::raw_count() const {
  return (view_ != NULL) ? view_->Count() : 0;
}

int QmlViewCollectionModel::selected_count() const {
  // TODO: Selected count should honor limit and head as well
  return (view_ != NULL) ? view_->SelectedCount() : 0;
}

int QmlViewCollectionModel::head() const {
  return head_;
}

void QmlViewCollectionModel::set_head(int head) {
  if (head_ == head)
    return;
  
  head_ = head;
  
  head_changed();
}

int QmlViewCollectionModel::limit() const {
  return limit_;
}

void QmlViewCollectionModel::set_limit(int limit) {
  int normalized = (limit >= 0) ? limit : -1;
  
  if (limit_ == normalized)
    return;
  
  limit_ = normalized;
  
  limit_changed();
}

void QmlViewCollectionModel::clear_limit() {
  set_limit(-1);
}

void QmlViewCollectionModel::SetBackingViewCollection(SelectableViewCollection* view) {
  int old_count = (view_ != NULL) ? view_->Count() : 0;
  int old_selected_count = (view_ != NULL) ? view_->SelectedCount() : 0;
  
  DisconnectBackingViewCollection();
  
  beginResetModel();
  
  view_ = view;
  
  endResetModel();
  
  QObject::connect(view_,
    SIGNAL(selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::connect(view_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::connect(view_,
    SIGNAL(ordering_altered()),
    this,
    SLOT(on_ordering_altered()));
  
  notify_backing_collection_changed();
  
  if (old_count != view_->Count())
    emit count_changed();
  
  if (old_selected_count != view_->SelectedCount())
    emit selectedCountChanged();
}

void QmlViewCollectionModel::DisconnectBackingViewCollection() {
  if (view_ == NULL)
    return;
  
  QObject::disconnect(view_,
    SIGNAL(selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::disconnect(view_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  QObject::disconnect(view_,
    SIGNAL(ordering_altered()),
    this,
    SLOT(on_ordering_altered()));
  
  beginResetModel();
  
  delete view_;
  view_ = NULL;
  
  endResetModel();
}

SelectableViewCollection* QmlViewCollectionModel::BackingViewCollection() const {
  return view_;
}

DataObjectComparator QmlViewCollectionModel::default_comparator() const {
  return default_comparator_;
}

void QmlViewCollectionModel::set_default_comparator(DataObjectComparator comparator) {
  default_comparator_ = comparator;
}

void QmlViewCollectionModel::MonitorSourceCollection(SourceCollection* sources) {
  SelectableViewCollection* view = new SelectableViewCollection("MonitorSourceCollection");
  if (default_comparator_ != NULL)
    view->SetComparator(default_comparator_);
  view->MonitorDataCollection(sources, NULL, (default_comparator_ == NULL));
  
  SetBackingViewCollection(view);
}

void QmlViewCollectionModel::MonitorContainerSource(ContainerSource* container) {
  SelectableViewCollection* view = new SelectableViewCollection("MonitorContainerSource");
  if (default_comparator_ != NULL)
    view->SetComparator(default_comparator_);
  view->MonitorDataCollection(container->contained(), NULL,
    (default_comparator_ == NULL));
  
  SetBackingViewCollection(view);
}

bool QmlViewCollectionModel::IsMonitoring() const {
  return view_ != NULL && view_->IsMonitoring();
}

void QmlViewCollectionModel::StopMonitoring() {
  if (view_ == NULL)
    return;
  
  int old_count = view_->Count();
  int old_selected_count = view_->SelectedCount();
  
  DisconnectBackingViewCollection();
  
  notify_backing_collection_changed();
  
  if (old_count != 0)
    emit count_changed();
  
  if (old_selected_count != 0)
    emit selectedCountChanged();
}

void QmlViewCollectionModel::notify_backing_collection_changed() {
  emit backing_collection_changed();
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

void QmlViewCollectionModel::on_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  // Previous code used begin/endInsertRows, but these were not working as
  // expected when insertions were made on an existing QmlViewCollectionModel,
  // creating unstable sort orders ... rather than fight this now, using the
  // model reset methods to simply clear the decks and start anew for each
  // inseration and removal.  This may be a performance problem later as
  // collections grow, in which case the more specific insert/remove methods
  // should be used.  (Reporting the insertion and removal of spans rather than
  // paired calls for each element may also be more efficient.)
  
  beginResetModel();
  endResetModel();
  
  emit count_changed();
}

void QmlViewCollectionModel::on_ordering_altered() {
  beginResetModel();
  endResetModel();
  
  emit ordering_altered();
}

bool QmlViewCollectionModel::IntReverseLessThan(int a, int b) {
  return b < a;
}
