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

#include <QApplication>

const QString PreviewManager::PREVIEW_DIR = ".thumbs";

const int PreviewManager::PREVIEW_WIDTH_MAX = 360;
const int PreviewManager::PREVIEW_HEIGHT_MAX = 360;
// FIXME adapt to different sizes. This is fixed size for the demo device
const int PreviewManager::THUMBNAIL_SIZE = 216;
const int PreviewManager::PREVIEW_QUALITY = 70;
const char* PreviewManager::PREVIEW_FILE_FORMAT = "JPEG";
const char* PreviewManager::PREVIEW_FILE_EXT = "JPG";

PreviewManager* PreviewManager::instance_ = NULL;

PreviewManager::PreviewManager() {
  // Monitor MediaCollection for all new MediaSources
  QObject::connect(MediaCollection::instance(),
  SIGNAL(contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
  this,
  SLOT(on_media_added_removed(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  QObject::connect(MediaCollection::instance(),
  SIGNAL(destroying(const QSet<DataObject*>*)),
  this,
  SLOT(on_media_destroying(const QSet<DataObject*>*)));

  // Verify previews for all existing added MediaSources
  on_media_added_removed(&MediaCollection::instance()->GetAsSet(), NULL);
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
    foreach (object, *added) {
      MediaSource* source = qobject_cast<MediaSource*>(object);

      QObject::connect(source, SIGNAL(data_altered()),
        this, SLOT(on_media_data_altered()), Qt::UniqueConnection);

      ensure_preview_for_media(source);
    }
  }

  if (removed != NULL) {
    DataObject* object;
    foreach (object, *removed) {
      MediaSource* source = qobject_cast<MediaSource*>(object);
      QObject::disconnect(source, SIGNAL(data_altered()),
        this, SLOT(on_media_data_altered()));
    }
  }
}

void PreviewManager::on_media_destroying(const QSet<DataObject*>* destroying) {
  if (destroying != NULL) {
    DataObject* object;
    foreach (object, *destroying)
      DestroyPreview(qobject_cast<MediaSource*>(object));
  }
}

void PreviewManager::on_media_data_altered() {
  QObject* object = QObject::sender();
  MediaSource* source = qobject_cast<MediaSource*>(object);

  ensure_preview_for_media(source, true);
}

QFileInfo PreviewManager::PreviewFileFor(const MediaSource* media) const {
  QFileInfo file = media->file();
  
  return QFileInfo(file.dir(),
    PREVIEW_DIR + "/" + file.completeBaseName() + "_th." + PREVIEW_FILE_EXT);
}

QFileInfo PreviewManager::ThumbnailFileFor(const MediaSource *media) const {
  QFileInfo file = media->file();
  return QFileInfo(file.dir(),
    PREVIEW_DIR + "/" + file.completeBaseName() + "_th_s." + PREVIEW_FILE_EXT);
}

bool PreviewManager::ensure_preview_for_media(MediaSource* media, bool regen) {
  // create the thumbnail directory if not already present
  media->file().dir().mkdir(PREVIEW_DIR);
  
  // If preview file exists, considered valid (unless we're regenerating it).
  QFileInfo preview = PreviewFileFor(media);
  QFileInfo thumbnail = ThumbnailFileFor(media);

  QImage thumbMaster;
  if (!preview.exists() || regen) {
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
  
    if (!scaled.save(preview.filePath(), PREVIEW_FILE_FORMAT, PREVIEW_QUALITY)) {
      qDebug("Unable to save preview %s", qPrintable(preview.filePath()));
      return false;
    }
    thumbMaster = scaled;
  }

  if (!thumbnail.exists() || regen) {
    if (thumbMaster.isNull()) {
      thumbMaster.load(preview.filePath());
      if (thumbMaster.isNull()) {
        qDebug("Unable load preview image for %s, not generating thumbnail",
          qPrintable(preview.filePath()));
        return false;
      }
    }

    QImage square = generate_Thumbnail(thumbMaster);
    if (!square.save(thumbnail.filePath(), PREVIEW_FILE_FORMAT, PREVIEW_QUALITY)) {
      qDebug("Unable to save preview %s", qPrintable(thumbnail.filePath()));
      return false;
    }
  }

  // Spin the event loop so we don't hang the app if we do lots of this.
  QApplication::processEvents();

  return true;
}

void PreviewManager::DestroyPreview(MediaSource* media) {
  QString filename = PreviewFileFor(media).filePath();
  if (!QFile::remove(filename))
    qDebug("Unable to remove preview %s", qPrintable(filename));
  filename = ThumbnailFileFor(media).filePath();
  if (!QFile::remove(filename))
    qDebug("Unable to remove thumbnail %s", qPrintable(filename));
}

QImage PreviewManager::generate_Thumbnail(const QImage &master) const {
  int xOffset = 0;
  int yOffset = 0;
  int size = 0;
  if (master.width() > master.height()) {
      size = master.height();
      xOffset = (master.width() - size) / 2;
  } else {
      size = master.width();
      yOffset = (master.height() - size) / 2;
  }
  QImage square = master.copy(xOffset, yOffset, size, size);

  QImage thumbnail = square.scaled(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
  return thumbnail;
}
