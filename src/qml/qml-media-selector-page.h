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

#ifndef GALLERY_QML_MEDIA_SELECTOR_PAGE_H_
#define GALLERY_QML_MEDIA_SELECTOR_PAGE_H_

#include <QObject>
#include <QDeclarativeView>

#include "album/album.h"
#include "core/selectable-view-collection.h"
#include "core/view-collection.h"
#include "qml/qml-media-collection-model.h"
#include "qml/qml-page.h"

class QmlMediaSelectorPage : public QmlPage {
  Q_OBJECT
  
 signals:
  void finished();
  
 public:
  QmlMediaSelectorPage(QDeclarativeView* view);
  ~QmlMediaSelectorPage();
  
  virtual const char* qml_rc() const;
  
  virtual void PrepareContext();
  virtual void PageLoaded();
  
  // Use to select media from the view to add to the album; if album is NULL,
  // a new Album will be created when the user clicks the "done" button
  void PrepareToEnter(ViewCollection* view, Album* album);
  
 private slots:
  void OnAddSelected();
  void OnFinished();
  void OnSelectionToggled(int media_number);
  void OnUnselectAll();
  
 private:
  ViewCollection selected_;
  Album* album_;
  SelectableViewCollection* view_;
  QmlMediaCollectionModel* qml_media_collection_model_;
};

#endif  // GALLERY_QML_MEDIA_SELECTOR_PAGE_H_
