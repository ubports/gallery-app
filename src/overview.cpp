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

#include "album.h"
#include "album-collection.h"
#include "default-album-template.h"
#include "media-collection.h"

static const char* CTX_MEDIA_MODEL = "ctx_overview_media_model";
static const char* CTX_ALBUMS_MODEL = "ctx_overview_albums_model";
static const char* CTX_ALBUM_PICKER_MODEL = "ctx_album_picker_model";

Overview::Overview(QDeclarativeView* view)
  : QmlPage(view, "overview"), media_view_("Media ViewCollection"),
  albums_view_("Albums ViewCollection") {
  media_view_.MonitorDataCollection(MediaCollection::instance(), NULL, false);
  albums_view_.MonitorDataCollection(AlbumCollection::instance(), NULL, false);
  
  media_model_ = new QmlMediaModel(NULL);
  media_model_->Init(&media_view_);
  
  albums_model_ = new QmlAlbumCollectionModel(NULL);
  albums_model_->Init(&albums_view_);
}

Overview::~Overview() {
  delete media_model_;
  delete albums_model_;
}

QmlMediaModel* Overview::media_model() const {
  return media_model_;
}

QmlAlbumCollectionModel* Overview::albums_model() const {
  return albums_model_;
}

const char* Overview::qml_rc() const {
  return "qrc:/rc/qml/Overview.qml";
}

void Overview::PrepareContext() {
  SetContextProperty(CTX_MEDIA_MODEL, NULL);
  SetContextProperty(CTX_ALBUMS_MODEL, NULL);
}

void Overview::PageLoaded() {
  // Set these context properties now, as they won't change during the
  // lifetime of the app
  SetContextProperty(CTX_MEDIA_MODEL, media_model_);
  SetContextProperty(CTX_ALBUMS_MODEL, albums_model_);
  SetContextProperty(CTX_ALBUM_PICKER_MODEL, albums_model_);

  //
  // Overview containing pane
  //
  
  Connect("overview", SIGNAL(create_album()), this, SIGNAL(create_album()));

  Connect("overview", SIGNAL(create_album_from_selected()), this,
    SLOT(on_create_album_from_selected_photos()));

  Connect("overview", SIGNAL(popupAlbumPicked(int)), this,
    SLOT(on_popup_album_picked(int)));
  
  //
  // Photos checkerboard
  //
  
  Connect("photos_checkerboard", SIGNAL(activated(int)), this,
    SLOT(on_photo_activated(int)));
  
  Connect("photos_checkerboard", SIGNAL(selection_toggled(int)), this,
    SLOT(on_photo_selection_toggled(int)));
  
  Connect("photos_checkerboard", SIGNAL(unselect_all()), this,
    SLOT(on_photos_unselect_all()));
  
  //
  // Albums checkerboard
  //
  
  Connect("albums_checkerboard", SIGNAL(activated(int)), this,
    SLOT(on_album_activated(int)));
}

void Overview::PrepareToEnter() {
}

void Overview::on_popup_album_picked(int album_number) {
  Album* album = qobject_cast<Album*>(albums_view_.FindByNumber(album_number));

  album->AttachMany(media_view_.GetSelected());
}

void Overview::on_photo_activated(int media_number) {
  MediaSource* media_object = qobject_cast<MediaSource*>(
    media_view_.FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    emit photo_activated(media_object);
  else
    qDebug("Unable to locate activated photo #%d", media_number);
}

void Overview::on_photo_selection_toggled(int media_number) {
  MediaSource* media_object = qobject_cast<MediaSource*>(
    media_view_.FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    media_view_.ToggleSelect(media_object);
  else
    qDebug("Unable to located toggled photo #%d", media_number);
}

void Overview::on_photos_unselect_all() {
  media_view_.UnselectAll();
}

void Overview::on_create_album_from_selected_photos() {
  if (media_view_.GetSelectedCount() == 0)
    return;
  
  Album *album = new Album(*DefaultAlbumTemplate::instance());
  album->AttachMany(media_view_.GetSelected());
  
  AlbumCollection::instance()->Add(album);
}

void Overview::on_album_activated(int album_number) {
  Album* album = qobject_cast<Album*>(albums_view_.FindByNumber(album_number));
  if (album != NULL)
    emit album_activated(album);
  else
    qDebug("Unable to located activated album #%d", album_number);
}
