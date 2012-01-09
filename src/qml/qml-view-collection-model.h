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
#include <QByteArray>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QPointer>
#include <QVariant>

#include "core/data-object.h"
#include "core/selectable-view-collection.h"

class QmlViewCollectionModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int selectedCount READ selectedCount NOTIFY selectedCountChanged)
  
 signals:
  void selectedCountChanged();
  
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
    ObjectNumberRole = Qt::UserRole + 1,
    SelectionRole,
    LastCommonRole
  };
  
  explicit QmlViewCollectionModel(QObject* parent);
  
  // Init() required because subclasses are a QML Declarative Type which
  // has restrictions on its ctor signature
  void Init(SelectableViewCollection* view, const QHash<int, QByteArray>& roles);
  bool IsInited() const;
  
  virtual int rowCount(const QModelIndex& parent) const;
  virtual QVariant data(const QModelIndex& index, int role) const;
  
  int selectedCount() const;
  
  SelectableViewCollection* BackingViewCollection() const;
  
 protected:
  // Utility method for creating file paths packed inside a QVariant (don't
  // prepend URI specifier)
  static QVariant FilenameVariant(const QFileInfo& file_info);
  
  // Subclasses should return data packed in a QVariant for the specified role
  // of the DataObject, otherwise return an empty QVariant if the role number is
  // unknown
  virtual QVariant DataForRole(DataObject* object, int role) const = 0;
  
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
  QPointer<SelectableViewCollection> view_;
  QList<int> to_be_removed_;
  
  static bool IntReverseLessThan(int a, int b);
  
  void NotifySetAltered(const QSet<DataObject*> *list, int role);
};

#endif  // GALLERY_QML_VIEW_COLLECTION_MODEL_H_
