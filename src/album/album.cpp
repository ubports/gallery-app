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

#include "album/album.h"

#include <QQueue>

#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "core/data-collection.h"
#include "media/media-source.h"
#include "media/media-collection.h"
#include "qml/qml-media-collection-model.h"
#include "util/collections.h"
#include "util/variants.h"
#include "database/database.h"

const char *Album::DEFAULT_TITLE = "New Photo Album";
const char *Album::DEFAULT_SUBTITLE = "Subtitle";
const int Album::PAGES_PER_COVER = 1;
// We use the left page's number as current_page_.  -1 because the cover is 0
// and we want it to show up on the right.
const int Album::FIRST_VALID_CURRENT_PAGE = -1;

Album::Album()
  : ContainerSource(DEFAULT_TITLE, MediaCollection::ExposureDateTimeAscendingComparator),
    album_template_(AlbumDefaultTemplate::instance()), title_(DEFAULT_TITLE),
    subtitle_(DEFAULT_SUBTITLE) {
  InitInstance();
}

Album::Album(AlbumTemplate* album_template)
  : ContainerSource(DEFAULT_TITLE, MediaCollection::ExposureDateTimeAscendingComparator),
    album_template_(album_template), title_(DEFAULT_TITLE), subtitle_(DEFAULT_SUBTITLE) {
  InitInstance();
}

Album::Album(AlbumTemplate* album_template, const QString& title,
  const QString& subtitle)
  : ContainerSource(title, MediaCollection::ExposureDateTimeAscendingComparator),
    album_template_(album_template), title_(title), subtitle_(subtitle) {
  InitInstance();
}

Album::~Album() {
  if (content_pages_ != NULL) {
    content_pages_->DestroyAll(false, true);
    delete content_pages_;
  }
}

void Album::RegisterType() {
  qmlRegisterType<Album>("Gallery", 1, 0, "Album");
}

void Album::InitInstance() {
  creation_date_time_ = QDateTime::currentDateTime();
  current_page_ = FIRST_VALID_CURRENT_PAGE;
  closed_ = true;
  content_pages_ = new SourceCollection(QString("Pages for ") + title_);
  refreshing_container_ = false;
  id_ = INVALID_ID;
  cover_nickname_ = "default";
  
  QObject::connect(content_pages_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_album_page_content_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
}

void Album::addMediaSource(QVariant vmedia) {
  MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
  if (media != NULL)
    Attach(media);
}

QVariant Album::addSelectedMediaSources(QVariant vmodel) {
  QmlMediaCollectionModel* model = VariantToObject<QmlMediaCollectionModel*>(vmodel);
  
  // Since Events are mixed into the QmlEventOverviewModel (which is a
  // subclass of QmlMediaCollectionModel), filter them out and only add
  // MediaSources
  QSet<DataObject*> media_sources =
      FilterSetOnlyType<DataObject*, MediaSource*>(
          model->BackingViewCollection()->GetSelected());

  // Only adding ones that aren't already in the set.
  QSet<DataObject*> adding = media_sources - contained()->GetAsSet();

  AttachMany(media_sources);

  QList<DataObject*> sorted(adding.toList());
  qSort(sorted.begin(), sorted.end(), contained()->comparator());

  // We return the sorted-first photo that was added, so we can later jump to
  // that page of the album.
  if (sorted.count() == 0)
    return QVariant();
  return QVariant::fromValue(qobject_cast<MediaSource*>(sorted.at(0)));
}

void Album::removeMediaSource(QVariant vmedia) {
  MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
  if (media != NULL)
    Detach(media);
  
  // TODO: see comment in removeSelectedMediaSources()
  if (ContainedCount() == 0)
    AlbumCollection::instance()->Destroy(this, true, true);
}

void Album::removeSelectedMediaSources(QVariant vmodel) {
  QmlMediaCollectionModel* model = VariantToObject<QmlMediaCollectionModel*>(vmodel);

  DetachMany(
    FilterSetOnlyType<DataObject*, MediaSource*>(
      model->BackingViewCollection()->GetSelected()));

  // TODO: it's unfortunate that this has to happen here AND in AlbumCollection
  // to handle all photos removed AND all photos deleted.  Ideally they could
  // be combined somewhere in notify_container_contents_altered() below, but it
  // seemed too risky to do the equivalent of delete this somewhere so deep in
  // the call chain.  This is right up at the top where we're sure it's safe.
  if (ContainedCount() == 0)
    AlbumCollection::instance()->Destroy(this, true, true);
}

QVariant Album::getPage(int page) const {
  AlbumPage* album_page = GetPage(page);
  
  return (album_page != NULL) ? QVariant::fromValue(album_page) : QVariant();
}

int Album::getPageForMediaSource(QVariant vmedia) const {
  MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
  if (media == NULL)
    return -1;
  
  if (content_pages_ == NULL)
    return -1;
  
  int page_count = content_pages_->Count();
  for (int page_ctr = 0; page_ctr < page_count; page_ctr++) {
    AlbumPage* album_page = content_pages_->GetAtAsType<AlbumPage*>(page_ctr);
    if (album_page->Contains(media))
      return album_page->page_number();
  }
  
  return -1;
}

const QString& Album::title() const {
  return title_;
}

const QString& Album::subtitle() const {
  return subtitle_;
}

const QDateTime& Album::creation_date_time() const {
  return creation_date_time_;
}

void Album::set_creation_date_time(QDateTime timestamp) {
  creation_date_time_ = timestamp;
  emit creation_date_time_altered();
}

AlbumTemplate* Album::album_template() const {
  return album_template_;
}

bool Album::is_closed() const {
  return closed_;
}

int Album::total_page_count() const {
  return content_pages_->Count() + PAGES_PER_COVER * 2;
}

int Album::content_page_count() const {
  return content_pages_->Count();
}

int Album::first_content_page() const {
  return content_to_absolute_page(0);
}

int Album::last_content_page() const {
  return content_to_absolute_page(content_pages_->Count() - 1);
}

int Album::first_valid_current_page() const {
  return FIRST_VALID_CURRENT_PAGE;
}

int Album::last_valid_current_page() const {
  // The back cover should show up on the left, hence it's our last.
  return total_page_count() - 1;
}

int Album::current_page() const {
  return current_page_;
}

void Album::set_current_page(int page) {
  if (current_page_ == page)
    return;
  
  current_page_ = page;
  
  notify_current_page_altered();
}

void Album::set_closed(bool closed) {
  if (closed_ == closed)
    return;
  
  closed_ = closed;
  notify_closed_altered();
}

void Album::set_id(qint64 id) {
  this->id_ = id;
}

qint64 Album::get_id() {
  return id_;
}

QString Album::cover_nickname() const {
  return cover_nickname_;
}

void Album::set_cover_nickname(QString name) {
  bool signal = cover_nickname_ != name;
  cover_nickname_ = name;
  
  if (signal) {
    Database::instance()->get_album_table()->set_cover_nickname(id_, cover_nickname_);
    emit coverNicknameAltered();
  }
}

SourceCollection* Album::content_pages() {
  return (SourceCollection*) content_pages_;
}

AlbumPage* Album::GetPage(int page) const {
  int content_page = absolute_to_content_page(page);
  return qobject_cast<AlbumPage*>(content_pages_->GetAt(content_page));
}

QDeclarativeListProperty<MediaSource> Album::qml_all_media_sources() {
  return QDeclarativeListProperty<MediaSource>(this, all_media_sources_);
}

QDeclarativeListProperty<AlbumPage> Album::qml_pages() {
  return QDeclarativeListProperty<AlbumPage>(this, all_album_pages_);
}

void Album::notify_current_page_altered() {
  if (!refreshing_container_) {
    emit current_page_altered();
    Database::instance()->get_album_table()->set_current_page(id_, current_page_);
  }
}

void Album::notify_closed_altered() {
  if (!refreshing_container_) {
    emit closedAltered();
    Database::instance()->get_album_table()->set_is_closed(id_, closed_);
  }
}

void Album::notify_page_count_altered() {
  if (!refreshing_container_)
    emit pageCountAltered();
}

void Album::notify_content_pages_altered() {
  if (!refreshing_container_)
    emit contentPagesAltered();
}

void Album::notify_current_page_contents_altered() {
  emit current_page_contents_altered();
  
  // Don't call AlbumCollection::instance directly -- it's possible the
  // object is orphaned
  AlbumCollection* membership = qobject_cast<AlbumCollection*>(member_of());
  if (membership != NULL)
    membership->notify_album_current_page_contents_altered(this);
}

void Album::DestroySource(bool destroy_backing, bool as_orphan) {
  // TODO: Remove album entry in database
  
  if (content_pages_ != NULL)
    content_pages_->DestroyAll(true, true);
}

void Album::notify_container_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  bool stashed_refreshing_container = refreshing_container_;
  refreshing_container_ = true;

  int old_page_count = content_pages_->Count();

  ContainerSource::notify_container_contents_altered(added, removed);
  
  // Update database.
  // If the album isn't in the DB yet, ignore for now.
  if (get_id() != INVALID_ID) {
    if (added != NULL) {
      QSetIterator<DataObject*> i(*added);
      while (i.hasNext()) {
        MediaSource* media = qobject_cast<MediaSource*>(i.next());
        Q_ASSERT(media != NULL);
        Database::instance()->get_album_table()->attach_to_album(get_id(), 
                                                             media->get_id());
      }
    }
    
    if (removed != NULL) {
      QSetIterator<DataObject*> i(*removed);
      while (i.hasNext()) {
        MediaSource* media = qobject_cast<MediaSource*>(i.next());
        Q_ASSERT(media != NULL);
        Database::instance()->get_album_table()->detach_from_album(get_id(), 
                                                             media->get_id());
      }
    }
  }
  
  // TODO: Can be smarter than this, but since we don't know how position(s)
  // in the contained sources list have now changed, need to reset and start
  // afresh
  int stashed_current_page = current_page_;
  content_pages_->DestroyAll(true, true);
  
  // Convert contained DataObjects into a queue to process in order
  QQueue<DataObject*> queue;
  queue.append(contained()->GetAll());
  
  int building_page = content_to_absolute_page(0);
  bool page_is_left = true; // First page is on the left.

  album_template_->reset_best_fit_data();
  // We loop until we've added all photos, and then ensure there's always an
  // even number of pages (the last one of which may be empty).
  while(!queue.isEmpty() || content_pages_->Count() % 2 != 0) {
    PageOrientation next_photo_orientations[2];
    int next_photos_count = std::min(queue.count(), 2);
    for(int i = 0; i < next_photos_count; ++i) {
      MediaSource* photo = qobject_cast<MediaSource*>(queue.at(i));
      Q_ASSERT(photo != NULL);
      next_photo_orientations[i] = (photo->height() > photo->width()
                                    ? PORTRAIT : LANDSCAPE);
    }

    AlbumTemplatePage* template_page =
        album_template_->get_best_fit_page(page_is_left, next_photos_count,
                                           next_photo_orientations);
    AlbumPage* page = new AlbumPage(this, building_page, template_page);

    int photos_on_page = std::min(queue.count(), template_page->FrameCount());
    for(int i = 0; i < photos_on_page; ++i)
      page->Attach(queue.dequeue());

    content_pages_->Add(page);

    ++building_page;
    page_is_left = !page_is_left;
  }

  // update QML lists and notify QML watchers
  all_media_sources_ = CastListToType<DataObject*, MediaSource*>(contained()->GetAll());
  emit album_contents_altered();
  
  refreshing_container_ = stashed_refreshing_container;

  // return to stashed current page, unless pages have been removed ... note
  // that this will close the album if empty
  current_page_ = stashed_current_page;
  if (current_page_ > last_valid_current_page())
    current_page_ = last_valid_current_page();
  
  if (current_page_ != stashed_current_page)
    notify_current_page_altered();

  if (content_pages_->Count() != old_page_count)
    notify_page_count_altered();
  
  notify_content_pages_altered();
  // TODO: Again, could be smart and verify the current page has actually
  // changed
  notify_current_page_contents_altered();
}

int Album::content_to_absolute_page(int content_page) const {
  return content_page + PAGES_PER_COVER; // The front cover comes first.
}

int Album::absolute_to_content_page(int absolute_page) const {
  return absolute_page - PAGES_PER_COVER;
}

void Album::on_album_page_content_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  all_album_pages_ = CastListToType<DataObject*, AlbumPage*>(content_pages_->GetAll());
  
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
