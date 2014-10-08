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
#include <QVariantList>

// core
#include <data-collection.h>
#include <view-collection.h>
#include <media-source.h>

class DataObject;
class ContainerSource;
class SelectableViewCollection;
class SourceCollection;

/*!
 * \brief The QmlViewCollectionModel class
 */
class QmlViewCollectionModel : public QAbstractListModel, IDataFilter
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int rawCount READ rawCount NOTIFY rawCountChanged)
    Q_PROPERTY(int selectedCount READ selectedCount NOTIFY selectedCountChanged)
    // not for use in QML directly, but QML can use it, to pass it as QVariant back to C++
    Q_PROPERTY(QList<MediaSource*> selectedMedias READ selectedMedias NOTIFY selectionChanged)
    Q_PROPERTY(QVariantList selectedMediasQML READ selectedMediasQML NOTIFY selectionChanged)
    Q_PROPERTY(QVariant forCollection READ forCollection WRITE setForCollection
               NOTIFY backingCollectionChanged)
    Q_PROPERTY(QVariant monitorSelection READ monitorSelection
               WRITE setMonitorSelection NOTIFY monitorSelectionChanged)
    Q_PROPERTY(int head READ head WRITE setHead NOTIFY headChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(MediaSource::MediaType mediaTypeFilter READ mediaTypeFilter WRITE setMediaTypeFilter
               NOTIFY mediaTypeFilterChanged)
signals:
    void countChanged();
    void rawCountChanged();
    void selectedCountChanged();
    void selectionChanged();
    void backingCollectionChanged();
    void headChanged();
    void limitChanged();
    void orderingChanged();
    void monitorSelectionChanged();
    void mediaTypeFilterChanged();

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
                           DataObjectComparator defaultComparator);
    virtual ~QmlViewCollectionModel();

    QVariant forCollection() const;
    void setForCollection(const QVariant &var);

    QVariant monitorSelection() const;
    void setMonitorSelection(const QVariant &vmodel);

    Q_INVOKABLE int indexOf(const QVariant &var) const;
    Q_INVOKABLE QVariant getAt(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void add(const QVariant &var);
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void unselectAll();
    Q_INVOKABLE void toggleSelection(const QVariant &var);
    Q_INVOKABLE bool isSelected(const QVariant &var) const;

    virtual int rowCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;

    int count() const;
    int rawCount() const;
    int selectedCount() const;
    int head() const;
    void setHead(int head);
    int limit() const;
    void setLimit(int limit);
    void clearLimit();
    MediaSource::MediaType mediaTypeFilter() const;
    void setMediaTypeFilter(MediaSource::MediaType mediaTypeFilter);

    QList<MediaSource*> selectedMedias() const;
    QVariantList selectedMediasQML() const;

    SelectableViewCollection* backingViewCollection() const;

    DataObjectComparator defaultComparator() const;
    void setDefaultComparator(DataObjectComparator comparator);
    bool isAccepted(DataObject* item);

protected:
    virtual void notifyBackingCollectionChanged();

    void monitorSourceCollection(SourceCollection* sources);
    void monitorContainerSource(ContainerSource* container);
    void monitorCollection(const DataCollection* collection, QString viewName);
    bool isMonitoring() const;
    void stopMonitoring();

    // Subclasses should return the DataObject cast and packed in a QVariant
    virtual QVariant toVariant(DataObject* object) const = 0;

    // Subclasses should convert from QVariant into appropriate DataObject
    // subclass.  Return null if unknown type
    virtual DataObject* fromVariant(QVariant var) const = 0;

    void notifyElementAdded(int index);
    void notifyElementRemoved(int index);
    void notifyElementChanged(int index, int role);
    void notifyReset();

    virtual QHash<int, QByteArray> roleNames() const;

private slots:
    void onSelectionChanged(const QSet<DataObject*>* selected,
                            const QSet<DataObject*>* unselected);
    void onContentsAboutToBeChanged(const QSet<DataObject*>* added,
                                    const QSet<DataObject*>* removed);
    void onContentsChanged(const QSet<DataObject*>* add,
                             const QSet<DataObject*>* removed,
                             bool notify);
    void onOrderingChanged();

private:
    QVariant m_collection;
    QVariant m_monitorSelection;
    SelectableViewCollection* m_view;
    QList<int> m_toBeRemoved;
    DataObjectComparator m_defaultComparator;
    int m_head;
    int m_limit;
    QHash<int, QByteArray> m_roles;
    MediaSource::MediaType m_mediaTypeFilter;

    static bool intLessThan(int a, int b);
    static bool intReverseLessThan(int a, int b);

    void setBackingViewCollection(SelectableViewCollection* view);
    void disconnectBackingViewCollection();
    void notifySetChanged(const QSet<DataObject*> *list, int role);
};

#endif  // GALLERY_QML_VIEW_COLLECTION_MODEL_H_
