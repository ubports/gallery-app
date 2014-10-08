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

// core
#include "container-source.h"
#include "data-object.h"
#include "selectable-view-collection.h"

// util
#include "variants.h"

/*!
 * \brief QmlViewCollectionModel::QmlViewCollectionModel
 * \param parent
 * \param objectTypeName
 * \param defaultComparator
 */
QmlViewCollectionModel::QmlViewCollectionModel(QObject* parent, const QString& objectTypeName,
                                               DataObjectComparator defaultComparator)
    : QAbstractListModel(parent), m_view(NULL), m_defaultComparator(defaultComparator),
      m_head(0), m_limit(-1), m_mediaTypeFilter(MediaSource::None)
{
    m_roles.insert(ObjectRole, "object");
    m_roles.insert(SelectionRole, "isSelected");
    m_roles.insert(TypeNameRole, "typeName");

    // allow for subclasses to give object a semantically-significant name
    if (!objectTypeName.isEmpty())
        m_roles.insert(SubclassRole, objectTypeName.toLatin1());
}

/*!
 * \brief QmlViewCollectionModel::~QmlViewCollectionModel
 */
QmlViewCollectionModel::~QmlViewCollectionModel()
{
    delete m_view;
}

/*!
 * \brief QmlViewCollectionModel::forCollection
 * \return
 */
QVariant QmlViewCollectionModel::forCollection() const
{
    return m_collection;
}

/*!
 * \brief QmlViewCollectionModel::setForCollection
 * \param var
 */
void QmlViewCollectionModel::setForCollection(const QVariant &var)
{
    QObject* obj = qvariant_cast<QObject*>(var);
    if (obj == NULL) {
        // Print a warning if we got an unexpected type.  If the type is QObject*,
        // we assume it's uninitialized and nothing to worry about.
        if (var.isValid() && ((QMetaType::Type) var.type()) != QMetaType::QObjectStar)
            qDebug("Unable to set collection of type %s", var.typeName());

        stopMonitoring();

        return;
    }

    ContainerSource* container = qobject_cast<ContainerSource*>(obj);
    if (container != NULL) {
        m_collection = var;
        monitorContainerSource(container);

        return;
    }

    SourceCollection* source = qobject_cast<SourceCollection*>(obj);
    if (source != NULL) {
        m_collection = var;
        monitorSourceCollection(source);

        return;
    }

    stopMonitoring();

    qDebug("Unable to set collection that is neither ContainerSource nor SourceCollection");
}

/*!
 * \brief QmlViewCollectionModel::monitorSelection
 * \return
 */
QVariant QmlViewCollectionModel::monitorSelection() const
{
    return m_monitorSelection;
}

/*!
 * \brief QmlViewCollectionModel::setMonitorSelection
 * \param vmodel
 */
void QmlViewCollectionModel::setMonitorSelection(const QVariant &vmodel)
{
    // always stop monitoring
    m_monitorSelection = QVariant();
    if (m_view != NULL)
        m_view->stopMonitoringSelectionState();
    
    QmlViewCollectionModel* model = UncheckedVariantToObject<QmlViewCollectionModel*>(vmodel);
    if (model == NULL) {
        if (vmodel.isValid())
            qDebug("Unable to monitor selection of type %s", vmodel.typeName());

        return;
    }

    m_monitorSelection = vmodel;
    if (m_view != NULL)
        m_view->monitorSelectionState(model->m_view);
}

/*!
 * \brief QmlViewCollectionModel::indexOf
 * \param var
 * \return
 */
int QmlViewCollectionModel::indexOf(const QVariant &var) const
{
    DataObject* object = UncheckedVariantToObject<DataObject*>(var);
    if (object == NULL)
        return -1;

    return (m_view != NULL) ? m_view->indexOf(object) : -1;
}

/*!
 * \brief QmlViewCollectionModel::getAt
 * \param index
 * \return
 */
QVariant QmlViewCollectionModel::getAt(int index)
{
    if (m_view == NULL || index < 0)
        return QVariant();

    DataObject* object = m_view->getAt(index);

    return (object != NULL) ? toVariant(object) : QVariant();
}

/*!
 * \brief QmlViewCollectionModel::clear
 */
void QmlViewCollectionModel::clear()
{
    stopMonitoring();
}

/*!
 * \brief QmlViewCollectionModel::add
 * \param var
 */
void QmlViewCollectionModel::add(const QVariant &var)
{
    DataObject* object = fromVariant(var);
    if (object == NULL) {
        qDebug("Unable to convert variant into appropriate DataObject");

        return;
    }

    if (m_view == NULL) {
        SelectableViewCollection* view = new SelectableViewCollection("Manual ViewCollection");
        if (m_defaultComparator != NULL)
            view->setComparator(m_defaultComparator);

        setBackingViewCollection(view);
    }

    m_view->add(object);
}

/*!
 * \brief QmlViewCollectionModel::selectAll
 */
void QmlViewCollectionModel::selectAll()
{
    if (m_view != NULL)
        m_view->selectAll();
}

/*!
 * \brief QmlViewCollectionModel::unselectAll
 */
void QmlViewCollectionModel::unselectAll()
{
    if (m_view != NULL)
        m_view->unselectAll();
}

/*!
 * \brief QmlViewCollectionModel::toggleSelection
 * \param var
 */
void QmlViewCollectionModel::toggleSelection(const QVariant &var)
{
    if (m_view != NULL)
        m_view->toggleSelect(VariantToObject<DataObject*>(var));
}

/*!
 * \brief QmlViewCollectionModel::isSelected
 * \param var
 * \return
 */
bool QmlViewCollectionModel::isSelected(const QVariant &var) const
{
    return (m_view != NULL && var.isValid()
            ? m_view->isSelected(VariantToObject<DataObject*>(var))
            : false);
}

/*!
 * \brief QmlViewCollectionModel::rowCount
 * \param parent
 * \return
 */
int QmlViewCollectionModel::rowCount(const QModelIndex& parent) const
{
    return count();
}

/*!
 * \brief QmlViewCollectionModel::data
 * \param index
 * \param role
 * \return
 */
QVariant QmlViewCollectionModel::data(const QModelIndex& index, int role) const
{
    if (m_view == NULL)
        return QVariant();

    if (m_limit == 0)
        return QVariant();

    // calculate actual starting index from the head (a negative head means to
    // start from n elements from the tail of the list; relying on negative value
    // for addition in latter part of ternary operator her)
    int real_start = (m_head >= 0) ? m_head : m_view->count() + m_head;
    int real_index = index.row() + real_start;

    // bounds checking
    if (real_index < 0 || real_index >= m_view->count())
        return QVariant();

    // watch for indexing beyond upper limit
    if (m_limit > 0 && real_index >= (real_start + m_limit))
        return QVariant();

    DataObject* object = m_view->getAt(real_index);
    if (object == NULL)
        return QVariant();

    switch (role) {
    case ObjectRole:
    case SubclassRole:
        return toVariant(object);

    case SelectionRole:
        return QVariant(m_view->isSelected(object));

    case TypeNameRole:
        // Return type name with the pointer ("*") removed
        return QVariant(QString(toVariant(object).typeName()).remove('*'));

    default:
        return QVariant();
    }
}

/*!
 * \brief QmlViewCollectionModel::count
 * \return
 */
int QmlViewCollectionModel::count() const
{
    int count = (m_view != NULL) ? m_view->count() : 0;

    // account for head (if negative, means head of list starts at tail)
    count -= (m_head > 0) ? m_head : (-m_head);

    // watch for underflow
    if (count < 0)
        count = 0;

    // apply limit, if set
    if (m_limit >= 0 && count > m_limit)
        count = m_limit;

    return count;
}

/*!
 * \brief QmlViewCollectionModel::rawCount
 * \return
 */
int QmlViewCollectionModel::rawCount() const
{
    return (m_view != NULL) ? m_view->count() : 0;
}

/*!
 * \brief QmlViewCollectionModel::selectedCount
 * \return
 */
int QmlViewCollectionModel::selectedCount() const
{
    // TODO: Selected count should honor limit and head as well
    return (m_view != NULL) ? m_view->selectedCount() : 0;
}

/*!
 * \brief QmlViewCollectionModel::head
 * \return
 */
int QmlViewCollectionModel::head() const
{
    return m_head;
}

/*!
 * \brief QmlViewCollectionModel::setHead
 * \param head
 */
void QmlViewCollectionModel::setHead(int head) {
    if (m_head == head)
        return;

    m_head = head;

    emit headChanged();
    // TODO: this could be checked for and optimized instead of nuking it all.
    emit countChanged();
    notifyReset();
}

/*!
 * \brief QmlViewCollectionModel::limit
 * \return
 */
int QmlViewCollectionModel::limit() const
{
    return m_limit;
}

/*!
 * \brief QmlViewCollectionModel::setLimit
 * \param limit
 */
void QmlViewCollectionModel::setLimit(int limit)
{
    int normalized = (limit >= 0) ? limit : -1;

    if (m_limit == normalized)
        return;

    m_limit = normalized;

    emit limitChanged();
    emit countChanged();
    notifyReset();
}

/*!
 * \brief QmlViewCollectionModel::clearLimit
 */
void QmlViewCollectionModel::clearLimit()
{
    setLimit(-1);
}

/*!
 * \brief QmlViewCollectionModel::selectedMedias returns the selected media itmes
 * \return
 */
QList<MediaSource*> QmlViewCollectionModel::selectedMedias() const
{
    QList<MediaSource*> selectedList;
    QSet<DataObject*> totalSelection = m_view->getSelected();
    foreach (DataObject* data, totalSelection) {
        MediaSource *media = qobject_cast<MediaSource*>(data);
        if (media)
            selectedList.append(media);
    }
    return selectedList;
}

QVariantList QmlViewCollectionModel::selectedMediasQML() const
{
    QVariantList selectedList;
    QSet<DataObject*> totalSelection = m_view->getSelected();
    foreach (DataObject* data, totalSelection) {
        QVariant var;
        var.setValue(data);
        selectedList << var;
    }
    return selectedList;
}

/*!
 * \brief QmlViewCollectionModel::setBackingViewCollection
 * \param view
 */
void QmlViewCollectionModel::setBackingViewCollection(SelectableViewCollection* view)
{
    int old_count = (m_view != NULL) ? m_view->count() : 0;
    int old_selected_count = (m_view != NULL) ? m_view->selectedCount() : 0;

    disconnectBackingViewCollection();

    beginResetModel();

    m_view = view;

    endResetModel();

    QObject::connect(m_view,
                     SIGNAL(selectionChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)),
                     this,
                     SLOT(onSelectionChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)));

    QObject::connect(m_view,
                     SIGNAL(contentsAboutToBeChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)),
                     this,
                     SLOT(onContentsAboutToBeChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)));

    QObject::connect(m_view,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)),
                     this,
                     SLOT(onContentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)));

    QObject::connect(m_view, SIGNAL(orderingChanged()),
                     this, SLOT(onOrderingChanged()));

    notifyBackingCollectionChanged();

    if (old_count != m_view->count()) {
        emit rawCountChanged();
        // TODO: we could maybe check for this instead of always firing it.
        emit countChanged();
    }

    emit selectionChanged();
    if (old_selected_count != m_view->selectedCount())
        emit selectedCountChanged();
}

/*!
 * \brief QmlViewCollectionModel::disconnectBackingViewCollection
 */
void QmlViewCollectionModel::disconnectBackingViewCollection()
{
    if (m_view == NULL)
        return;

    QObject::disconnect(m_view,
                        SIGNAL(selectionChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)),
                        this,
                        SLOT(onSelectionChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)));

    QObject::disconnect(m_view,
                        SIGNAL(contentsAboutToBeChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)),
                        this,
                        SLOT(onContentsAboutToBeChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)));

    QObject::disconnect(m_view,
                        SIGNAL(contentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)),
                        this,
                        SLOT(onContentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)));

    QObject::disconnect(m_view, SIGNAL(orderingChanged()),
                        this, SLOT(onOrderingChanged()));

    beginResetModel();

    delete m_view;
    m_view = NULL;

    endResetModel();
}

/*!
 * \brief QmlViewCollectionModel::backingViewCollection
 * \return
 */
SelectableViewCollection* QmlViewCollectionModel::backingViewCollection() const
{
    return m_view;
}

/*!
 * \brief QmlViewCollectionModel::defaultComparator
 * \return
 */
DataObjectComparator QmlViewCollectionModel::defaultComparator() const
{
    return m_defaultComparator;
}

/*!
 * \brief QmlViewCollectionModel::setDefaultComparator
 * \param comparator
 */
void QmlViewCollectionModel::setDefaultComparator(DataObjectComparator comparator)
{
    m_defaultComparator = comparator;
}

/*!
 * \brief QmlViewCollectionModel::isAccepted
 * \param item
 */
bool QmlViewCollectionModel::isAccepted(DataObject* item)
{
    Q_UNUSED(item);
    return true;
}

/*!
 * \brief QmlViewCollectionModel::monitorSourceCollection
 * \param sources
 */
void QmlViewCollectionModel::monitorSourceCollection(SourceCollection* sources)
{
    monitorCollection(sources, "MonitorSourceCollection");
}

/*!
 * \brief QmlViewCollectionModel::monitorContainerSource
 * \param container
 */
void QmlViewCollectionModel::monitorContainerSource(ContainerSource* container)
{
    monitorCollection(container->contained(), "MonitorContainerSource");
}

/*!
 * \brief QmlViewCollectionModel::monitorCollection
 * \param container
 */
void QmlViewCollectionModel::monitorCollection(const DataCollection* collection, QString viewName)
{
    SelectableViewCollection* view = new SelectableViewCollection(viewName);
    if (m_defaultComparator != NULL)
        view->setComparator(m_defaultComparator);
    view->monitorDataCollection(collection, this, (m_defaultComparator == NULL));

    setBackingViewCollection(view);
}

MediaSource::MediaType QmlViewCollectionModel::mediaTypeFilter() const
{
    return m_mediaTypeFilter;
}

void QmlViewCollectionModel::setMediaTypeFilter(MediaSource::MediaType mediaTypeFilter)
{
    if (m_mediaTypeFilter != mediaTypeFilter) {
        const DataCollection* collection = 0;
        QString viewName;

        if (m_view) {
            collection = m_view->collection();
            viewName = m_view->toString();
            disconnectBackingViewCollection();
        }

        m_mediaTypeFilter = mediaTypeFilter;
        if (collection) {
            monitorCollection(collection, viewName);
        }

        Q_EMIT mediaTypeFilterChanged();
    }
}

/*!
 * \brief QmlViewCollectionModel::isMonitoring
 * \return
 */
bool QmlViewCollectionModel::isMonitoring() const
{
    return m_view != NULL && m_view->isMonitoring();
}

/*!
 * \brief QmlViewCollectionModel::stopMonitoring
 */
void QmlViewCollectionModel::stopMonitoring()
{
    if (m_view == NULL)
        return;

    int old_count = m_view->count();
    int old_selected_count = m_view->selectedCount();

    disconnectBackingViewCollection();

    notifyBackingCollectionChanged();

    if (old_count != 0) {
        emit rawCountChanged();
        emit countChanged();
    }

    if (old_selected_count != 0)
        emit selectedCountChanged();
}

/*!
 * \brief QmlViewCollectionModel::notifyBackingCollectionChanged
 */
void QmlViewCollectionModel::notifyBackingCollectionChanged()
{
    emit backingCollectionChanged();
}

/*!
 * \brief QmlViewCollectionModel::notifyElementAdded
 * This notifies model subscribers that the element has been added at the
 * particular index ... note that QmlViewCollectionModel monitors
 * the SelectableViewCollections "contents-altered" signal already
 * \param index
 */
void QmlViewCollectionModel::notifyElementAdded(int index)
{
    if (index >= 0) {
        beginInsertRows(QModelIndex(), index, index);
        endInsertRows();
    }
}

/*!
 * \brief QmlViewCollectionModel::notifyElementRemoved
 * This notifies model subscribers that the element at this index was
 * removed ... note that QmlViewCollectionModel monitors the
 * SelectableViewCollections' "contents-altered" signal already
 * \param index
 */
void QmlViewCollectionModel::notifyElementRemoved(int index)
{
    if (index >= 0) {
        beginRemoveRows(QModelIndex(), index, index);
        endRemoveRows();
    }
}

/*!
 * \brief QmlViewCollectionModel::notifyElementChanged
 * This notifies model subscribers that the element at the particular index
 * has been altered in some way.
 * \param index
 * \param role
 */
void QmlViewCollectionModel::notifyElementChanged(int index, int role)
{
    if (index >= 0) {
        QModelIndex model_index = createIndex(index, role);
        emit dataChanged(model_index, model_index);
    }
}

/*!
 * \brief QmlViewCollectionModel::notifySetChanged
 * \param list
 * \param role
 */
void QmlViewCollectionModel::notifySetChanged(const QSet<DataObject*>* list, int role)
{
    DataObject* object;
    foreach (object, *list)
        notifyElementChanged(m_view->indexOf(object), role);
}

/*!
 * \brief QmlViewCollectionModel::notifyReset Tells model subscribers that everything has changed.
 */
void QmlViewCollectionModel::notifyReset()
{
    beginResetModel();
    endResetModel();
}

/*!
 * \brief QmlViewCollectionModel::roleNames
 * \return
 */
QHash<int, QByteArray> QmlViewCollectionModel::roleNames() const
{
    return m_roles;
}

/*!
 * \brief QmlViewCollectionModel::onSelectionChanged
 * \param selected
 * \param unselected
 */
void QmlViewCollectionModel::onSelectionChanged(const QSet<DataObject*>* selected,
                                                  const QSet<DataObject*>* unselected)
{
    if (selected != NULL)
        notifySetChanged(selected, SelectionRole);

    if (unselected != NULL)
        notifySetChanged(unselected, SelectionRole);

    emit selectionChanged();
    emit selectedCountChanged();
}

/*!
 * \brief QmlViewCollectionModel::onContentsAboutToBeChanged
 * \param added
 * \param removed
 */
void QmlViewCollectionModel::onContentsAboutToBeChanged(const QSet<DataObject*>* added,
                                                       const QSet<DataObject*>* removed)
{
    // Gather the indexes of all the elements to be removed before removal,
    // then report their removal when they've actually been removed
    if (removed != NULL) {
        // should already be cleared; either first use or cleared in on_contents_altered()
        Q_ASSERT(m_toBeRemoved.count() == 0);
        DataObject* object;
        foreach (object, *removed) {
            int index = m_view->indexOf(object);
            Q_ASSERT(index >= 0);
            m_toBeRemoved.append(index);
        }
    }
}

/*!
 * \brief QmlViewCollectionModel::onContentsChanged
 * \param added
 * \param removed
 */
void QmlViewCollectionModel::onContentsChanged(const QSet<DataObject*>* added,
                                                 const QSet<DataObject*>* removed,
                                                 bool notify)
{
    // Report removed items using indices gathered from on_contents_to_be_altered()
    if (m_toBeRemoved.count() > 0) {
        if (notify) {
            // sort indices in reverse order and walk in descending order so they're
            // always accurate as items are "removed"
            qSort(m_toBeRemoved.begin(), m_toBeRemoved.end(), intReverseLessThan);

            // Only if we aren't getting a sub-view do we directly map these up to QML.
            if (m_head == 0 && m_limit < 0) {
                int index;
                foreach (index, m_toBeRemoved)
                    notifyElementRemoved(index);
            }

            m_toBeRemoved.clear();
        } else {
            //FIXME We are doing a notifyReset since we are facing model corruption after
            // some deletes on the Events tab
            m_toBeRemoved.clear();
            notifyReset();
        }
    }

    // Report inserted items after they've been inserted
    if (added != NULL) {
        // sort the indices in ascending order so each element is added in the
        // right place inside the "virtual" list held by QAbstractListModel
        QList<int> indices;

        DataObject* object;
        foreach (object, *added)
            indices.append(m_view->indexOf(object));

        qSort(indices.begin(), indices.end(), intLessThan);

        // Again, don't map directly to QML if we're only getting a sub-view.
        if (m_head == 0 && m_limit < 0) {
            int index;
            foreach (index, indices)
                notifyElementAdded(index);
        }
    }

    // TODO: "filtered" views get some special treatment.  Instead of figuring
    // out how each addition/deletion affects it, we just wipe the whole thing
    // out each time it's altered.  This is probably wasteful.
    if (m_head != 0 || m_limit >= 0)
        notifyReset();

    emit rawCountChanged();
    emit countChanged();
}

/*!
 * \brief QmlViewCollectionModel::onOrderingChanged
 */
void QmlViewCollectionModel::onOrderingChanged()
{
    notifyReset();

    emit orderingChanged();
}

/*!
 * \brief QmlViewCollectionModel::intLessThan
 * \param a
 * \param b
 * \return
 */
bool QmlViewCollectionModel::intLessThan(int a, int b)
{
    return a < b;
}

/*!
 * \brief QmlViewCollectionModel::intReverseLessThan
 * \param a
 * \param b
 * \return
 */
bool QmlViewCollectionModel::intReverseLessThan(int a, int b)
{
    return b < a;
}
