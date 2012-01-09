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

#include "qml/qml-media-selector-page.h"

#include "album/album-collection.h"
#include "album/album-default-template.h"

static const char* CTX_MEDIA_SELECTOR_MODEL = "ctx_media_selector_model";

QmlMediaSelectorPage::QmlMediaSelectorPage(QDeclarativeView* view)
  : QmlPage(view, "mediaSelector"), selected_("QmlMediaSelectorPage"),
  album_(NULL), view_(NULL), qml_media_model_(NULL) {
}

QmlMediaSelectorPage::~QmlMediaSelectorPage() {
  delete qml_media_model_;
  delete view_;
}

const char* QmlMediaSelectorPage::qml_rc() const {
  return "qrc:/rc/qml/MediaSelector.qml";
}

void QmlMediaSelectorPage::PrepareContext() {
  SetContextProperty(CTX_MEDIA_SELECTOR_MODEL, NULL);
}

void QmlMediaSelectorPage::PageLoaded() {
  Connect("mediaSelector", SIGNAL(addSelected()), this, SLOT(OnAddSelected()));
  Connect("mediaSelector", SIGNAL(finished()), this, SLOT(OnFinished()));
  Connect("mediaCheckerboard", SIGNAL(selection_toggled(int)), this,
    SLOT(OnSelectionToggled(int)));
  Connect("mediaCheckerboard", SIGNAL(unselect_all()), this, SLOT(OnUnselectAll()));
}

void QmlMediaSelectorPage::PrepareToEnter(ViewCollection* view, Album* album) {
  album_ = album;
  
  delete view_;
  view_ = new SelectableViewCollection("View for MediaSelectorPage");
  view_->MonitorDataCollection(view, NULL, false);
  
  delete qml_media_model_;
  qml_media_model_ = new QmlMediaModel();
  qml_media_model_->Init(view_);
  
  ClearProperty("mediaCheckerboard", "checkerboardModel");
  SetContextProperty(CTX_MEDIA_SELECTOR_MODEL, qml_media_model_);
}

void QmlMediaSelectorPage::OnAddSelected() {
  selected_.AddMany(view_->GetSelected());
  view_->UnselectAll();
}

void QmlMediaSelectorPage::OnFinished() {
  if (selected_.Count() > 0) {
    Album* to_populate = album_;
    if (to_populate == NULL)
      to_populate = new Album(*AlbumDefaultTemplate::instance());
    
    to_populate->AttachMany(selected_.GetAsSet());
    selected_.Clear();
    
      // only add to AlbumCollection if a new instance
    if (album_ == NULL)
      AlbumCollection::instance()->Add(to_populate);
  }
  
  emit finished();
}

void QmlMediaSelectorPage::OnSelectionToggled(int media_number) {
  MediaSource* media_object = qobject_cast<MediaSource*>(
    view_->FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    view_->ToggleSelect(media_object);
  else
    qDebug("Unable to located toggled photo #%d", media_number);
}

void QmlMediaSelectorPage::OnUnselectAll() {
  view_->UnselectAll();
}
