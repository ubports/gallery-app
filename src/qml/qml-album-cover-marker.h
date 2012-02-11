/*
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef GALLERY_QML_ALBUM_COVER_MARKER_H_
#define GALLERY_QML_ALBUM_COVER_MARKER_H_

#include <QObject>
#include <QtDeclarative>

#include "core/data-source.h"

class QmlAlbumCoverMarker : public DataSource {
  Q_OBJECT
  
 public:
  QmlAlbumCoverMarker();
  
  static void RegisterType();
  
 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
};

QML_DECLARE_TYPE(QmlAlbumCoverMarker)

#endif  // GALLERY_QML_ALBUM_COVER_MARKER_H_
