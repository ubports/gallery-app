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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "overview.h"

#include <QDeclarativeContext>
#include <QDeclarativeView>

#include "album.h"
#include "album-collection.h"

Overview::Overview()
  : agent_(NULL) {
  photos_view_.MonitorSourceCollection(MediaCollection::instance(), NULL);
  albums_view_.MonitorSourceCollection(AlbumCollection::instance(), NULL);
  
  photos_model_ = new QmlMediaModel(NULL);
  photos_model_->Init(&photos_view_);
  
  albums_model_ = new QmlAlbumModel(NULL);
  albums_model_->Init(&albums_view_);
}

Overview::~Overview() {
  delete agent_;
  delete photos_model_;
  delete albums_model_;
}

QmlMediaModel* Overview::photos_model() const {
  return photos_model_;
}

QmlAlbumModel* Overview::albums_model() const {
  return albums_model_;
}

const char* Overview::qml_file_path() const {
  return "qml/Overview.qml";
}

void Overview::Prepare(QDeclarativeView* view) {
  view->rootContext()->setContextProperty("photosGridModel", photos_model_);
  view->rootContext()->setContextProperty("albumsGridModel", albums_model_);
}

void Overview::SwitchingTo(QDeclarativeView* view) {
  Q_ASSERT(agent_ == NULL);
  agent_ = new OverviewAgent(view);
  
  QObject::connect(agent_, SIGNAL(photo_activated(int)), this,
    SLOT(on_photo_activated(int)));
  
  QObject::connect(agent_, SIGNAL(photo_selection_toggled(int)), this,
    SLOT(on_photo_selection_toggled(int)));
  
  QObject::connect(agent_, SIGNAL(photos_unselect_all()), this,
    SLOT(on_photos_unselect_all()));
  
  QObject::connect(agent_, SIGNAL(create_album_from_selected_photos()), this,
    SLOT(on_create_album_from_selected_photos()));
}

void Overview::SwitchingFrom(QDeclarativeView* view) {
  delete agent_;
  agent_ = NULL;
}

void Overview::on_photo_activated(int media_number) {
  MediaSource* media_object = qobject_cast<MediaSource*>(
    photos_view_.FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    emit photo_activated(media_object);
  else
    qDebug("Unable to locate activated photo #%d", media_number);
}

void Overview::on_photo_selection_toggled(int media_number) {
  MediaSource* media_object = qobject_cast<MediaSource*>(
    photos_view_.FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    photos_view_.ToggleSelect(media_object);
  else
    qDebug("Unable to located toggled photo #%d", media_number);
}

void Overview::on_photos_unselect_all() {
  photos_view_.UnselectAll();
}

void Overview::on_create_album_from_selected_photos() {
  if (photos_view_.GetSelectedCount() == 0)
    return;
  
  Album *album = new Album();
  album->AttachMany(photos_view_.GetSelected());
  
  AlbumCollection::instance()->Add(album);
}
