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

// util
#include "orientation.h"

#include <QFileInfo>
#include <QImage>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QQuickImageProvider>
#include <QSize>
#include <QString>
#include <QUrl>

/*!
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
class GalleryStandardImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

public:
    static const char* PROVIDER_ID;
    static const char* PROVIDER_ID_SCHEME;

    static const char* REVISION_PARAM_NAME;
    static const char* ORIENTATION_PARAM_NAME;

    static const char* SIZE_KEY;

    GalleryStandardImageProvider();
    virtual ~GalleryStandardImageProvider();

    static QUrl toURL(const QFileInfo& file);

    virtual QImage requestImage(const QString& id, QSize* size,
                                const QSize& requestedSize);

    void setLogging(bool enableLogging);
    void setMaxLoadResolution(int resolution);

private:
    class CachedImage {
    public:
        const QString id;
        const QUrl uri;
        const QString file;
        QMutex imageMutex;

        // these fields should only be accessed when imageMutex_ is locked
        bool hasOrientation;
        QImage image;
        QSize fullSize;
        Orientation orientation;

        // the following should only be accessed when cacheMutex_ is locked; the
        // counter controls removing a CachedImage entry from the cache table
        int inUseCount;
        uint byteCount;

        CachedImage(const QString& fileId, const QString& filename);

        void storeImage(const QImage& newImage, const QSize& newFullSize,
                        Orientation newOrientation);
        bool isFullSized() const;
        bool isReady() const;
        bool isCacheHit(const QSize& requestedSize) const;
    };

    QMap<QString, CachedImage*> m_cache;
    QList<QString> m_fifo;
    QMutex m_cacheMutex;
    long m_cachedBytes;
    bool m_logImageLoading;
    int m_maxLoadResolution;

    static QSize orientSize(const QSize& size, Orientation orientation);

    CachedImage* claimCachedImageEntry(const QString& id, QString& loggingStr);

    QImage fetchCachedImage(CachedImage* cachedImage, const QSize& requestedSize,
                              uint* bytesLoaded, QString& loggingStr);

    void releaseCachedImageEntry(CachedImage* cachedImage, uint bytesLoaded,
                                    long* currentCachedBytes, int* currentCacheEntries);

    QString idToFile(const QString& id) const;

    //Allow our test access to private variables.
    friend class tst_GalleryStandardImageProvider;
};

#endif // GALLERY_GALLERY_STANDARD_IMAGE_PROVIDER_H_
