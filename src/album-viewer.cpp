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

#include "album-viewer.h"

#include <QVariant>

static const char* CONTEXT_ALBUM_MODEL = "ctx_album_viewer_album_model";
static const char* CONTEXT_MEDIA_MODEL = "ctx_album_viewer_media_model";

AlbumViewer::AlbumViewer(QDeclarativeView* view)
  : QmlPage(view, "album_viewer"), album_model_(NULL), media_model_(NULL),
  view_(NULL) {
}

AlbumViewer::~AlbumViewer() {
  delete media_model_;
  delete album_model_;
  delete view_;
}

const char* AlbumViewer::qml_rc() const {
  return "qrc:/rc/qml/AlbumViewer.qml";
}

void AlbumViewer::PrepareContext() {
  SetContextProperty(CONTEXT_ALBUM_MODEL, NULL);
  SetContextProperty(CONTEXT_MEDIA_MODEL, NULL);
}

void AlbumViewer::PageLoaded() {
  Connect("album_viewer", SIGNAL(exit_viewer()), this, SIGNAL(exit_viewer()));
  Connect("grid_checkerboard", SIGNAL(activated(int)), this,
    SLOT(on_media_activated(int)));
}

void AlbumViewer::PrepareToEnter(Album* album) {
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
  
  SetContextProperty(CONTEXT_ALBUM_MODEL, album_model_);
  SetContextProperty(CONTEXT_MEDIA_MODEL, media_model_);
  
  // don't use ListView's currentIndex property, as that will animate the
  // ListView as it magically scrolls through the list to the photo; rather,
  // call positionViewAtIndex() for an immediate jump
  QDeclarativeItem* list_view = FindChild("template_pager");
  QMetaObject::invokeMethod(list_view, "positionViewAtIndex",
    Q_ARG(int, album->current_page()), Q_ARG(int, 0));
}

QmlMediaModel* AlbumViewer::media_model() const {
  return media_model_;
}

void AlbumViewer::on_media_activated(int media_number) {
  MediaSource* media = qobject_cast<MediaSource*>(view_->FindByNumber(media_number));
  if (media == NULL) {
    qDebug("Unable to find media source for #%d", media_number);
    
    return;
  }
  
  emit media_activated(media);
}
