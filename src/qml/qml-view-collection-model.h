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


#ifndef GALLERY_QML_VIEW_COLLECTION_MODEL_H_
#define GALLERY_QML_VIEW_COLLECTION_MODEL_H_

#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "core/container-source.h"
#include "core/data-object.h"
#include "core/selectable-view-collection.h"
#include "core/source-collection.h"

class QmlViewCollectionModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int count READ count NOTIFY count_changed)
  Q_PROPERTY(int rawCount READ raw_count NOTIFY count_changed)
  Q_PROPERTY(int selectedCount READ selected_count NOTIFY selected_count_changed)
  Q_PROPERTY(QVariant forCollection READ for_collection WRITE set_for_collection
    NOTIFY backing_collection_changed)
  Q_PROPERTY(int head READ head WRITE set_head NOTIFY head_changed)
  Q_PROPERTY(int limit READ limit WRITE set_limit NOTIFY limit_changed)
  
 signals:
  void count_changed();
  void selected_count_changed();
  void backing_collection_changed();
  void head_changed();
  void limit_changed();
  
 public:
  // These roles are available for all subclasses of QmlViewCollectionModel.
  // However, they must be manually added (and named) in the QHash that's
  // passed to Init(); they will not be added automatically.  However, if they
  // are present in the QHash, QmlViewCollectionModel will automatically
  // handle them; subclasses don't need to account for them in DataForRole().
  //
  // Subclasses should start their numbering with LastCommonRole to avoid
  // conflict.
  enum CommonRole {
    SelectionRole = Qt::UserRole + 1,
    ObjectRole,
    SubclassRole,
    TypeNameRole,
    LastCommonRole
  };
  
  QmlViewCollectionModel(QObject* parent, const QString& objectTypeName,
    DataObjectComparator default_comparator);
  virtual ~QmlViewCollectionModel();
  
  QVariant for_collection() const;
  void set_for_collection(QVariant var);
  
  Q_INVOKABLE int indexOf(QVariant var);
  Q_INVOKABLE QVariant getAt(int index);
  Q_INVOKABLE void unselectAll();
  Q_INVOKABLE void toggleSelection(QVariant var);
  
  virtual int rowCount(const QModelIndex& parent) const;
  virtual QVariant data(const QModelIndex& index, int role) const;
  
  int count() const;
  int raw_count() const;
  int selected_count() const;
  int head() const;
  void set_head(int head);
  int limit() const;
  void set_limit(int limit);
  void clear_limit();
  
  SelectableViewCollection* BackingViewCollection() const;
  
 protected:
  virtual void notify_backing_collection_changed();
  
  void MonitorSourceCollection(SourceCollection* sources);
  void MonitorContainerSource(ContainerSource* container);
  
  // Subclasses should return the DataObject cast and packed in a QVariant
  virtual QVariant VariantFor(DataObject* object) const = 0;
  
  // This notifies model subscribers that the element has been added at the
  // particular index ... note that QmlViewCollectionModel monitors
  // the SelectableViewCollections "contents-altered" signal already
  void NotifyElementAdded(int index);
  
  // This notifies model subscribers that the element at this index was
  // removed ... note that QmlViewCollectionModel monitors the
  // SelectableViewCollections' "contents-altered" signal already
  void NotifyElementRemoved(int index);
  
  // This notifies model subscribers that the element at the particular index
  // has been altered in some way.
  void NotifyElementAltered(int index, int role);
  
private slots:
  void on_selection_altered(const QSet<DataObject*>* selected,
    const QSet<DataObject*>* unselected);
  void on_contents_to_be_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  void on_contents_altered(const QSet<DataObject*>* add,
    const QSet<DataObject*>* removed);
  
 private:
  QVariant collection_;
  SelectableViewCollection* view_;
  QList<int> to_be_removed_;
  DataObjectComparator default_comparator_;
  int head_;
  int limit_;
  
  static bool IntReverseLessThan(int a, int b);
  
  void SetBackingViewCollection(SelectableViewCollection* view);
  void DisconnectBackingViewCollection();
  void NotifySetAltered(const QSet<DataObject*> *list, int role);
};

#endif  // GALLERY_QML_VIEW_COLLECTION_MODEL_H_
