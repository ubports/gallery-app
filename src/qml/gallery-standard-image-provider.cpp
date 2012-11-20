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
 * Lucas Beeler <lucas@yorba.org>
 * Jim Nelson <jim@yorba.org>
 */

#include "qml/gallery-standard-image-provider.h"

#include <QImageReader>
#include <QSize>

#include "photo/photo-metadata.h"
#include "media/preview-manager.h"

// set to 1 to log image cache and loading status via qDebug
#define GALLERY_LOG_IMAGE 0

GalleryStandardImageProvider* GalleryStandardImageProvider::instance_ = NULL;

const char* GalleryStandardImageProvider::PROVIDER_ID = "gallery-standard";
const char* GalleryStandardImageProvider::PROVIDER_ID_SCHEME = "image://gallery-standard/";

const long MAX_CACHE_BYTES = 80L * 1024L * 1024L;

// fully load previews into memory when requested
const int SCALED_LOAD_FLOOR_DIM_PIXELS =
  qMax(PreviewManager::PREVIEW_WIDTH_MAX, PreviewManager::PREVIEW_HEIGHT_MAX);

GalleryStandardImageProvider::GalleryStandardImageProvider()
  : QQuickImageProvider(QQuickImageProvider::Image),
  cachedBytes_(0) {
}

GalleryStandardImageProvider::~GalleryStandardImageProvider() {
  // NOTE: This assumes that the GSIP is not receiving any requests any longer
  while (!fifo_.isEmpty())
    delete cache_.value(fifo_.takeFirst());
}

void GalleryStandardImageProvider::Init() {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new GalleryStandardImageProvider();
}

GalleryStandardImageProvider* GalleryStandardImageProvider::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

QUrl GalleryStandardImageProvider::ToURL(const QFileInfo& file) {
  return QUrl::fromUserInput(PROVIDER_ID_SCHEME + file.absoluteFilePath());
}

#if GALLERY_LOG_IMAGE
#define LOG_IMAGE_STATUS(status) { \
  resultStr += status; \
}
#else
#define LOG_IMAGE_STATUS(status) { \
}
#endif

QImage GalleryStandardImageProvider::requestImage(const QString& id,
  QSize* size, const QSize& requestedSize) {
  // for LOG_IMAGE_STATUS
  QString resultStr = "";
  
  // Note that even though id looks like a path at this point, it can contain
  // arbitrary "parameters" that came from the original URL.  We use these
  // parameters to ensure that we reload images from disk (skipping QML's
  // internal cache) when editing photos.  So, for now, we strip off everything
  // but the path part of the "URL" (the ?edit=x part is simply ignored).
  QString file = CachedImage::idToFile(id);
  
  CachedImage* cachedImage = NULL;
  
  // lock the cache table and retrieve the element for the cached image; if
  // not found, create one as a placeholder
  cacheMutex_.lock();
  
  cachedImage = cache_.value(file, NULL);
  if (cachedImage != NULL) {
    // remove CachedImage before prepending to FIFO
    fifo_.removeOne(file);
  } else {
    cachedImage = new CachedImage(id);
    cache_.insert(file, cachedImage);
    LOG_IMAGE_STATUS("new-cache-entry ");
  }
  
  // add to front of FIFO
  fifo_.prepend(file);
  
  // should be the same size, always
  Q_ASSERT(cache_.size() == fifo_.size());
  
  // claim the CachedImage *while cacheMutex_ is locked* ... this prevents the
  // CachedImage from being removed from the cache while its being filled
  cachedImage->inUseCount_++;
  
  cacheMutex_.unlock();
  
  // the final image returned to the user
  QImage readyImage;
  Q_ASSERT(readyImage.isNull());
  
  // lock the cached image itself to access
  cachedImage->imageMutex_.lock();
  
  // if image is available, see if a fit
  if (cachedImage->isCacheHit(requestedSize)) {
    readyImage = cachedImage->image_;
    LOG_IMAGE_STATUS("cache-hit ");
  } else if (cachedImage->isReady()) {
    LOG_IMAGE_STATUS("cache-miss ");
  }
  
  int bytesAdded = 0;
  
  // if not available, load now
  if (readyImage.isNull()) {
    QImageReader reader(file);
    
    // load file's original size
    QSize fullSize = reader.size();
    QSize loadSize(fullSize);
    
    // use scaled load-and-decode if size has been requested
    if (fullSize.isValid() && (requestedSize.width() > 0 || requestedSize.height() > 0)) {
      // adjust requested size if necessary, but if small enough, just load the
      // whole thing once and be done with it
      if (fullSize.width() > SCALED_LOAD_FLOOR_DIM_PIXELS
        && fullSize.height() > SCALED_LOAD_FLOOR_DIM_PIXELS) {
        loadSize.scale(requestedSize, Qt::KeepAspectRatioByExpanding);
        if (loadSize.width() > fullSize.width() || loadSize.height() > fullSize.height())
          loadSize = fullSize;
      }
    }
    
    if (loadSize != fullSize) {
      LOG_IMAGE_STATUS("scaled-load ");
      
      // configure reader for scaled load-and-decode
      reader.setScaledSize(loadSize);
    } else {
      LOG_IMAGE_STATUS("full-load ");
    }
    
    readyImage = reader.read();
    if (!readyImage.isNull()) {
      if (!fullSize.isValid())
        fullSize = readyImage.size();
      
      // apply orientation if necessary
      // TODO: Would be more efficient if the caller supplied a known orientation from the
      // media database or, if a thumbnail, a TOP LEFT orientation to skip checking anyway
      std::auto_ptr<PhotoMetadata> metadata(PhotoMetadata::FromFile(file));
      if (metadata.get() != NULL && metadata->orientation() != TOP_LEFT_ORIGIN)
        readyImage = readyImage.transformed(metadata->orientation_transform());
      
      cachedImage->image_ = readyImage;
      cachedImage->fullSize_ = fullSize;
      
      bytesAdded = readyImage.byteCount();
    } else {
      qDebug("Unable to load %s: %s", qPrintable(id), qPrintable(reader.errorString()));
    }
  }
  
  cachedImage->imageMutex_.unlock();
  
  if (readyImage.isNull())
    LOG_IMAGE_STATUS("load-failure ");
  
#if GALLERY_LOG_IMAGE
  long currentCachedBytes = -1;
#endif
  
  // update total cached bytes and remove excess bytes
  cacheMutex_.lock();
  
  cachedBytes_ += bytesAdded;
  
  // free the CachedImage use count inside of *cachedMutex_ lock*
  Q_ASSERT(cachedImage->inUseCount_ > 0);
  cachedImage->inUseCount_--;
  
  // trim the cache
  while (cachedBytes_ > MAX_CACHE_BYTES && !fifo_.isEmpty()) {
    QString droppedFile = fifo_.takeLast();
    
    CachedImage* droppedCachedImage = cache_.value(droppedFile);
    Q_ASSERT(droppedCachedImage != NULL);
    
    // for simplicity, stop when dropped item is in use or doesn't contain
    // an image (which it won't for too long) ... will clean up next time
    // through
    if (droppedCachedImage->inUseCount_ > 0 || !droppedCachedImage->isReady()) {
      fifo_.append(droppedFile);
      
      break;
    }
    
    // remove from map
    cache_.remove(droppedFile);
    
    // decrement total cached size
    cachedBytes_ -= droppedCachedImage->image_.byteCount();
    Q_ASSERT(cachedBytes_ >= 0);
    
    delete droppedCachedImage;
  }
  
#if GALLERY_LOG_IMAGE
  currentCachedBytes = cachedBytes_;
#endif
  
  cacheMutex_.unlock();
  
#if GALLERY_LOG_IMAGE
  if (currentCachedBytes >= 0) {
    qDebug("%s %s req:%dx%d ret:%dx%d cache:%ldb added:%db", qPrintable(resultStr),
      qPrintable(id), requestedSize.width(), requestedSize.height(), readyImage.width(),
      readyImage.height(), currentCachedBytes, bytesAdded);
  } else {
    qDebug("%s %s req:%dx%d ret:%dx%d", qPrintable(resultStr),
      qPrintable(id), requestedSize.width(), requestedSize.height(), readyImage.width(),
      readyImage.height());
  }
#endif
  
  if (size != NULL)
    *size = readyImage.size();
  
  return readyImage;
}

GalleryStandardImageProvider::CachedImage::CachedImage(const QString& id)
  : id_(id), file_(idToFile(id)), inUseCount_(0) {
}

QString GalleryStandardImageProvider::CachedImage::idToFile(const QString& id) {
  return QUrl(id).path();
}

bool GalleryStandardImageProvider::CachedImage::isReady() const {
  return !image_.isNull();
}

bool GalleryStandardImageProvider::CachedImage::isFullSized() const {
  return isReady() && (image_.size() == fullSize_);
}

bool GalleryStandardImageProvider::CachedImage::isCacheHit(const QSize& requestedSize) const {
  if (!isReady())
    return false;
  
  if (isFullSized())
    return true;
  
  if ((requestedSize.width() != 0 && image_.width() >= requestedSize.width())
    || (requestedSize.height() != 0 && image_.height() >= requestedSize.height())) {
    return true;
  }
  
  return false;
}
