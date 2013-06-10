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
#include "album-table.h"
#include "database.h"

// media
#include "media-collection.h"

// qml
#include "qml-media-collection-model.h"

// util
#include "variants.h"

// src
#include "gallery-manager.h"

const char *Album::DEFAULT_TITLE = "New Photo Album";
const char *Album::DEFAULT_SUBTITLE = "Subtitle";
const int Album::PAGES_PER_COVER = 1;
// We use the left page's number as current_page_.  -1 because the cover is 0
// and we want it to show up on the right.
const int Album::FIRST_VALID_CURRENT_PAGE = -1;

/*!
 * \brief Album::Album
 * \param parent
 */
Album::Album(QObject * parent)
    : ContainerSource(parent, DEFAULT_TITLE, MediaCollection::exposureDateTimeAscendingComparator),
      album_template_(GalleryManager::instance()->album_default_template()), title_(DEFAULT_TITLE),
      subtitle_(DEFAULT_SUBTITLE)
{
    InitInstance();
}

/*!
 * \brief Album::Album
 * \param parent
 * \param album_template
 */
Album::Album(QObject * parent, AlbumTemplate* album_template)
    : ContainerSource(parent, DEFAULT_TITLE, MediaCollection::exposureDateTimeAscendingComparator),
      album_template_(album_template), title_(DEFAULT_TITLE), subtitle_(DEFAULT_SUBTITLE)
{
    InitInstance();
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
Album::Album(QObject * parent, AlbumTemplate* album_template, const QString& title,
             const QString& subtitle, qint64 id, QDateTime creation_timestamp, bool closed,
             int current_page, const QString &cover_nickname)
    : ContainerSource(parent, title, MediaCollection::exposureDateTimeAscendingComparator),
      album_template_(album_template), title_(title), subtitle_(subtitle)
{
    InitInstance();

    // replace defaults with what was read from DB
    cover_nickname_ = cover_nickname;
    creation_date_time_ = creation_timestamp;
    current_page_ = current_page;
    closed_ = closed;
    id_ = id;
}

/*!
 * \brief Album::~Album
 */
Album::~Album()
{
    delete content_pages_;
}

/*!
 * \brief Album::RegisterType
 */
void Album::RegisterType()
{
    qmlRegisterType<Album>("Gallery", 1, 0, "Album");
}

/*!
 * \brief Album::InitInstance
 */
void Album::InitInstance()
{
    creation_date_time_ = QDateTime::currentDateTime();
    current_page_ = FIRST_VALID_CURRENT_PAGE;
    closed_ = true;
    populated_pages_count_ = 0;
    content_pages_ = new SourceCollection(QString("Pages for ") + title_);
    refreshing_container_ = false;
    id_ = INVALID_ID;
    cover_nickname_ = "default";

    create_add_photos_page();

    QObject::connect(content_pages_,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*)),
                     this,
                     SLOT(on_album_page_content_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
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
 * \brief Album::addSelectedMediaSources
 * \param vmodel
 * \return
 */
QVariant Album::addSelectedMediaSources(QVariant vmodel)
{
    QmlMediaCollectionModel* model = VariantToObject<QmlMediaCollectionModel*>(vmodel);

    // Since Events are mixed into the QmlEventOverviewModel (which is a
    // subclass of QmlMediaCollectionModel), filter them out and only add
    // MediaSources
    QSet<DataObject*> media_sources =
            FilterSetOnlyType<DataObject*, MediaSource*>(
                model->BackingViewCollection()->getSelected());

    // Only adding ones that aren't already in the set.
    QSet<DataObject*> adding = media_sources - contained()->getAsSet();

    attachMany(media_sources);

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
        detach(media);
}

/*!
 * \brief Album::removeSelectedMediaSources
 * \param vmodel
 */
void Album::removeSelectedMediaSources(QVariant vmodel)
{
    QmlMediaCollectionModel* model = VariantToObject<QmlMediaCollectionModel*>(vmodel);

    detachMany( FilterSetOnlyType<DataObject*, MediaSource*>(
                    model->BackingViewCollection()->getSelected()));
}

/*!
 * \brief Album::getPage
 * \param page
 * \return
 */
QVariant Album::getPage(int page) const
{
    AlbumPage* album_page = GetPage(page);

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

    if (content_pages_ == NULL)
        return -1;

    int page_count = content_pages_->count();
    for (int page_ctr = 0; page_ctr < page_count; page_ctr++) {
        AlbumPage* album_page = content_pages_->getAtAsType<AlbumPage*>(page_ctr);
        if (album_page->contains(media))
            return album_page->page_number();
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
    return title_;
}

/*!
 * \brief Album::set_title
 * \param title
 */
void Album::set_title(QString title)
{
    bool signal = title_ != title;
    title_ = title;

    if (signal) {
        GalleryManager::instance()->database()->get_album_table()->set_title(id_, title_);
        emit title_altered();
    }
}

/*!
 * \brief Album::subtitle
 * \return
 */
const QString& Album::subtitle() const
{
    return subtitle_;
}

/*!
 * \brief Album::set_subtitle
 * \param subtitle
 */
void Album::set_subtitle(QString subtitle)
{
    bool signal = subtitle_ != subtitle;
    subtitle_ = subtitle;

    if (signal) {
        GalleryManager::instance()->database()->get_album_table()->set_subtitle(id_, subtitle_);
        emit subtitle_altered();
    }
}

/*!
 * \brief Album::creation_date_time
 * \return
 */
const QDateTime& Album::creation_date_time() const
{
    return creation_date_time_;
}

/*!
 * \brief Album::set_creation_date_time
 * \param timestamp
 */
void Album::set_creation_date_time(QDateTime timestamp)
{
    creation_date_time_ = timestamp;
    emit creation_date_time_altered();
}

/*!
 * \brief Album::album_template
 * \return
 */
AlbumTemplate* Album::album_template() const
{
    return album_template_;
}

/*!
 * \brief Album::is_closed
 * \return
 */
bool Album::is_closed() const
{
    return closed_;
}

/*!
 * \brief Album::total_page_count
 * \return
 */
int Album::total_page_count() const
{
    return content_pages_->count() + PAGES_PER_COVER * 2;
}

/*!
 * \brief Album::content_page_count
 * \return
 */
int Album::content_page_count() const
{
    return content_pages_->count();
}

/*!
 * \brief Album::populated_content_page_count
 * \return
 */
int Album::populated_content_page_count() const
{
    return populated_pages_count_;
}

/*!
 * \brief Album::first_content_page
 * \return
 */
int Album::first_content_page() const
{
    return content_to_absolute_page(0);
}

/*!
 * \brief Album::last_content_page
 * \return
 */
int Album::last_content_page() const
{
    return content_to_absolute_page(content_pages_->count() - 1);
}

/*!
 * \brief Album::last_populated_content_page
 * \return
 */
int Album::last_populated_content_page() const
{
    return content_to_absolute_page(populated_pages_count_ - 1);
}

/*!
 * \brief Album::first_valid_current_page
 * \return
 */
int Album::first_valid_current_page() const
{
    return FIRST_VALID_CURRENT_PAGE;
}

/*!
 * \brief Album::last_valid_current_page
 * \return
 */
int Album::last_valid_current_page() const
{
    // The back cover should show up on the left, hence it's our last.
    return total_page_count() - 1;
}

/*!
 * \brief Album::current_page
 * \return
 */
int Album::current_page() const
{
    return current_page_;
}

/*!
 * \brief Album::set_current_page
 * \param page
 */
void Album::set_current_page(int page)
{
    if (current_page_ == page)
        return;

    current_page_ = page;

    notify_current_page_altered();
}

/*!
 * \brief Album::set_closed
 * \param closed
 */
void Album::set_closed(bool closed)
{
    if (closed_ == closed)
        return;

    closed_ = closed;
    notify_closed_altered();
}

/*!
 * \brief Album::set_id
 * \param id
 */
void Album::set_id(qint64 id)
{
    this->id_ = id;
}

/*!
 * \brief Album::get_id
 * \return
 */
qint64 Album::get_id()
{
    return id_;
}

/*!
 * \brief Album::cover_nickname
 * \return
 */
QString Album::cover_nickname() const
{
    return cover_nickname_;
}

/*!
 * \brief Album::set_cover_nickname
 * \param name
 */
void Album::set_cover_nickname(QString name)
{
    bool signal = cover_nickname_ != name;
    cover_nickname_ = name;

    if (signal) {
        GalleryManager::instance()->database()->get_album_table()->set_cover_nickname(id_, cover_nickname_);
        emit coverNicknameAltered();
    }
}

/*!
 * \brief Album::content_pages
 * \return returns a SourceCollection representing all AlbumPages held by this Album
 */
SourceCollection* Album::content_pages()
{
    return (SourceCollection*) content_pages_;
}

/*!
 * \brief Album::GetPage
 * \param page
 * \return
 */
AlbumPage* Album::GetPage(int page) const
{
    int content_page = absolute_to_content_page(page);
    return qobject_cast<AlbumPage*>(content_pages_->getAt(content_page));
}

/*!
 * \brief Album::qml_all_media_sources
 * \return
 */
QQmlListProperty<MediaSource> Album::qml_all_media_sources()
{
    return QQmlListProperty<MediaSource>(this, all_media_sources_);
}

/*!
 * \brief Album::qml_pages
 * \return
 */
QQmlListProperty<AlbumPage> Album::qml_pages()
{
    return QQmlListProperty<AlbumPage>(this, all_album_pages_);
}

/*!
 * \brief Album::notify_current_page_altered
 */
void Album::notify_current_page_altered()
{
    if (!refreshing_container_) {
        emit current_page_altered();
        GalleryManager::instance()->database()->get_album_table()->set_current_page(id_, current_page_);
    }
}

/*!
 * \brief Album::notify_closed_altered
 */
void Album::notify_closed_altered()
{
    if (!refreshing_container_) {
        emit closedAltered();
        GalleryManager::instance()->database()->get_album_table()->set_is_closed(id_, closed_);
    }
}

/*!
 * \brief Album::notify_page_count_altered
 */
void Album::notify_page_count_altered()
{
    if (!refreshing_container_)
        emit pageCountAltered();
}

/*!
 * \brief Album::notify_content_pages_altered
 */
void Album::notify_content_pages_altered()
{
    if (!refreshing_container_)
        emit contentPagesAltered();
}

/*!
 * \brief Album::notify_current_page_contents_altered
 */
void Album::notify_current_page_contents_altered()
{
    emit current_page_contents_altered();

    // Don't call AlbumCollection::instance directly -- it's possible the
    // object is orphaned
    AlbumCollection* membership = qobject_cast<AlbumCollection*>(memberOf());
    if (membership != NULL)
        membership->notify_album_current_page_contents_altered(this);
}

/*!
 * \brief Album::DestroySource \reimp
 * \param destroy_backing
 * \param as_orphan
 */
void Album::destroySource(bool destroy_backing, bool as_orphan)
{
    // TODO: Remove album entry in database

    if (content_pages_ != NULL)
        content_pages_->destroyAll(true, true);
}

/*!
 * \brief Album::notify_container_contents_altered
 * \param added
 * \param removed
 */
void Album::notifyContainerContentsChanged(const QSet<DataObject*>* added,
                                              const QSet<DataObject*>* removed)
{
    bool stashed_refreshing_container = refreshing_container_;
    refreshing_container_ = true;

    int old_page_count = content_pages_->count();

    ContainerSource::notifyContainerContentsChanged(added, removed);

    // Update database.
    // If the album isn't in the DB yet, ignore for now.
    if (get_id() != INVALID_ID) {
        if (added != NULL) {
            QSetIterator<DataObject*> i(*added);
            while (i.hasNext()) {
                MediaSource* media = qobject_cast<MediaSource*>(i.next());
                Q_ASSERT(media != NULL);
                GalleryManager::instance()->database()->get_album_table()->attach_to_album(get_id(), media->id());
            }
        }

        if (removed != NULL) {
            QSetIterator<DataObject*> i(*removed);
            while (i.hasNext()) {
                MediaSource* media = qobject_cast<MediaSource*>(i.next());
                Q_ASSERT(media != NULL);
                GalleryManager::instance()->database()->get_album_table()->detach_from_album(get_id(), media->id());
            }
        }
    }

    // TODO: Can be smarter than this, but since we don't know how position(s)
    // in the contained sources list have now changed, need to reset and start
    // afresh
    int stashed_current_page = current_page_;
    content_pages_->destroyAll(true, true);
    populated_pages_count_ = 0;

    // Convert contained DataObjects into a queue to process in order
    QQueue<DataObject*> queue;
    queue.append(contained()->getAll());

    int building_page = content_to_absolute_page(0);
    bool page_is_left = true; // First page is on the left.

    album_template_->reset_best_fit_data();
    // We loop until we've added all photos, and then ensure there's always an
    // even number of pages (the last one of which may be empty).
    while(!queue.isEmpty() || content_pages_->count() % 2 != 0) {
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
                album_template_->get_best_fit_page(page_is_left, next_photos_count,
                                                   next_photo_orientations);
        AlbumPage* page = new AlbumPage(this, building_page, template_page);

        int photos_on_page = std::min(queue.count(), template_page->FrameCount());
        for(int i = 0; i < photos_on_page; ++i)
            page->attach(queue.dequeue());

        content_pages_->add(page);
        if (photos_on_page > 0)
            ++populated_pages_count_;

        ++building_page;
        page_is_left = !page_is_left;
    }

    // update QML lists and notify QML watchers
    all_media_sources_ = CastListToType<DataObject*, MediaSource*>(contained()->getAll());
    emit album_contents_altered();

    refreshing_container_ = stashed_refreshing_container;

    // If there's no content, add the "add photos" page.
    if (containedCount() == 0)
        create_add_photos_page();

    // return to stashed current page, unless pages have been removed ... note
    // that this will close the album if empty
    current_page_ = stashed_current_page;
    if (current_page_ > last_valid_current_page())
        current_page_ = last_valid_current_page();

    if (current_page_ != stashed_current_page)
        notify_current_page_altered();

    if (content_pages_->count() != old_page_count)
        notify_page_count_altered();

    notify_content_pages_altered();
    // TODO: Again, could be smart and verify the current page has actually
    // changed
    notify_current_page_contents_altered();
}

/*!
 * \brief Album::content_to_absolute_page
 * \param content_page
 * \return
 */
int Album::content_to_absolute_page(int content_page) const
{
    return content_page + PAGES_PER_COVER; // The front cover comes first.
}

/*!
 * \brief Album::absolute_to_content_page
 * \param absolute_page
 * \return returns NULL if page number is beyond bounds

 */
int Album::absolute_to_content_page(int absolute_page) const
{
    return absolute_page - PAGES_PER_COVER;
}

/*!
 * \brief Album::create_add_photos_page
 * Creates the "add photos" pages for an empty album
 * Note: make sure the album is empty before calling this.  No page count
 * alterated signal is fired by this function.
 */
void Album::create_add_photos_page()
{
    PageOrientation orientation[0];
    AlbumTemplatePage* template_page_add = new AlbumTemplatePage(
                "Add photos", "qml/Components/AlbumInternals/AlbumPageLayoutAdd.qml", true, 0);
    AlbumPage* page1 = new AlbumPage(this, content_to_absolute_page(0), template_page_add);
    AlbumPage* page2 = new AlbumPage(this, content_to_absolute_page(0),
                                     album_template_->get_best_fit_page(false, 0, orientation));

    content_pages_->add(page1);
    content_pages_->add(page2);
    populated_pages_count_ += 1;

    set_closed(true);
}

/*!
 * \brief Album::on_album_page_content_altered
 * \param added
 * \param removed
 */
void Album::on_album_page_content_altered(const QSet<DataObject*>* added,
                                          const QSet<DataObject*>* removed)
{
    all_album_pages_ = CastListToType<DataObject*, AlbumPage*>(content_pages_->getAll());

    bool changed = false;
    if (current_page_ > last_valid_current_page()) {
        // this deals with the closed case too
        current_page_ = last_valid_current_page();
        changed = true;
    }

    notify_page_count_altered();
    notify_content_pages_altered();
    if (changed)
        notify_current_page_altered();
}
