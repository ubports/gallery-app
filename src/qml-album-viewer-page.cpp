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

#include "qml-album-viewer-page.h"

#include "album-picker.h"
#include "album-collection.h"
#include "default-album-template.h"
#include "qml-album-viewer-page.h"

#include <QVariant>

static const char* CONTEXT_ALBUM_MODEL = "ctx_album_viewer_album_model";
static const char* CONTEXT_MEDIA_MODEL = "ctx_album_viewer_media_model";
static const char* CONTEXT_ALBUM_PICKER_MODEL = "ctx_album_picker_model";

QmlAlbumViewerPage::QmlAlbumViewerPage(QDeclarativeView* view)
  : QmlPage(view, "album_viewer"), album_model_(NULL), media_model_(NULL),
  view_(NULL) {
}

QmlAlbumViewerPage::~QmlAlbumViewerPage() {
  delete media_model_;
  delete album_model_;
  delete view_;
}

const char* QmlAlbumViewerPage::qml_rc() const {
  return "qrc:/rc/qml/AlbumViewer.qml";
}

void QmlAlbumViewerPage::PrepareContext() {
  SetContextProperty(CONTEXT_ALBUM_MODEL, NULL);
  SetContextProperty(CONTEXT_MEDIA_MODEL, NULL);
  SetContextProperty(CONTEXT_ALBUM_PICKER_MODEL, NULL);
}

void QmlAlbumViewerPage::PageLoaded() {
  Connect("album_viewer", SIGNAL(exit_viewer()), this, SIGNAL(exit_viewer()));
  Connect("grid_checkerboard", SIGNAL(activated(int)), this,
    SLOT(on_media_activated(int)));
  Connect("album_viewer", SIGNAL(addToAlbum()), this, SLOT(on_add_to_album()));
  Connect("grid_checkerboard", SIGNAL(selection_toggled(int)), this,
    SLOT(on_media_selection_toggled(int)));
  Connect("grid_checkerboard", SIGNAL(unselect_all()), this,
    SLOT(on_media_unselect_all()));
  Connect("album_viewer", SIGNAL(popupAlbumPicked(int)), this,
    SLOT(on_popup_album_picked(int)));
  Connect("album_viewer", SIGNAL(createAlbumFromSelected()), this,
    SLOT(on_create_album_from_selected_media()));
}

void QmlAlbumViewerPage::PrepareToEnter(Album* album) {
  album_ = album;
  
  delete view_;
  view_ = new SelectableViewCollection(
    QString("SelectableViewCollection for ") + QString(album->ToString()));
  view_->MonitorDataCollection(album->contained(), NULL, false);
  
  delete album_model_;
  album_model_ = new QmlAlbumModel(NULL);
  album_model_->Init(album);
  
  delete media_model_;
  media_model_ = new QmlMediaModel(NULL);
  media_model_->Init(view_);
  
  // Clear item properties before setting context properties ... apparently
  // changes to context properties do not properly propagate to their bound
  // item properties
  ClearProperty("grid_checkerboard", "checkerboardModel");
  ClearProperty("template_pager", "model");
  ClearProperty("album_picker", "designated_model");
  
  SetContextProperty(CONTEXT_ALBUM_MODEL, album_model_);
  SetContextProperty(CONTEXT_MEDIA_MODEL, media_model_);
  SetContextProperty(CONTEXT_ALBUM_PICKER_MODEL,
    AlbumPicker::instance()->universal_albums_model());

  
  // don't use ListView's currentIndex property, as that will animate the
  // ListView as it magically scrolls through the list to the photo; rather,
  // call positionViewAtIndex() for an immediate jump
  QDeclarativeItem* list_view = FindChild("template_pager");
  QMetaObject::invokeMethod(list_view, "positionViewAtIndex",
    Q_ARG(int, album->current_page()), Q_ARG(int, 0));
}

QmlMediaModel* QmlAlbumViewerPage::media_model() const {
  return media_model_;
}

void QmlAlbumViewerPage::on_media_activated(int media_number) {
  MediaSource* media = qobject_cast<MediaSource*>(view_->FindByNumber(media_number));
  if (media == NULL) {
    qDebug("Unable to find media source for #%d", media_number);
    
    return;
  }
  
  emit media_activated(media);
}

void QmlAlbumViewerPage::on_media_selection_toggled(int media_number) {
  MediaSource* media_object = qobject_cast<MediaSource*>(
    view_->FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    view_->ToggleSelect(media_object);
  else
    qDebug("Unable to located toggled photo #%d", media_number);
}

void QmlAlbumViewerPage::on_media_unselect_all() {
  view_->UnselectAll();
}

void QmlAlbumViewerPage::on_popup_album_picked(int album_number) {
  AlbumPicker::instance()->GetAlbumForIndex(album_number)->AttachMany(
    view_->GetSelected());
}

void QmlAlbumViewerPage::on_create_album_from_selected_media() {
  if (view_->GetSelectedCount() == 0)
    return;

  Album *album = new Album(*DefaultAlbumTemplate::instance());
  album->AttachMany(view_->GetSelected());

  AlbumCollection::instance()->Add(album);
}

void QmlAlbumViewerPage::on_add_to_album() {
  emit add_media_to_album(album_);
}
