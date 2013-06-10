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

#include "gallery-standard-image-provider.h"
#include "photo-metadata.h"
#include "preview-manager.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QImageReader>
#include <QSize>
#include <QUrlQuery>

const char* GalleryStandardImageProvider::PROVIDER_ID = "gallery-standard";
const char* GalleryStandardImageProvider::PROVIDER_ID_SCHEME = "image://gallery-standard/";

const char* GalleryStandardImageProvider::REVISION_PARAM_NAME = "edit";
const char* GalleryStandardImageProvider::ORIENTATION_PARAM_NAME = "orientation";

const char* GalleryStandardImageProvider::SIZE_KEY = "size_level";

const long MAX_CACHE_BYTES = 20L * 1024L * 1024L;

// fully load previews into memory when requested
const int SCALED_LOAD_FLOOR_DIM_PIXELS =
        qMax(PreviewManager::PREVIEW_SIZE, PreviewManager::PREVIEW_SIZE);

/*!
 * \brief GalleryStandardImageProvider::GalleryStandardImageProvider
 */
GalleryStandardImageProvider::GalleryStandardImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image),
      cachedBytes_(0),
      m_previewManager(0),
      m_logImageLoading(false),
      maxLoadResolution_(INT_MAX)
{
}

/*!
 * \brief GalleryStandardImageProvider::~GalleryStandardImageProvider
 */
GalleryStandardImageProvider::~GalleryStandardImageProvider()
{
    // NOTE: This assumes that the GSIP is not receiving any requests any longer
    while (!fifo_.isEmpty())
        delete cache_.value(fifo_.takeFirst());
}

/*!
 * \brief GalleryStandardImageProvider::ToURL
 * \param file
 * \return
 */
QUrl GalleryStandardImageProvider::ToURL(const QFileInfo& file)
{
    return QUrl::fromUserInput(PROVIDER_ID_SCHEME + file.absoluteFilePath());
}

#define LOG_IMAGE_STATUS(status) { \
    if (m_logImageLoading) \
    loggingStr += status; \
    }

/*!
 * \brief GalleryStandardImageProvider::requestImage
 * \param id
 * \param size
 * \param requestedSize
 * \return
 */
QImage GalleryStandardImageProvider::requestImage(const QString& id,
                                                  QSize* size, const QSize& requestedSize)
{
    if (!m_previewManager) {
        qWarning() << Q_FUNC_INFO << "no PreviewManager set";
        return QImage();
    }

    // for LOG_IMAGE_STATUS
    QString loggingStr = "";
    QElapsedTimer timer;
    timer.start();

    QUrl url(id);
    QFileInfo photoFile(url.path());
    m_previewManager->ensurePreview(photoFile);

    CachedImage* cachedImage = claim_cached_image_entry(id, loggingStr);
    Q_ASSERT(cachedImage != NULL);

    uint bytesLoaded = 0;
    QImage readyImage = fetch_cached_image(cachedImage, requestedSize, &bytesLoaded,
                                           loggingStr);
    if (readyImage.isNull())
        LOG_IMAGE_STATUS("load-failure ");

    long currentCachedBytes = 0;
    int currentCacheEntries = 0;
    release_cached_image_entry(cachedImage, bytesLoaded, &currentCachedBytes, &currentCacheEntries);

    if (m_logImageLoading) {
        if (bytesLoaded > 0) {
            qDebug("%s %s req:%dx%d ret:%dx%d cache:%ldb/%d loaded:%db time:%lldms", qPrintable(loggingStr),
                   qPrintable(id), requestedSize.width(), requestedSize.height(), readyImage.width(),
                   readyImage.height(), currentCachedBytes, currentCacheEntries, bytesLoaded,
                   timer.elapsed());
        } else {
            qDebug("%s %s req:%dx%d ret:%dx%d cache:%ldb/%d time:%lldms", qPrintable(loggingStr),
                   qPrintable(id), requestedSize.width(), requestedSize.height(), readyImage.width(),
                   readyImage.height(), currentCachedBytes, currentCacheEntries, timer.elapsed());
        }
    }

    if (size != NULL)
        *size = readyImage.size();

    return readyImage;
}

void GalleryStandardImageProvider::setPreviewManager(PreviewManager *previewManager)
{
    m_previewManager = previewManager;
}

void GalleryStandardImageProvider::setLogging(bool enableLogging)
{
    m_logImageLoading = enableLogging;
}

/*!
 * \brief GalleryStandardImageProvider::setMaxLoadResolution sets the maximal size of the loaded
 * images. Images loaded are limited to a max width/height, but keep their aspect ratio.
 * Limiting the size is useful to not exceed the texture size limit of the GPU. Or to limit for
 * performance reasons.
 * Default is to have no limit (INT_MAX).
 * \param resolution maxiaml length in pixel
 */
void GalleryStandardImageProvider::setMaxLoadResolution(int resolution)
{
    if (resolution > 0)
        maxLoadResolution_ = resolution;
}

/*!
 * \brief GalleryStandardImageProvider::claim_cached_image_entry
 * Returns a CachedImage with an inUseCount > 0, meaning it cannot be
 * removed from the cache until released
 * \param id
 * \param loggingStr
 * \return
 */
GalleryStandardImageProvider::CachedImage* GalleryStandardImageProvider::claim_cached_image_entry(
        const QString& id, QString& loggingStr)
{
    // lock the cache table and retrieve the element for the cached image; if
    // not found, create one as a placeholder
    cacheMutex_.lock();

    CachedImage* cachedImage = cache_.value(id, NULL);
    if (cachedImage != NULL) {
        // remove CachedImage before prepending to FIFO
        fifo_.removeOne(id);
    } else {
        cachedImage = new CachedImage(id, idToFile(id));
        cache_.insert(id, cachedImage);
        LOG_IMAGE_STATUS("new-cache-entry ");
    }

    // add to front of FIFO
    fifo_.prepend(id);

    // should be the same size, always
    Q_ASSERT(cache_.size() == fifo_.size());

    // claim the CachedImage *while cacheMutex_ is locked* ... this prevents the
    // CachedImage from being removed from the cache while its being filled
    cachedImage->inUseCount_++;

    cacheMutex_.unlock();

    return cachedImage;
}

/*!
 * \brief GalleryStandardImageProvider::fetch_cached_image Inspects and loads a proper image
 * Inspects and loads a proper image for this request into the CachedImage
 * \param cachedImage
 * \param requestedSize
 * \param bytesLoaded
 * \param loggingStr
 * \return
 */
QImage GalleryStandardImageProvider::fetch_cached_image(CachedImage *cachedImage,
                                                        const QSize& requestedSize, uint* bytesLoaded, QString& loggingStr)
{
    Q_ASSERT(cachedImage != NULL);

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

    if (bytesLoaded != NULL)
        *bytesLoaded = 0;

    // if not available, load now
    if (readyImage.isNull()) {
        QImageReader reader(cachedImage->file_);

        // load file's original size
        QSize fullSize = reader.size();
        QSize loadSize(fullSize);

        // use scaled load-and-decode if size has been requested
        if (fullSize.isValid() && (requestedSize.width() > 0 || requestedSize.height() > 0)) {
            // adjust requested size if necessary, but if small enough, just load the
            // whole thing once and be done with it
            if (fullSize.width() > SCALED_LOAD_FLOOR_DIM_PIXELS
                    && fullSize.height() > SCALED_LOAD_FLOOR_DIM_PIXELS) {
                loadSize.scale(requestedSize, Qt::KeepAspectRatio);
                if (loadSize.width() > fullSize.width() || loadSize.height() > fullSize.height())
                    loadSize = fullSize;
            }
        }

        if (loadSize.width() > maxLoadResolution_ || loadSize.height() > maxLoadResolution_) {
            loadSize.scale(maxLoadResolution_, maxLoadResolution_, Qt::KeepAspectRatio);
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

            // If orientation not supplied in URI, load from file and save in cache
            Orientation orientation = TOP_LEFT_ORIGIN;
            if (cachedImage->hasOrientation_) {
                orientation = cachedImage->orientation_;
            } else {
                std::auto_ptr<PhotoMetadata> metadata(PhotoMetadata::FromFile(
                                                          cachedImage->file_));
                if (metadata.get() != NULL)
                    orientation = metadata->orientation();
            }

            // rotate image if not TOP LEFT
            if (orientation != TOP_LEFT_ORIGIN)  {
                readyImage = readyImage.transformed(
                            OrientationCorrection::fromOrientation(orientation).toTransform());
            }

            cachedImage->storeImage(readyImage, fullSize, orientation);

            if (bytesLoaded != NULL)
                *bytesLoaded = readyImage.byteCount();
        } else {
            qDebug("Unable to load %s: %s", qPrintable(cachedImage->id_),
                   qPrintable(reader.errorString()));
        }
    }

    cachedImage->imageMutex_.unlock();

    return readyImage;
}

/*!
 * \brief GalleryStandardImageProvider::release_cached_image_entry
 * Releases a CachedImage to the cache; takes its bytes loaded (0 if nothing
 * was loaded) and returns the current cached byte total
 * \param cachedImage
 * \param bytesLoaded
 * \param currentCachedBytes
 * \param currentCacheEntries
 */
void GalleryStandardImageProvider::release_cached_image_entry
(CachedImage *cachedImage, uint bytesLoaded,
 long *currentCachedBytes, int *currentCacheEntries)
{
    Q_ASSERT(cachedImage != NULL);

    // update total cached bytes and remove excess bytes
    cacheMutex_.lock();

    cachedBytes_ += bytesLoaded;

    // update the CachedImage use count and byte count inside of *cachedMutex_ lock*
    Q_ASSERT(cachedImage->inUseCount_ > 0);
    cachedImage->inUseCount_--;
    if (bytesLoaded != 0)
        cachedImage->byteCount_ = bytesLoaded;

    // trim the cache
    QList<CachedImage*> dropList;
    while (cachedBytes_ > MAX_CACHE_BYTES && !fifo_.isEmpty()) {
        QString droppedFile = fifo_.takeLast();

        CachedImage* droppedCachedImage = cache_.value(droppedFile);
        Q_ASSERT(droppedCachedImage != NULL);

        // for simplicity, stop when dropped item is in use or doesn't contain
        // an image (which it won't for too long) ... will clean up next time
        // through
        if (droppedCachedImage->inUseCount_ > 0) {
            fifo_.append(droppedFile);

            break;
        }

        // remove from map
        cache_.remove(droppedFile);

        // decrement total cached size
        cachedBytes_ -= droppedCachedImage->byteCount_;
        Q_ASSERT(cachedBytes_ >= 0);

        dropList.append(droppedCachedImage);
    }

    // coherency is good
    Q_ASSERT(cache_.size() == fifo_.size());

    if (currentCachedBytes != NULL)
        *currentCachedBytes = cachedBytes_;

    if (currentCacheEntries != NULL)
        *currentCacheEntries = cache_.size();

    cacheMutex_.unlock();

    // perform actual deletion outside of lock
    while (!dropList.isEmpty())
        delete dropList.takeFirst();
}

/*!
 * \brief GalleryStandardImageProvider::orientSize
 * \param size
 * \param orientation
 * \return
 */
QSize GalleryStandardImageProvider::orientSize(const QSize& size, Orientation orientation)
{
    switch (orientation) {
    case LEFT_TOP_ORIGIN:
    case RIGHT_TOP_ORIGIN:
    case RIGHT_BOTTOM_ORIGIN:
    case LEFT_BOTTOM_ORIGIN:
        return QSize(size.height(), size.width());
        break;

    default:
        // no change
        return size;
    }
}

/*!
 * \brief GalleryStandardImageProvider::idToFile
 * \param id
 * \return
 */
QString GalleryStandardImageProvider::idToFile(const QString& id) const
{
    if (!m_previewManager) {
        qWarning() << Q_FUNC_INFO << "no PreviewManager set";
        return QString();
    }

    QUrl url = QUrl(id);
    QString fileName = url.path();

    //Get our item value from our query by it's key.
    QUrlQuery url_query(url);
    url_query.hasQueryItem(GalleryStandardImageProvider::SIZE_KEY);
    QString value = url_query.queryItemValue(GalleryStandardImageProvider::SIZE_KEY);

    if (value == "1") {
        fileName = m_previewManager->previewFileName(fileName);
    }

    return fileName;
}

/*!
 * \brief GalleryStandardImageProvider::CachedImage::CachedImage
 * \param id the full URI of the image
 * \param fileName the filename for the URI (can be the file itself or the preview)
 */
GalleryStandardImageProvider::CachedImage::CachedImage(const QString& id,
                                                       const QString& filename)
    : id_(id), uri_(id), file_(filename), hasOrientation_(false),
      orientation_(TOP_LEFT_ORIGIN), inUseCount_(0), byteCount_(0)
{
    QUrlQuery query(uri_);
    if (query.hasQueryItem(ORIENTATION_PARAM_NAME)) {
        QString value = query.queryItemValue(ORIENTATION_PARAM_NAME);
        if (!value.isEmpty()) {
            bool ok = false;
            int toInt = value.toInt(&ok);
            if (ok && toInt >= MIN_ORIENTATION && toInt <= MAX_ORIENTATION) {
                orientation_ = (Orientation) toInt;
                hasOrientation_ = true;
            }
        }
    }
}

/*!
 * \brief GalleryStandardImageProvider::CachedImage::storeImage
 * Importand: the following should only be called when imageMutex_ is locked
 * \param image
 * \param fullSize
 * \param orientation
 */
void GalleryStandardImageProvider::CachedImage::storeImage(const QImage& image,
                                                           const QSize& fullSize, Orientation orientation)
{
    image_ = image;
    fullSize_ = orientSize(fullSize, orientation);
    hasOrientation_ = true;
    orientation_ = orientation;
}

/*!
 * \brief GalleryStandardImageProvider::CachedImage::isReady
 * \return
 */
bool GalleryStandardImageProvider::CachedImage::isReady() const
{
    return !image_.isNull();
}

/*!
 * \brief GalleryStandardImageProvider::CachedImage::isFullSized
 * \return
 */
bool GalleryStandardImageProvider::CachedImage::isFullSized() const
{
    return isReady() && (image_.size() == fullSize_);
}

/*!
 * \brief GalleryStandardImageProvider::CachedImage::isCacheHit
 * \param requestedSize
 * \return
 */
bool GalleryStandardImageProvider::CachedImage::isCacheHit(const QSize& requestedSize) const
{
    if (!isReady())
        return false;

    if (isFullSized())
        return true;

    QSize properRequestedSize = orientSize(requestedSize, orientation_);

    if ((properRequestedSize.width() != 0 && image_.width() >= properRequestedSize.width())
            || (properRequestedSize.height() != 0 && image_.height() >= properRequestedSize.height())) {
        return true;
    }

    return false;
}
