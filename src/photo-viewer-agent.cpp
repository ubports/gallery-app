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

#include "photo-viewer-agent.h"

#include <assert.h>

#include <QObject>
#include <QString>
#include <QFile>
#include <QDeclarativeView>
#include <QDeclarativeItem>

PhotoViewerAgent::PhotoViewerAgent(QDeclarativeView* view) {
  QDeclarativeItem* root =
    qobject_cast<QDeclarativeItem*>(view->rootObject());
  assert(root != NULL);
  
  QObject* viewer = root->findChild<QObject*>("photo_viewer");
  assert(viewer != NULL);
  
  QObject::connect(viewer, SIGNAL(exit_viewer()), this,
    SIGNAL(exit_pressed()));
}
