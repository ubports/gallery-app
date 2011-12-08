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
  
  // scale the preview so it will fill the viewport specified by PREVIEW_*_MAX
  // these values are replicated in the QML so that the preview will fill each
  // grid cell, cropping down to the center of the image if necessary
  QImage scaled;
  if (fullsized.height() > fullsized.width())
    scaled = fullsized.scaledToWidth(PREVIEW_WIDTH_MAX, Qt::SmoothTransformation);
  else
    scaled = fullsized.scaledToHeight(PREVIEW_HEIGHT_MAX, Qt::SmoothTransformation);
  
  if (scaled.isNull())
    qFatal("Unable to scale %s", qPrintable(original.filePath()));
  
  if (!scaled.save(dest.filePath()))
    qFatal("Unable to save %s", qPrintable(dest.filePath()));
  
  return true;
}

void Photo::DestroySource(bool destroy_backing) {
  // TODO: destroy the backing photo file and database entry
}