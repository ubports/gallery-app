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

#include "checkerboard-agent.h"

#include <QDeclarativeView>
#include <QDeclarativeItem>

#include "qml-media-model.h"

CheckerboardAgent::CheckerboardAgent(QDeclarativeView* view) {
  // TODO: Clean this up
  QDeclarativeItem* item =
    qobject_cast<QDeclarativeItem*>(view->rootObject());
  Q_ASSERT(item != NULL);
  
  QObject* loader = item->findChild<QObject*>("loader");
  Q_ASSERT(loader != NULL);
  
  QObject* checkerboard = loader->findChild<QObject*>("checkerboard");
  Q_ASSERT(checkerboard != NULL);
  
  QObject::connect(checkerboard, SIGNAL(activated(int)), this,
    SIGNAL(activated(int)));
  
  QObject::connect(checkerboard, SIGNAL(selection_toggled(int)), this,
    SIGNAL(selection_toggled(int)));
  
  QObject::connect(checkerboard, SIGNAL(unselect_all()), this,
    SIGNAL(unselect_all()));
  
  QObject::connect(checkerboard, SIGNAL(create_album_from_selected()), this,
    SIGNAL(create_album_from_selected()));
  
  grid_ = checkerboard->findChild<QObject*>("grid");
  Q_ASSERT(grid_ != NULL);
}
