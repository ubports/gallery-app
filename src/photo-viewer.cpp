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

PhotoViewer::PhotoViewer(QDeclarativeView* view)
  : QmlPage(view) {
}

PhotoViewer::~PhotoViewer() {
}

const char* PhotoViewer::qml_rc() const {
  return "qrc:/rc/qml/PhotoViewer.qml";
}

void PhotoViewer::Prepare(QmlMediaModel *model, Photo* start) {
  SetContextProperty("viewer_model", model);
  SetContextProperty("viewer_current_index",
    model->BackingViewCollection()->IndexOf(start));
}

void PhotoViewer::SwitchingTo() {
  Connect("photo_viewer", SIGNAL(exit_viewer()), this, SIGNAL(exit_viewer()));
}

void PhotoViewer::SwitchingFrom() {
}
