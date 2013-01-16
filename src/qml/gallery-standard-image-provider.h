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

#ifndef GALLERY_GALLERY_STANDARD_IMAGE_PROVIDER_H_
#define GALLERY_GALLERY_STANDARD_IMAGE_PROVIDER_H_

#include <QObject>
#include <QQuickImageProvider>
#include <QFileInfo>
#include <QImage>
#include <QMap>
#include <QMutex>
#include <QSize>
#include <QString>
#include <QUrl>

#include "photo/photo-metadata.h"

/*
 * Gallery uses a custom image provider for three reasons:
 *
 * 1. QML's image loader does not respect EXIF orientation.  This provider will
 *    rotate and mirror the image as necessary.
 *
 * 2. QML's image caching appears to be directly related to image size (scaling)
 *    which leads to thrashing when animating a thumbnail or loading images at
 *    various sizes (such as in the album viewer versus the photo viewer).
 *    The strategy here is to cache the largest requested size of the image
 *    and allow QML to downscale it as necessary.  This minimizes expensive
 *    JPEG load-and-decodes.
 *
 * 3. Logging (enabled with --log-image-loading) allows for monitoring of
 *    all image I/O, useful when debugging and optimizing.
 */

class GalleryStandardImageProvider
  : public QObject, public QQuickImageProvider {
  Q_OBJECT
  
 public:
  static const char* PROVIDER_ID;
  static const char* PROVIDER_ID_SCHEME;
  
  static const char* REVISION_PARAM_NAME;
  static const char* ORIENTATION_PARAM_NAME;
  
  virtual ~GalleryStandardImageProvider();
  
  static QUrl ToURL(const QFileInfo& file);
  
  GalleryStandardImageProvider();

  virtual QImage requestImage(const QString& id, QSize* size,
    const QSize& requestedSize);
  
 private:
  class CachedImage {
   public:
    const QString id_;
    const QUrl uri_;
    const QString file_;
    QMutex imageMutex_;
    
    // these fields should only be accessed when imageMutex_ is locked
    bool hasOrientation_;
    QImage image_;
    QSize fullSize_;
    Orientation orientation_;
    
    // the following should only be accessed when cacheMutex_ is locked; the
    // counter controls removing a CachedImage entry from the cache table
    int inUseCount_;
    uint byteCount_;
    
    CachedImage(const QString& id);
    
    static QString idToFile(const QString& id);
    
    // the following should only be called when imageMutex_ is locked
    void storeImage(const QImage& image, const QSize& fullSize, Orientation orientation);
    bool isFullSized() const;
    bool isReady() const;
    bool isCacheHit(const QSize& requestedSize) const;
  };
  
  QMap<QString, CachedImage*> cache_;
  QList<QString> fifo_;
  QMutex cacheMutex_;
  long cachedBytes_;
  
  static QSize orientSize(const QSize& size, Orientation orientation);
  
  // Returns a CachedImage with an inUseCount > 0, meaning it cannot be
  // removed from the cache until released
  CachedImage* claim_cached_image_entry(const QString& id, QString& loggingStr);
  
  // Inspects and loads a proper image for this request into the CachedImage
  QImage fetch_cached_image(CachedImage* cachedImage, const QSize& requestedSize,
    uint* bytesLoaded, QString& loggingStr);
  
  // Releases a CachedImage to the cache; takes its bytes loaded (0 if nothing
  // was loaded) and returns the current cached byte total
  void release_cached_image_entry(CachedImage* cachedImage, uint bytesLoaded,
  long* currentCachedBytes, int* currentCacheEntries);
};

#endif // GALLERY_GALLERY_STANDARD_IMAGE_PROVIDER_H_
