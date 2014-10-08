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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "album.h"
#include "album-collection.h"
#include "album-default-template.h"

// core
#include "selectable-view-collection.h"

// database
#include "database.h"
#include "album-table.h"

// media
#include "media-collection.h"

// util
#include "variants.h"

const int Album::PAGES_PER_COVER = 1;
// We use the left page's number as current_page_.  -1 because the cover is 0
// and we want it to show up on the right.
const int Album::FIRST_VALID_CURRENT_PAGE = -1;

/*!
 * \brief Album::Album
 * \param parent
 */
Album::Album(QObject *parent)
    : ContainerSource(parent, "New Album", MediaCollection::exposureDateTimeAscendingComparator),
      m_albumTemplate(0),
      m_newAlbum(false),
      m_albumTable(0)
{
    initInstance();
}

/*!
 * \brief Album::Album
 * \param parent
 * \param album_template
 * \param title
 * \param subtitle
 * \param id
 * \param creation_timestamp
 * \param closed
 * \param current_page
 * \param cover_nickname
 */
Album::Album(QObject *parent, const QString& title, const QString& subtitle,
             qint64 id, QDateTime creationTimestamp, bool closed,
             int currentPage, const QString &coverNickname)
    : ContainerSource(parent, title, MediaCollection::exposureDateTimeAscendingComparator),
      m_albumTemplate(0),
      m_title(title),
      m_subtitle(subtitle),
      m_newAlbum(false),
      m_albumTable(0)
{
    initInstance();

    // replace defaults with what was read from DB
    m_coverNickname = coverNickname;
    m_creationDateDime = creationTimestamp;
    m_currentPage = currentPage;
    m_closed = closed;
    m_id = id;
}

/*!
 * \brief Album::~Album
 */
Album::~Album()
{
    delete m_contentPages;
}

/*!
 * \brief Album::initInstance
 */
void Album::initInstance()
{
    m_creationDateDime = QDateTime::currentDateTime();
    m_currentPage = FIRST_VALID_CURRENT_PAGE;
    m_closed = true;
    m_populatedPagesCount = 0;
    m_contentPages = new SourceCollection(QString("Pages for ") + m_title);
    m_refreshingContainer = false;
    m_id = INVALID_ID;
    m_coverNickname = "default";

    createAddPhotosPage();

    QObject::connect(m_contentPages,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)),
                     this,
                     SLOT(onAlbumPageContentChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)));
}

/*!
 * \brief Album::mediaList2ObjectSet converts a list of MediaSource into a set
 * of DataObjects
 * \param mediaList
 * \return
 */
QSet<DataObject *> Album::mediaList2ObjectSet(QVariant mediaList) const
{
    QSet<DataObject*> objectSet;

    if (!mediaList.canConvert<QList<MediaSource*> >()) {
        qWarning() << Q_FUNC_INFO << mediaList << "is not a QList<MediaSource*>";
        return objectSet;
    }

    QList<MediaSource*> mlist = qvariant_cast<QList<MediaSource*> >(mediaList);
    foreach (MediaSource *media, mlist) {
        objectSet.insert(media);
    }

    return objectSet;
}

/*!
 * \brief Album::addMediaSource
 * \param vmedia
 */
void Album::addMediaSource(QVariant vmedia)
{
    MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
    if (media != NULL)
        attach(media);
}

/*!
 * \brief Album::addSelectedMediaSources adds the medias in variant to the album
 * \param mediaList A QVariant containing a QList<MediaSource*> of the medias to add
 * \return The first media that got added in a QVariant
 */
QVariant Album::addSelectedMediaSources(QVariant mediaList)
{
    QSet<DataObject*> mediaSet = mediaList2ObjectSet(mediaList);

    // Only adding ones that aren't already in the set.
    QSet<DataObject*> adding = mediaSet - contained()->getAsSet();

    attachMany(adding);

    QList<DataObject*> sorted(adding.toList());
    qSort(sorted.begin(), sorted.end(), contained()->comparator());

    // We return the sorted-first photo that was added, so we can later jump to
    // that page of the album.
    if (sorted.count() == 0)
        return QVariant();
    return QVariant::fromValue(qobject_cast<MediaSource*>(sorted.at(0)));
}

/*!
 * \brief Album::removeMediaSource
 * \param vmedia
 */
void Album::removeMediaSource(QVariant vmedia)
{
    MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
    if (media != NULL)
        detach(media, true);
}

/*!
 * \brief Album::removeSelectedMediaSources  removes the medias in the variant
 * of the album
 * \param mediaList A QVariant containing a QList<MediaSource*> of the medias to remove
 */
void Album::removeSelectedMediaSources(QVariant mediaList)
{
    QSet<DataObject*> mediaSet = mediaList2ObjectSet(mediaList);
    detachMany(mediaSet);
}

/*!
 * \brief Album::getPage
 * \param page
 * \return
 */
QVariant Album::getPage(int page) const
{
    AlbumPage* album_page = getAlbumPage(page);

    return (album_page != NULL) ? QVariant::fromValue(album_page) : QVariant();
}

/*!
 * \brief Album::getPageForMediaSource
 * \param vmedia
 * \return
 */
int Album::getPageForMediaSource(QVariant vmedia) const
{
    MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
    if (media == NULL)
        return -1;

    if (m_contentPages == NULL)
        return -1;

    int page_count = m_contentPages->count();
    for (int page_ctr = 0; page_ctr < page_count; page_ctr++) {
        AlbumPage* album_page = m_contentPages->getAtAsType<AlbumPage*>(page_ctr);
        if (album_page->contains(media))
            return album_page->pageNumber();
    }

    return -1;
}

/*!
 * \brief Album::containsMedia
 * \param vmedia
 * \return
 */
bool Album::containsMedia(QVariant vmedia) const
{
    MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
    if (media == NULL)
        return false;

    return ContainerSource::contains(media);
}

/*!
 * \brief Album::containsAll
 * \param vContainerSource
 * \return
 */
bool Album::containsAll(QVariant vContainerSource) const
{
    ContainerSource* container = UncheckedVariantToObject<ContainerSource*>(vContainerSource);
    if (container == NULL)
        return false;

    return ContainerSource::containsAll(container);
}

/*!
 * \brief Album::title
 * \return
 */
const QString& Album::title() const
{
    return m_title;
}

/*!
 * \brief Album::setTitle
 * \param title
 */
void Album::setTitle(QString title)
{
    Q_ASSERT(m_albumTable);
    if (title == m_title)
        return;

    m_title = title;
    m_albumTable->setTitle(m_id, m_title);
    emit titleChanged();
}

/*!
 * \brief Album::subtitle
 * \return
 */
const QString& Album::subtitle() const
{
    return m_subtitle;
}

/*!
 * \brief Album::setSubtitle
 * \param subtitle
 */
void Album::setSubtitle(QString subtitle)
{
    Q_ASSERT(m_albumTable);
    if (subtitle == m_subtitle)
        return;

    m_subtitle = subtitle;
    m_albumTable->setSubtitle(m_id, m_subtitle);
    emit subtitleChanged();
}

/*!
 * \brief Album::creationDateTime
 * \return
 */
const QDateTime& Album::creationDateTime() const
{
    return m_creationDateDime;
}

/*!
 * \brief Album::setCreationDateTime
 * \param timestamp
 */
void Album::setCreationDateTime(QDateTime timestamp)
{
    m_creationDateDime = timestamp;
    emit creationDateTimeChanged();
}

/*!
 * \brief Album::albumTemplate
 * \return
 */
AlbumTemplate* Album::albumTemplate() const
{
    return m_albumTemplate;
}

/*!
 * \brief Album::setAlbumTemplate
 * \param albumTemplate
 */
void Album::setAlbumTemplate(AlbumTemplate *albumTemplate)
{
    m_albumTemplate = albumTemplate;
}

/*!
 * \brief Album::isClosed
 * \return
 */
bool Album::isClosed() const
{
    return m_closed;
}

/*!
 * \brief Album::isNewAlbum Check if the album is beeing created, that changes
 * the behavior of some options on the gallery interface
 * \return
 */
bool Album::isNewAlbum() const
{
    return m_newAlbum;
}

/*!
 * \brief Album::totalPageCount
 * \return
 */
int Album::totalPageCount() const
{
    return m_contentPages->count() + PAGES_PER_COVER * 2;
}

/*!
 * \brief Album::contentPageCount
 * \return
 */
int Album::contentPageCount() const
{
    return m_contentPages->count();
}

/*!
 * \brief Album::populatedContentPageCount
 * \return
 */
int Album::populatedContentPageCount() const
{
    return m_populatedPagesCount;
}

/*!
 * \brief Album::firstContentPage
 * \return
 */
int Album::firstContentPage() const
{
    return contentToAbsolutePage(0);
}

/*!
 * \brief Album::lastContentPage
 * \return
 */
int Album::lastContentPage() const
{
    return contentToAbsolutePage(m_contentPages->count() - 1);
}

/*!
 * \brief Album::lastPopulatedContentPage
 * \return
 */
int Album::lastPopulatedContentPage() const
{
    return contentToAbsolutePage(m_populatedPagesCount - 1);
}

/*!
 * \brief Album::firstValidCurrentPage
 * \return
 */
int Album::firstValidCurrentPage() const
{
    return FIRST_VALID_CURRENT_PAGE;
}

/*!
 * \brief Album::lastValidCurrentPage
 * \return
 */
int Album::lastValidCurrentPage() const
{
    // The back cover should show up on the left, hence it's our last.
    return totalPageCount() - 1;
}

/*!
 * \brief Album::currentPage
 * \return
 */
int Album::currentPage() const
{
    return m_currentPage;
}

/*!
 * \brief Album::setCurrentPage
 * \param page
 */
void Album::setCurrentPage(int page)
{
    if (m_currentPage == page)
        return;

    m_currentPage = page;

    notifyCurrentPageChanged();
}

/*!
 * \brief Album::setClosed
 * \param closed
 */
void Album::setClosed(bool closed)
{
    if (m_closed == closed)
        return;

    m_closed = closed;
    notifyClosedChanged();
}

/*!
 * \brief Album::setNewAlbum Set if the album is beeing created, that changes
 * the behavior of some options on the gallery interface
 * \param newAlbum
 */
void Album::setNewAlbum(bool newAlbum)
{
    if (m_newAlbum == newAlbum)
        return;

    m_newAlbum = newAlbum;
    emit newAlbumChanged();
}

/*!
 * \brief Album::setId
 * \param id
 */
void Album::setId(qint64 id)
{
    this->m_id = id;
}

/*!
 * \brief Album::id
 * \return
 */
qint64 Album::id() const
{
    return m_id;
}

/*!
 * \brief Album::coverNickname
 * \return
 */
QString Album::coverNickname() const
{
    return m_coverNickname;
}

/*!
 * \brief Album::setCoverNickname
 * \param name
 */
void Album::setCoverNickname(QString name)
{
    Q_ASSERT(m_albumTable);
    if (name == m_coverNickname)
        return;

    m_coverNickname = name;
    m_albumTable->setCoverNickname(m_id, m_coverNickname);
    emit coverNicknameChanged();
}

/*!
 * \brief Album::contentPages
 * \return returns a SourceCollection representing all AlbumPages held by this Album
 */
SourceCollection* Album::contentPages()
{
    return (SourceCollection*) m_contentPages;
}

/*!
 * \brief Album::getAlbumPage
 * \param page
 * \return
 */
AlbumPage* Album::getAlbumPage(int page) const
{
    int content_page = absoluteToContentPage(page);
    return qobject_cast<AlbumPage*>(m_contentPages->getAt(content_page));
}

/*!
 * \brief Album::qmlAllMediaSources
 * \return
 */
QQmlListProperty<MediaSource> Album::qmlAllMediaSources()
{
    return QQmlListProperty<MediaSource>(this, m_allMediaSources);
}

/*!
 * \brief Album::setAlbumTable
 * \param alumTable
 */
void Album::setAlbumTable(AlbumTable *albumTable)
{
    m_albumTable = albumTable;
}

/*!
 * \brief Album::qmlPages
 * \return
 */
QQmlListProperty<AlbumPage> Album::qmlPages()
{
    return QQmlListProperty<AlbumPage>(this, m_allAlbumPages);
}

/*!
 * \brief Album::notifyCurrentPageChanged
 */
void Album::notifyCurrentPageChanged()
{
    Q_ASSERT(m_albumTable);
    if (!m_refreshingContainer) {
        emit currentPageChanged();
        m_albumTable->setCurrentPage(m_id, m_currentPage);
    }
}

/*!
 * \brief Album::notifyClosedChanged
 */
void Album::notifyClosedChanged()
{
    Q_ASSERT(m_albumTable);
    if (!m_refreshingContainer) {
        emit closedChanged();
        m_albumTable->setIsClosed(m_id, m_closed);
    }
}

/*!
 * \brief Album::notifyPageCountChanged
 */
void Album::notifyPageCountChanged()
{
    if (!m_refreshingContainer)
        emit pageCountChanged();
}

/*!
 * \brief Album::notifyContentPagesChanged
 */
void Album::notifyContentPagesChanged()
{
    if (!m_refreshingContainer)
        emit contentPagesChanged();
}

/*!
 * \brief Album::notifyCurrentPageContentsChanged
 */
void Album::notifyCurrentPageContentsChanged()
{
    emit currentPageContentsChanged();

    // Don't call AlbumCollection::instance directly -- it's possible the
    // object is orphaned
    AlbumCollection* membership = qobject_cast<AlbumCollection*>(memberOf());
    if (membership != NULL)
        membership->notifyAlbumCurrentPageContentsChanged(this);
}

/*!
 * \brief Album::destroySource \reimp
 * \param destroyBacking
 * \param asOrphan
 */
void Album::destroySource(bool destroyBacking, bool asOrphan)
{
    // TODO: Remove album entry in database

    if (m_contentPages != NULL)
        m_contentPages->destroyAll(true, true);
}

/*!
 * \brief Album::notifyContainerContentsChanged
 * \param added
 * \param removed
 */
void Album::notifyContainerContentsChanged(const QSet<DataObject*>* added,
                                           const QSet<DataObject*>* removed)
{
    Q_ASSERT(m_albumTemplate);
    Q_ASSERT(m_albumTable);
    bool stashed_refreshing_container = m_refreshingContainer;
    m_refreshingContainer = true;

    int old_page_count = m_contentPages->count();

    ContainerSource::notifyContainerContentsChanged(added, removed);

    // Update database.
    // If the album isn't in the DB yet, ignore for now.
    if (id() != INVALID_ID) {
        if (added != NULL) {
            QSetIterator<DataObject*> i(*added);
            while (i.hasNext()) {
                MediaSource* media = qobject_cast<MediaSource*>(i.next());
                Q_ASSERT(media != NULL);
                m_albumTable->attachToAlbum(id(), media->id());
            }
        }

        if (removed != NULL) {
            QSetIterator<DataObject*> i(*removed);
            while (i.hasNext()) {
                MediaSource* media = qobject_cast<MediaSource*>(i.next());
                Q_ASSERT(media != NULL);
                m_albumTable->detachFromAlbum(id(), media->id());
            }
        }
    }

    // TODO: Can be smarter than this, but since we don't know how position(s)
    // in the contained sources list have now changed, need to reset and start
    // afresh
    int stashed_current_page = m_currentPage;
    m_contentPages->destroyAll(true, true);
    m_populatedPagesCount = 0;

    // Convert contained DataObjects into a queue to process in order
    QQueue<DataObject*> queue;
    queue.append(contained()->getAll());

    int building_page = contentToAbsolutePage(0);
    bool page_is_left = true; // First page is on the left.

    m_albumTemplate->resetBestFitData();
    // We loop until we've added all photos, and then ensure there's always an
    // even number of pages (the last one of which may be empty).
    while(!queue.isEmpty() || m_contentPages->count() % 2 != 0) {
        PageOrientation next_photo_orientations[2];
        int next_photos_count = std::min(queue.count(), 2);
        for(int i = 0; i < next_photos_count; ++i) {
            MediaSource* photo = qobject_cast<MediaSource*>(queue.at(i));
            Q_ASSERT(photo != NULL);
            QSize size = photo->size();
            next_photo_orientations[i] = (size.height() > size.width()
                                          ? PORTRAIT : LANDSCAPE);
        }

        AlbumTemplatePage* template_page =
                m_albumTemplate->getBestFitPage(page_is_left, next_photos_count,
                                                   next_photo_orientations);
        AlbumPage* page = new AlbumPage(this, building_page, template_page);

        int photos_on_page = std::min(queue.count(), template_page->frameCount());
        for(int i = 0; i < photos_on_page; ++i)
            page->attach(queue.dequeue());

        m_contentPages->add(page);
        if (photos_on_page > 0)
            ++m_populatedPagesCount;

        ++building_page;
        page_is_left = !page_is_left;
    }

    // update QML lists and notify QML watchers
    m_allMediaSources = CastListToType<DataObject*, MediaSource*>(contained()->getAll());
    emit albumContentsChanged();

    m_refreshingContainer = stashed_refreshing_container;

    // If there's no content, add the "add photos" page.
    if (containedCount() == 0)
        createAddPhotosPage();

    // return to stashed current page, unless pages have been removed ... note
    // that this will close the album if empty
    m_currentPage = stashed_current_page;
    if (m_currentPage > lastValidCurrentPage())
        m_currentPage = lastValidCurrentPage();

    if (m_currentPage != stashed_current_page)
        notifyCurrentPageChanged();

    if (m_contentPages->count() != old_page_count)
        notifyPageCountChanged();

    notifyContentPagesChanged();
    // TODO: Again, could be smart and verify the current page has actually
    // changed
    notifyCurrentPageContentsChanged();
}

/*!
 * \brief Album::contentToAbsolutePage
 * \param contentPage
 * \return
 */
int Album::contentToAbsolutePage(int contentPage) const
{
    return contentPage + PAGES_PER_COVER; // The front cover comes first.
}

/*!
 * \brief Album::absoluteToContentPage
 * \param absolutePage
 * \return returns NULL if page number is beyond bounds

 */
int Album::absoluteToContentPage(int absolutePage) const
{
    return absolutePage - PAGES_PER_COVER;
}

/*!
 * \brief Album::createAddPhotosPage
 * Creates the "add photos" pages for an empty album
 * Note: make sure the album is empty before calling this.  No page count
 * alterated signal is fired by this function.
 */
void Album::createAddPhotosPage()
{
    PageOrientation orientation[0];
    AlbumTemplatePage* template_page_add = new AlbumTemplatePage(
                "Add photos", "qml/AlbumViewer/AlbumInternals/AlbumPageLayoutAdd.qml", true, 0);
    AlbumPage* page1 = new AlbumPage(this, contentToAbsolutePage(0), template_page_add);
    AlbumPage* page2 = 0;
    if (m_albumTemplate) {
        page2 = new AlbumPage(this, contentToAbsolutePage(0),
                              m_albumTemplate->getBestFitPage(false, 0, orientation));
    } else {
        AlbumDefaultTemplate albumTemplate;
        page2 = new AlbumPage(this, contentToAbsolutePage(0),
                              albumTemplate.getBestFitPage(false, 0, orientation));
    }

    m_contentPages->add(page1);
    m_contentPages->add(page2);
    m_populatedPagesCount += 1;

    setClosed(true);
}

/*!
 * \brief Album::onAlbumPageContentChanged
 * \param added
 * \param removed
 */
void Album::onAlbumPageContentChanged(const QSet<DataObject*>* added,
                                      const QSet<DataObject*>* removed,
                                      bool notify)
{
    m_allAlbumPages = CastListToType<DataObject*, AlbumPage*>(m_contentPages->getAll());

    bool changed = false;
    if (m_currentPage > lastValidCurrentPage()) {
        // this deals with the closed case too
        m_currentPage = lastValidCurrentPage();
        changed = true;
    }

    notifyPageCountChanged();
    notifyContentPagesChanged();
    if (changed)
        notifyCurrentPageChanged();
}
