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

#ifndef GALLERY_PHOTO_VIEWER_AGENT_H_
#define GALLERY_PHOTO_VIEWER_AGENT_H_

#include <QObject>
#include <QString>
#include <QFile>
#include <QPointer>
#include <QDeclarativeView>

class PhotoViewerAgent : public QObject {
  Q_OBJECT

signals:
  void exit_pressed();

public:
  explicit PhotoViewerAgent(QDeclarativeView* view);
};

#endif  // GALLERY_PHOTO_VIEWER_AGENT_H_
