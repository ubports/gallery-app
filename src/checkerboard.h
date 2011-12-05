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

#ifndef GALLERY_CHECKERBOARD_H_
#define GALLERY_CHECKERBOARD_H_

#include <QObject>
#include <QDeclarativeView>
#include <QString>

#include "media-source.h"
#include "media-collection.h"
#include "checkerboard-agent.h"
#include "qml-media-model.h"
#include "selectable-view-collection.h"

class Checkerboard : public QObject {
  Q_OBJECT
  
signals:
  void activated(MediaSource* media_source);
  
public:
  Checkerboard(MediaCollection* media, SourceFilter filter);
  virtual ~Checkerboard();
  
  QmlMediaModel* model() const;
  
  // returned path is a relative path, not an absolute one
  const char* qml_file_path() const;
  
  void Prepare(QDeclarativeView* view);
  void SwitchingTo(QDeclarativeView* view);
  void SwitchingFrom(QDeclarativeView* view);
  
private slots:
  void on_activated(int);
  void on_selection_toggled(int);
  void on_unselect_all();
  void on_create_album_from_selected();
  
private:
  SelectableViewCollection view_;
  CheckerboardAgent* agent_;
  QmlMediaModel* model_;
};

#endif  // GALLERY_CHECKERBOARD_H_
