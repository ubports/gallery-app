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

class GalleryStandardImageProvider
  : public QObject, public QQuickImageProvider {
  Q_OBJECT
  
 public:
  static const char* PROVIDER_ID;
  static const char* PROVIDER_ID_SCHEME;
  
  virtual ~GalleryStandardImageProvider();
  
  static void Init();
  
  static GalleryStandardImageProvider* instance();
  
  static QUrl ToURL(const QFileInfo& file);
  
  virtual QImage requestImage(const QString& id, QSize* size,
    const QSize& requestedSize);
  
 private:
  class CachedImage {
   public:
    const QString id_;
    const QString file_;
    QMutex imageMutex_;
    
    // these fields should only be accessed when imageMutex_ is locked
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
  
  static GalleryStandardImageProvider* instance_;
  
  QMap<QString, CachedImage*> cache_;
  QList<QString> fifo_;
  QMutex cacheMutex_;
  long cachedBytes_;
  
  static QSize orientSize(const QSize& size, Orientation orientation);
  
  GalleryStandardImageProvider();
  
  // Returns a CachedImage with an inUseCount > 0, meaning it cannot be
  // removed from the cache until released
  CachedImage* claim_cached_image_entry(const QString& id, QString& loggingStr);
  
  // Inspects and loads a proper image for this request into the CachedImage
  QImage fetch_cached_image(CachedImage* cachedImage, const QSize& requestedSize,
    uint* bytesLoaded, QString& loggingStr);
  
  // Releases a CachedImage to the cache; takes its bytes loaded (0 if nothing
  // was loaded) and returns the current cached byte total
  void release_cached_image_entry(CachedImage* cachedImage, uint bytesLoaded,
    long* currentCachedBytes, int* currentCacheEntries, QString& loggingStr);
};

#endif // GALLERY_GALLERY_STANDARD_IMAGE_PROVIDER_H_
