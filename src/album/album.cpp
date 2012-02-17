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

#include "album/album.h"

#include <QQueue>

#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "core/data-collection.h"
#include "media/media-source.h"
#include "qml/qml-media-collection-model.h"
#include "util/collections.h"
#include "util/variants.h"

const char *Album::DEFAULT_NAME = "New Photo Album";
const int Album::CLOSED_PAGE = -1;

Album::Album()
  : ContainerSource(DEFAULT_NAME), album_template_(*AlbumDefaultTemplate::instance()),
  name_(DEFAULT_NAME) {
  InitInstance();
}

Album::Album(const AlbumTemplate& album_template)
  : ContainerSource(DEFAULT_NAME), album_template_(album_template), name_(DEFAULT_NAME) {
  InitInstance();
}

Album::Album(const AlbumTemplate& album_template, const QString& name)
  : ContainerSource(name), album_template_(album_template), name_(name) {
  InitInstance();
}

Album::~Album() {
  if (pages_ != NULL) {
    pages_->DestroyAll(false, true);
    delete pages_;
  }
}

void Album::RegisterType() {
  qmlRegisterType<Album>("Gallery", 1, 0, "Album");
}

void Album::InitInstance() {
  creation_date_ = QDate::currentDate();
  current_page_ = CLOSED_PAGE;
  pages_ = new SourceCollection(QString("Pages for ") + name_);
  refreshing_container_ = false;
  
  QObject::connect(pages_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_album_page_content_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
}

void Album::addMediaSource(QVariant vmedia) {
  MediaSource* media = UncheckedVariantToObject<MediaSource*>(vmedia);
  if (media != NULL)
    Attach(media);
}

void Album::addSelectedMediaSources(QVariant vmodel) {
  QmlMediaCollectionModel* model = VariantToObject<QmlMediaCollectionModel*>(vmodel);
  
  // Since QmlEventMarkers are mixed into the QmlEventOverviewModel (which is a
  // subclass of QmlMediaCollectionModel), filter out the markers and only add
  // MediaSources
  AttachMany(
    FilterSetOnlyType<DataObject*, MediaSource*>(model->BackingViewCollection()->GetSelected()));
}

QVariant Album::getPage(int page) const {
  AlbumPage* album_page = GetPage(page);
  
  return (album_page != NULL) ? QVariant::fromValue(album_page) : QVariant();
}

const QString& Album::name() const {
  return name_;
}

const QDate& Album::creation_date() const {
  return creation_date_;
}

const AlbumTemplate& Album::album_template() const {
  return album_template_;
}

bool Album::is_closed() const {
  return current_page_ <= CLOSED_PAGE;
}

void Album::close() {
  set_current_page_number(CLOSED_PAGE);
}

int Album::page_count() const {
  return pages_->Count();
}

int Album::current_page_number() const {
  return (current_page_ > CLOSED_PAGE) ? current_page_ : CLOSED_PAGE;
}

void Album::set_current_page_number(int page) {
  // normalize closed value (to deal with closed state)
  if (page < CLOSED_PAGE)
    page = CLOSED_PAGE;
  
  if (current_page_ == page)
    return;
  
  int old_current_page = current_page_;
  current_page_ = page;
  
  notify_current_page_number_altered();
  notify_current_page_altered();
  if (old_current_page == CLOSED_PAGE || page == CLOSED_PAGE)
    notify_opened_closed();
}

SourceCollection* Album::pages() {
  return (SourceCollection*) pages_;
}

AlbumPage* Album::GetPage(int page) const {
  return qobject_cast<AlbumPage*>(pages_->GetAt(page));
}

QVariant Album::qml_current_page() const {
  return !is_closed() ? QVariant::fromValue(GetPage(current_page_)) : QVariant();
}

QDeclarativeListProperty<MediaSource> Album::qml_all_media_sources() {
  return QDeclarativeListProperty<MediaSource>(this, all_media_sources_);
}

QDeclarativeListProperty<AlbumPage> Album::qml_pages() {
  return QDeclarativeListProperty<AlbumPage>(this, all_album_pages_);
}

void Album::notify_current_page_altered() {
  if (!refreshing_container_)
    emit current_page_altered();
}

void Album::notify_current_page_number_altered() {
  if (!refreshing_container_)
    emit current_page_number_altered();
}

void Album::notify_opened_closed() {
  if (!refreshing_container_)
    emit opened_closed();
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
  
  if (pages_ != NULL)
    pages_->DestroyAll(true, true);
}

void Album::notify_container_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  bool old_refreshing_container = refreshing_container_;
  refreshing_container_ = true;

  ContainerSource::notify_container_contents_altered(added, removed);
  
  // TODO: Can be smarter than this, but since we don't know how position(s)
  // in the contained sources list have now changed, need to reset and start
  // afresh
  int stashed_current_page = current_page_;
  pages_->DestroyAll(true, true);
  
  // Convert contained DataObjects into a queue to process in order
  QQueue<DataObject*> queue;
  queue.append(contained()->GetAll());
  
  int building_page = 0;
  int building_page_template = 1;
  AlbumPage* page = NULL;
  while (queue.count() > 0) {
    if ((page == NULL)
      || (page->ContainedCount() >= page->template_page()->FrameCount())) {
      if (page != NULL)
        pages_->Add(page);
      
      // create the AlbumPage using the current template page
      page = new AlbumPage(this, building_page++,
        album_template_.pages()[building_page_template++]);
      
      // simple algorithm: simply move on to next page in template, returning
      // to first on overflow
      if (building_page_template >= album_template_.page_count())
        building_page_template = 0;
    }
    
    page->Attach(queue.dequeue());
  }
  
  // Deal with last page (destroy if empty)
  if (page != NULL) {
    if (page->ContainedCount() > 0) {
      pages_->Add(page);
    } else {
      page->DestroyOrphan(true);
      delete page;
    }
  }
  
  // update QML lists and notify QML watchers
  all_media_sources_ = CastListToType<DataObject*, MediaSource*>(contained()->GetAll());
  emit album_contents_altered();
  
  refreshing_container_ = old_refreshing_container;

  // return to stashed current page, unless pages have been removed ... note
  // that this will close the album if empty
  current_page_ = stashed_current_page;
  if (current_page_ >= pages_->Count())
    current_page_ = pages_->Count() - 1;
  
  if (current_page_ != stashed_current_page) {
    notify_current_page_number_altered();
    notify_current_page_altered();
    if (current_page_ == CLOSED_PAGE || stashed_current_page == CLOSED_PAGE)
      notify_opened_closed();
  }
  
  // TODO: Again, could be smart and verify the current page has actually
  // changed
  notify_current_page_contents_altered();
  notify_current_page_altered();
}

void Album::on_album_page_content_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  all_album_pages_ = CastListToType<DataObject*, AlbumPage*>(pages_->GetAll());
  
  bool changed = false;
  if (current_page_ >= all_album_pages_.count()) {
    // this deals with the closed case too
    current_page_ = all_album_pages_.count() - 1;
    changed = true;
  }
  
  emit pages_altered();
  if (changed) {
    notify_current_page_number_altered();
    notify_current_page_altered();
    if (current_page_ == CLOSED_PAGE)
      notify_opened_closed();
  }
}
