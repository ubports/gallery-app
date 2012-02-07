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

#include "media/preview-manager.h"

#include "media/media-collection.h"

const QString PreviewManager::PREVIEW_DIR = "thumbs";

PreviewManager* PreviewManager::instance_ = NULL;

PreviewManager::PreviewManager() {
  // Monitor MediaCollection for all new MediaSources
  QObject::connect(MediaCollection::instance(),
  SIGNAL(contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
  this,
  SLOT(on_media_added_removed(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  // Verify previews for all existing added MediaSources
  on_media_added_removed(&MediaCollection::instance()->GetAsSet(), NULL);
  
  // TODO: Subscribe to destroyed signal to remove cached previews
}

void PreviewManager::Init() {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new PreviewManager();
}

void PreviewManager::Terminate() {
  delete instance_;
  instance_ = NULL;
}

PreviewManager* PreviewManager::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

void PreviewManager::on_media_added_removed(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added)
      VerifyPreview(qobject_cast<MediaSource*>(object));
  }
}

QFileInfo PreviewManager::PreviewFileFor(const MediaSource* media) const {
  QFileInfo file = media->file();
  
  return QFileInfo(file.dir(),
    PREVIEW_DIR + "/" + file.completeBaseName() + "_th." + file.completeSuffix());
}

bool PreviewManager::VerifyPreview(MediaSource* media) {
  // create the thumbnail directory if not already present
  media->file().dir().mkdir(PREVIEW_DIR);
  
  // If preview file exists, considered valid
  QFileInfo preview = PreviewFileFor(media);
  if (preview.exists())
    return true;
  
  QImage fullsized = media->Image();
  if (fullsized.isNull()) {
    qDebug("Unable to generate fullsized image for %s, not generating preview",
      media->ToString());
    
    return false;
  }
  
  // scale the preview so it will fill the viewport specified by PREVIEW_*_MAX
  // these values are replicated in the QML so that the preview will fill each
  // grid cell, cropping down to the center of the image if necessary
  QImage scaled = (fullsized.height() > fullsized.width())
    ? fullsized.scaledToWidth(PREVIEW_WIDTH_MAX, Qt::SmoothTransformation)
    : fullsized.scaledToHeight(PREVIEW_HEIGHT_MAX, Qt::SmoothTransformation);
  
  if (scaled.isNull()) {
    qDebug("Unable to scale %s for preview", media->ToString());
    
    return false;
  }
  
  if (!scaled.save(preview.filePath())) {
    qDebug("Unable to save preview %s", qPrintable(preview.filePath()));
    
    return false;
  }
  
  return true;
}
