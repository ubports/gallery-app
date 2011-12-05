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

#include "photo.h"

#include <QFileInfo>
#include <QImage>

Photo::Photo(const QFileInfo& file)
  : MediaSource(file) {
}

bool Photo::MakePreview(const QFileInfo& original, const QFileInfo &dest) {
  QImage fullsized = QImage(original.filePath());
  if (fullsized.isNull())
    qFatal("Unable to load %s", qPrintable(original.filePath()));
  
  QImage scaled;
  if (fullsized.height() > fullsized.width())
    scaled = fullsized.scaledToHeight(156, Qt::SmoothTransformation);
  else
    scaled = fullsized.scaledToWidth(156, Qt::SmoothTransformation);

  if (scaled.isNull())
    qFatal("Unable to scale %s", qPrintable(original.filePath()));

  if (!scaled.save(dest.filePath()))
    qFatal("Unable to save %s", qPrintable(dest.filePath()));
  
  return true;
}

void Photo::DestroySource(bool destroy_backing) {
  // TODO: destroy the backing photo file and database entry
}