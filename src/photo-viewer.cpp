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

#include "photo-viewer.h"

#include <QMetaObject>

#include "album-collection.h"
#include "album-picker.h"
#include "default-album-template.h"

static const char* CTX_PHOTO_MODEL = "ctx_photo_viewer_photo_model";

PhotoViewer::PhotoViewer(QDeclarativeView* view)
  : QmlPage(view, "photo_viewer"), album_view_collection("PhotoViewer Album ViewCollection"),
  media_model_(NULL) {
  album_view_collection.MonitorDataCollection(AlbumCollection::instance(),
    NULL, false);
  album_picker_model_.Init(&album_view_collection);
}

const char* PhotoViewer::qml_rc() const {
  return "qrc:/rc/qml/PhotoViewer.qml";
}

void PhotoViewer::PrepareContext() {
  SetContextProperty(CTX_PHOTO_MODEL, NULL);
  SetContextProperty("ctx_album_picker_model",
    AlbumPicker::instance()->universal_albums_model());
}

void PhotoViewer::PageLoaded() {
  Connect("photo_viewer", SIGNAL(exit_viewer()), this,
    SIGNAL(exit_viewer()));
  Connect("photo_viewer", SIGNAL(popupAlbumPicked(int)), this,
    SLOT(on_popup_album_picked(int)));
  Connect("photo_viewer", SIGNAL(newAlbumRequested()), this,
          SLOT(on_new_album_requested()));
  
  ClearProperty("album_picker", "designated_model");
  SetContextProperty("ctx_album_picker_model", &album_picker_model_);
}

void PhotoViewer::PrepareToEnter(QmlMediaModel *model, Photo* start) {
  media_model_ = model;
  
  // Clear item properties before setting context properties ... apparently
  // changes to context properties do not properly propagate to their bound
  // item properties
  ClearProperty("image_pager", "model");
  SetContextProperty(CTX_PHOTO_MODEL, model);
  
  // don't use ListView's currentIndex property, as that will animate the
  // ListView as it magically scrolls through the list to the photo; rather,
  // call positionViewAtIndex() for an immediate jump
  QDeclarativeItem* list_view = FindChild("image_pager");
  QMetaObject::invokeMethod(list_view, "positionViewAtIndex",
    Q_ARG(int, model->BackingViewCollection()->IndexOf(start)),
    Q_ARG(int, 0));
}

void PhotoViewer::on_popup_album_picked(int album_number) {
  int visible_index = GetProperty("image_pager", "currentIndex").toInt();
  
  Photo* photo = qobject_cast<Photo*>(
    media_model_->BackingViewCollection()->GetAt(visible_index));
  Album* album = AlbumPicker::instance()->GetAlbumForIndex(album_number);
  
  album->Attach(photo);
}

void PhotoViewer::on_new_album_requested() {
  Photo* photo =
    qobject_cast<Photo*>(media_model_->BackingViewCollection()->GetAt(
    GetProperty("image_pager", "currentIndex").toInt()));

  Album* album = new Album(*DefaultAlbumTemplate::instance());
  album->Attach(photo);

  AlbumCollection::instance()->Add(album);
}
