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

#include "overview-agent.h"

#include <QDeclarativeView>
#include <QDeclarativeItem>

#include "qml-media-model.h"

OverviewAgent::OverviewAgent(QDeclarativeView* view) {
  // TODO: Clean this up
  QDeclarativeItem* item =
    qobject_cast<QDeclarativeItem*>(view->rootObject());
  Q_ASSERT(item != NULL);
  
  QObject* loader = item->findChild<QObject*>("loader");
  Q_ASSERT(loader != NULL);
  
  QObject* overview = item->findChild<QObject*>("overview");
  Q_ASSERT(overview != NULL);
  
  QObject::connect(overview, SIGNAL(create_album_from_selected()), this,
    SIGNAL(create_album_from_selected_photos()));
  
  QObject* photos_checkerboard = loader->findChild<QObject*>("photos_checkerboard");
  Q_ASSERT(photos_checkerboard != NULL);
  
  QObject::connect(photos_checkerboard, SIGNAL(activated(int)), this,
    SIGNAL(photo_activated(int)));
  
  QObject::connect(photos_checkerboard, SIGNAL(selection_toggled(int)), this,
    SIGNAL(photo_selection_toggled(int)));
  
  QObject::connect(photos_checkerboard, SIGNAL(unselect_all()), this,
    SIGNAL(photos_unselect_all()));
}
