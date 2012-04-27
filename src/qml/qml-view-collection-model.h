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
  Q_PROPERTY(int selectedCount READ selected_count NOTIFY selectedCountChanged)
  Q_PROPERTY(QVariant forCollection READ for_collection WRITE set_for_collection
    NOTIFY backing_collection_changed)
  Q_PROPERTY(QVariant monitorSelection READ monitor_selection
    WRITE set_monitor_selection NOTIFY monitor_selection_changed)
  Q_PROPERTY(int head READ head WRITE set_head NOTIFY head_changed)
  Q_PROPERTY(int limit READ limit WRITE set_limit NOTIFY limit_changed)
  
 signals:
  void count_changed();
  void selectedCountChanged();
  void backing_collection_changed();
  void head_changed();
  void limit_changed();
  void ordering_altered();
  void monitor_selection_changed();
  
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
  
  QVariant monitor_selection() const;
  void set_monitor_selection(QVariant vmodel);
  
  Q_INVOKABLE int indexOf(QVariant var);
  Q_INVOKABLE QVariant getAt(int index);
  Q_INVOKABLE void clear();
  Q_INVOKABLE void add(QVariant var);
  Q_INVOKABLE void selectAll();
  Q_INVOKABLE void unselectAll();
  Q_INVOKABLE void toggleSelection(QVariant var);
  Q_INVOKABLE bool isSelected(QVariant var);
  
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
  
  DataObjectComparator default_comparator() const;
  void set_default_comparator(DataObjectComparator comparator);
  
 protected:
  virtual void notify_backing_collection_changed();
  
  void MonitorSourceCollection(SourceCollection* sources);
  void MonitorContainerSource(ContainerSource* container);
  bool IsMonitoring() const;
  void StopMonitoring();
  
  // Subclasses should return the DataObject cast and packed in a QVariant
  virtual QVariant VariantFor(DataObject* object) const = 0;
  
  // Subclasses should convert from QVariant into appropriate DataObject
  // subclass.  Return null if unknown type
  virtual DataObject* FromVariant(QVariant var) const = 0;
  
  // This notifies model subscribers that the element at the particular index
  // has been altered in some way.
  void NotifyElementAltered(int index, int role);
  
private slots:
  void on_selection_altered(const QSet<DataObject*>* selected,
    const QSet<DataObject*>* unselected);
  void on_contents_altered(const QSet<DataObject*>* add,
    const QSet<DataObject*>* removed);
  void on_ordering_altered();
  
 private:
  QVariant collection_;
  QVariant monitor_selection_;
  SelectableViewCollection* view_;
  DataObjectComparator default_comparator_;
  int head_;
  int limit_;
  
  static bool IntReverseLessThan(int a, int b);
  
  void SetBackingViewCollection(SelectableViewCollection* view);
  void DisconnectBackingViewCollection();
  void NotifySetAltered(const QSet<DataObject*> *list, int role);
};

#endif  // GALLERY_QML_VIEW_COLLECTION_MODEL_H_
