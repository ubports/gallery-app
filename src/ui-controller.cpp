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

#include "ui-controller.h"

#include <cstdlib>

#include <QObject>
#include <QGLWidget>
#include <QDeclarativeItem>
#include <QDir>
#include <QString>
#include <QUrl>

UIController::UIController(const QDir &path)
  : current_page_(NULL) {
  MediaCollection::InitInstance(path);
  
  //
  // Create the master QDeclarativeView that all the pages will operate within
  //
  
  // Enable OpenGL backing
  QGLFormat format = QGLFormat::defaultFormat();
  format.setSampleBuffers(false);
  QGLWidget *gl_widget = new QGLWidget(format);
  
  view_ = new QDeclarativeView();
  
  //
  // Initialize all the pages
  //
  
  overview_ = new Overview(view_);
  overview_->PrepareContext();
  
  photo_viewer_ = new PhotoViewer(view_);
  photo_viewer_->PrepareContext();
  
  album_viewer_ = new AlbumViewer(view_);
  album_viewer_->PrepareContext();
  
  media_selector_ = new QmlMediaSelectorPage(view_);
  media_selector_->PrepareContext();
  
  //
  // Load the root container with context prepped and allow pages to now
  // access page properties
  //
  
  view_->setSource(QUrl("qrc:/rc/qml/TabletSurface.qml"));
  view_->setViewport(gl_widget);
  
  overview_->PageLoaded();
  photo_viewer_->PageLoaded();
  album_viewer_->PageLoaded();
  media_selector_->PageLoaded();
  
  //
  // Tablet Surface (root container)
  //
  
  tablet_surface_ = qobject_cast<QObject*>(view_->rootObject());
  Q_ASSERT(!tablet_surface_.isNull() &&
    tablet_surface_->objectName() == "tablet_surface");
  
  QObject::connect(tablet_surface_, SIGNAL(power_off()), this,
    SLOT(on_power_off()));
  
  //
  // Overview (Photos / Albums)
  
  QObject::connect(overview_, SIGNAL(photo_activated(MediaSource*)), this,
    SLOT(on_overview_media_activated(MediaSource*)));
  
  QObject::connect(overview_, SIGNAL(album_activated(Album*)), this,
    SLOT(on_album_activated(Album*)));
  
  QObject::connect(overview_, SIGNAL(create_album()), this,
    SLOT(on_create_album()));
  
  //
  // PhotoViewer
  //
  
  QObject::connect(photo_viewer_, SIGNAL(exit_viewer()), this,
    SLOT(on_photo_viewer_exited()));
  
  //
  // AlbumViewer
  //
  
  QObject::connect(album_viewer_, SIGNAL(exit_viewer()), this,
    SLOT(on_exit_album_viewer()));
  
  QObject::connect(album_viewer_, SIGNAL(media_activated(MediaSource*)), this,
    SLOT(on_album_media_activated(MediaSource*)));
  
  //
  // QmlMediaSelectorPage
  //
  
  QObject::connect(media_selector_, SIGNAL(finished()), this,
    SLOT(on_media_selector_finished()));
  
  // start with Overview
  overview_->PrepareToEnter();
  SwitchTo(overview_);
}

UIController::~UIController() {
  delete overview_;
  delete photo_viewer_;
  delete album_viewer_;
}

void UIController::on_overview_media_activated(MediaSource* media_source) {
  ActivateMedia(overview_->media_model(), media_source);
}

void UIController::on_album_media_activated(MediaSource* media_source) {
  ActivateMedia(album_viewer_->media_model(), media_source);
}

void UIController::on_create_album() {
  media_selector_->PrepareToEnter(overview_->media_model()->BackingViewCollection(),
    NULL);
  SwitchTo(media_selector_);
}

void UIController::on_power_off() {
  std::exit(0);
}

void UIController::on_album_activated(Album* album) {
  album_viewer_->PrepareToEnter(album);
  SwitchTo(album_viewer_);
}

void UIController::on_photo_viewer_exited() {
  GoBack();
}

void UIController::on_media_selector_finished() {
  GoBack();
}

void UIController::on_exit_album_viewer() {
  GoBack();
}

void UIController::ShowHide(QmlPage* page, bool show) {
  QObject* root = tablet_surface_->findChild<QObject*>(page->page_root_name());
  Q_ASSERT(root != NULL);
  
  root->setProperty("visible", show);
}

// The caller should always call qml_page->Prepare(...) prior to calling this
// method ... since each Prepare is parameterized, can't include it here
void UIController::SwitchTo(QmlPage* qml_page) {
  if (current_page_ == qml_page)
    return;
  
  // give current page a chance to clean up and push onto navigation stack
  if (current_page_ != NULL) {
    current_page_->Leaving();
    navigation_stack_.push(current_page_);
    ShowHide(current_page_, false);
  }
  
  current_page_ = qml_page;
  
  // Show the page and inform that the user is entering it (moving forward
  // into it)
  ShowHide(current_page_, true);
  current_page_->Entered();
  
  view_->show();
}

void UIController::GoBack() {
  if (navigation_stack_.isEmpty())
    return;
  
  QmlPage* previous_page = navigation_stack_.pop();
  Q_ASSERT(previous_page != NULL);
  Q_ASSERT(previous_page != current_page_);
  
  // give current page a chance to clean up
  if (current_page_ != NULL) {
    current_page_->Leaving();
    ShowHide(current_page_, false);
  }
  
  current_page_ = previous_page;
  
  // Show the page and inform that the user has returned to (gone back to) it
  ShowHide(current_page_, true);
  current_page_->ReturnedTo();
  
  view_->show();
}

// Currently this only handles Photo objects; when Video objects are added,
// it should launch the video player
void UIController::ActivateMedia(QmlMediaModel* model, MediaSource* media_source) {
  Photo* photo = qobject_cast<Photo*>(media_source);
  if (photo == NULL) {
    qDebug("Non-photo object activated");
    
    return;
  }
  
  photo_viewer_->PrepareToEnter(model, photo);
  SwitchTo(photo_viewer_);
}
