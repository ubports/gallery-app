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
#include "core/utils.h"
#include "media/media-source.h"

const char *Album::DEFAULT_NAME = "New Photo Album";

Album::Album()
  : ContainerSource(DEFAULT_NAME), album_template_(*AlbumDefaultTemplate::instance()),
  name_(DEFAULT_NAME) {
  Init();
}

Album::Album(const AlbumTemplate& album_template)
  : ContainerSource(DEFAULT_NAME), album_template_(album_template), name_(DEFAULT_NAME) {
  Init();
}

Album::Album(const AlbumTemplate& album_template, const QString& name)
  : ContainerSource(name), album_template_(album_template), name_(name) {
  Init();
}

Album::~Album() {
  delete pages_;
}

void Album::RegisterType() {
  qmlRegisterType<Album>("org.yorba.qt.album", 1, 0, "Album");
}

void Album::Init() {
  current_page_ = 0;
  pages_ = new SourceCollection(QString("Pages for ") + name_);
  
  QObject::connect(pages_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_album_page_content_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
}

const QString& Album::name() const {
  return name_;
}

const AlbumTemplate& Album::album_template() const {
  return album_template_;
}

bool Album::is_closed() const {
  return current_page_ < 0;
}

void Album::close() {
  set_current_page(-1);
}

int Album::page_count() const {
  return pages_->Count();
}

int Album::current_page() const {
  return (current_page_ >= 0) ? current_page_ : -1;
}

void Album::set_current_page(int page) {
  current_page_ = (page >= 0) ? page : -1;
  
  notify_current_page_altered();
}

SourceCollection* Album::pages() {
  return (SourceCollection*) pages_;
}

AlbumPage* Album::GetPage(int page) const {
  return qobject_cast<AlbumPage*>(pages_->GetAt(page));
}

QDeclarativeListProperty<MediaSource> Album::qml_all_media_sources() {
  return QDeclarativeListProperty<MediaSource>(this, all_media_sources_);
}

QDeclarativeListProperty<AlbumPage> Album::qml_pages() {
  return QDeclarativeListProperty<AlbumPage>(this, all_album_pages_);
}

void Album::notify_current_page_altered() {
  emit current_page_altered();
}

void Album::notify_current_page_contents_altered() {
  emit current_page_contents_altered();
  
  // Don't call AlbumCollection::instance directly -- it's possible the
  // object is orphaned
  AlbumCollection* membership = qobject_cast<AlbumCollection*>(member_of());
  if (membership != NULL)
    membership->notify_album_current_page_contents_altered(this);
}

void Album::DestroySource(bool destroy_backing) {
  // TODO: Remove album entry in database
}

void Album::notify_container_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  ContainerSource::notify_container_contents_altered(added, removed);
  
  // TODO: Can be smarter than this, but since we don't know how position(s)
  // in the contained sources list have now changed, need to reset and start
  // afresh
  pages_->DestroyAll(false, true);
  
  // Convert contained DataObjects into a queue to process in order
  QQueue<DataObject*> queue;
  queue.append(contained()->GetAll());
  
  int current_page = 0;
  int current_page_template = 1;
  AlbumPage* page = NULL;
  while (queue.count() > 0) {
    if ((page == NULL)
      || (page->ContainedCount() >= page->template_page()->FrameCount())) {
      if (page != NULL)
        pages_->Add(page);
       
      page = new AlbumPage(current_page++,
        album_template_.pages()[current_page_template++]);
      if (current_page_template >= album_template_.page_count())
        current_page_template = 0;
    }
    
    page->Attach(queue.dequeue());
  }
  
  if (page != NULL) {
    if (page->ContainedCount() > 0)
      pages_->Add(page);
    else
      delete page;
  }
  
  // update QML lists and notify QML watchers
  all_media_sources_ = CastDataCollectionToList<MediaSource*>(contained());
  emit album_contents_altered();
  
  // TODO: Again, could be smart and verify the current page has actually
  // changed
  notify_current_page_contents_altered();
}

void Album::on_album_page_content_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  all_album_pages_ = CastDataCollectionToList<AlbumPage*>(pages_);
  emit pages_altered();
}
