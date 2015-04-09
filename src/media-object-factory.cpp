/*
 * Copyright (C) 2013 Canonical Ltd
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
 */

#include "media-object-factory.h"

// database
#include "database.h"
#include "media-table.h"

// medialoader
#include "photo-metadata.h"
#include "video-metadata.h"

// photo
#include "photo.h"

// video
#include <video.h>

#include <QApplication>

QWaitCondition listNotEmptyCondition;
QMutex createMutex;
QStringList createQueue;

/*!
 * \brief MediaObjectFactory::MediaObjectFactory
 * \param mediaTable
 */
MediaObjectFactory::MediaObjectFactory(bool desktopMode, Resource *res)
    : m_workerThread(this),
      m_isRunCreateRunning(false)
{
    m_worker = new MediaObjectFactoryWorker();
    m_worker->moveToThread(&m_workerThread);
    QObject::connect(&m_workerThread, SIGNAL(finished()),
                     m_worker, SLOT(deleteLater()));
    
    QObject::connect(m_worker, SIGNAL(mediaObjectCreated(MediaSource*)),
                     this, SIGNAL(mediaObjectCreated(MediaSource*)), Qt::QueuedConnection);
    QObject::connect(m_worker, SIGNAL(mediaFromDBLoaded(QSet<DataObject *>)),
                     this, SIGNAL(mediaFromDBLoaded(QSet<DataObject *>)), Qt::QueuedConnection);

    m_workerThread.start(QThread::LowPriority);
}

MediaObjectFactory::~MediaObjectFactory()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

/*!
 * \brief MediaObjectFactory::setMediaTable
 * \param mediaTable
 */
void MediaObjectFactory::setMediaTable(MediaTable *mediaTable)
{
    m_worker->setMediaTable(mediaTable);
}

/*!
 * \brief GalleryManager::enableContentLoadFilter enable filter to load only
 * content of certain type
 * \param filterType
 */
void MediaObjectFactory::enableContentLoadFilter(MediaSource::MediaType filterType)
{
    m_worker->enableContentLoadFilter(filterType);
}

/*!
 * \brief MediaObjectFactory::clear
 */
void MediaObjectFactory::clear()
{
    QMetaObject::invokeMethod(m_worker, "clear", Qt::QueuedConnection);
}

/*!
 * \brief MediaObjectFactory::create loads the data for a photo or video file.
 * Creates / updates the database as well.
 * \param file the file to load
 * \return 0 if this no valid photo/video file
 */
void MediaObjectFactory::create(const QFileInfo &file, int priority, bool desktopMode, Resource *res)
{
    enqueuePath(file.absoluteFilePath(), priority);
    if (!m_isRunCreateRunning) {
        QMetaObject::invokeMethod(m_worker, "runCreate", Qt::QueuedConnection);
        m_isRunCreateRunning = true;
    }
}

/*!
 * \brief MediaObjectFactory::loadMediaFromDB creates a set with all photos and video
 * stored in the DB.
 * Someone else needs to take the responsibility to delete all the objects in the set.
 * You should call clear() afterwards, to remove temporary data.
 * \return All media stored in the DB
 */
void MediaObjectFactory::loadMediaFromDB()
{
    QMetaObject::invokeMethod(m_worker, "mediaFromDB", Qt::QueuedConnection);
}

void MediaObjectFactory::enqueuePath(const QString &path, int priority)
{
    createMutex.lock();
    if (priority == Qt::HighEventPriority)
        createQueue.prepend(path);
    else
        createQueue.append(path);
    createMutex.unlock();
    listNotEmptyCondition.wakeAll();
}

MediaObjectFactoryWorker::MediaObjectFactoryWorker(QObject *parent)
    : QObject(parent),
      m_mediaTable(),
      m_filterType(MediaSource::None)
{
}

MediaObjectFactoryWorker::~MediaObjectFactoryWorker()
{
}

void MediaObjectFactoryWorker::runCreate()
{
    forever {
        QString path;
        createMutex.lock();
        if (createQueue.isEmpty()) {
            listNotEmptyCondition.wait(&createMutex);
        }

        path = createQueue.takeFirst();
        createMutex.unlock();

        QFileInfo file(path);
        if(file.exists()) {
            create(path);
        }
    }
}

void MediaObjectFactoryWorker::setMediaTable(MediaTable *mediaTable)
{
    m_mediaTable = mediaTable;
}

void MediaObjectFactoryWorker::enableContentLoadFilter(MediaSource::MediaType filterType)
{
    m_filterType = filterType;
}

void MediaObjectFactoryWorker::clear()
{
    clearMetadata();
    m_mediaFromDB.clear();
}

void MediaObjectFactoryWorker::create(const QString &path)
{
    Q_ASSERT(m_mediaTable);

    QFileInfo file(path);

    clearMetadata();

    MediaSource::MediaType mediaType = MediaSource::Photo;
    if (Video::isCameraVideo(file))
        mediaType = MediaSource::Video;

    if (m_filterType != MediaSource::None && mediaType != m_filterType)
        return;

    // Look for video in the database.
    qint64 id = m_mediaTable->getIdForMedia(file.absoluteFilePath());

    if (id == INVALID_ID) {
        if (mediaType == MediaSource::Video && !Video::isValid(file))
            return;
        if (mediaType == MediaSource::Photo && !Photo::isValid(file))
            return;
    }

    MediaSource *media = 0;
    Photo *photo = 0;
    if (mediaType == MediaSource::Photo) {
        photo = new Photo(file);
        media = photo;
    } else {
        media = new Video(file);
    }
    media->setMediaTable(m_mediaTable);

    if (id == INVALID_ID) {
        bool metadataOk;
        if (mediaType == MediaSource::Photo) {
            metadataOk = readPhotoMetadata(photo->file());
        } else {
            metadataOk = readVideoMetadata(file);
        }

        if (!metadataOk) {
            delete media;
            return;
        }

        // This will cause the real size to be read from the file
        if (photo) m_size = photo->size();

        // Add to DB.
        id = m_mediaTable->createIdForMedia(file.absoluteFilePath(), m_timeStamp,
                                            m_exposureTime, m_orientation, m_fileSize, m_size);
    } else {
        // Load metadata from DB.
        m_mediaTable->getRow(id, m_size, m_orientation, m_timeStamp, m_exposureTime);
    }
    media->setSize(m_size);
    media->setFileTimestamp(m_timeStamp);
    if (mediaType == MediaSource::Photo) {
        media->setExposureDateTime(m_exposureTime);
        photo->setOriginalOrientation(m_orientation);
    } else {
        media->setExposureDateTime(m_exposureTime.toLocalTime());
    }
    media->setId(id);

    media->moveToThread(QApplication::instance()->thread());
    emit mediaObjectCreated(media);
}

void MediaObjectFactoryWorker::mediaFromDB()
{
    Q_ASSERT(m_mediaTable);

    m_mediaFromDB.clear();

    connect(m_mediaTable,
            SIGNAL(row(qint64,QString,QSize,QDateTime,QDateTime,Orientation,qint64)),
            this,
            SLOT(addMedia(qint64,QString,QSize,QDateTime,QDateTime,Orientation,qint64)));

    m_mediaTable->emitAllRows();

    disconnect(m_mediaTable,
               SIGNAL(row(qint64,QString,QSize,QDateTime,QDateTime,Orientation,qint64)),
               this,
               SLOT(addMedia(qint64,QString,QSize,QDateTime,QDateTime,Orientation,qint64)));

    emit mediaFromDBLoaded(m_mediaFromDB);
}

/*!
 * \brief MediaObjectFactory::clearMetadata resets all memeber variables
 * regarding metadata
 */
void MediaObjectFactoryWorker::clearMetadata()
{
    m_timeStamp = QDateTime();
    m_exposureTime = QDateTime();
    m_orientation = TOP_LEFT_ORIGIN;
    m_fileSize = 0;
    m_size = QSize();
}

/*!
 * \brief MediaObjectFactory::readPhotoMetadata
 * \param file
 * \return 0 if there was an error reading the metadata
 */
bool MediaObjectFactoryWorker::readPhotoMetadata(const QFileInfo &file)
{
    PhotoMetadata* metadata = PhotoMetadata::fromFile(file);
    if (metadata == 0)
        return false;

    m_timeStamp = file.lastModified();
    m_orientation = metadata->orientation();
    m_fileSize = file.size();
    m_exposureTime = metadata->exposureTime().isValid() ?
                QDateTime(metadata->exposureTime()) : m_timeStamp;
    m_size = QSize();

    delete metadata;
    return true;
}

/*!
 * \brief MediaObjectFactory::readVideoMetadata
 * \param file
 * \return 0 if there was an error reading the metadata
 */
bool MediaObjectFactoryWorker::readVideoMetadata(const QFileInfo &file)
{
    if (!Video::isValid(file))
        return false;

    VideoMetadata metadata(file);
    bool ok = metadata.parseMetadata();
    if (!ok)
        return false;

    m_timeStamp = file.created();
    if (metadata.rotation() == 90) {
        m_orientation = LEFT_BOTTOM_ORIGIN;
    } else if (metadata.rotation() == 180) {
        m_orientation = BOTTOM_RIGHT_ORIGIN;
    } else if (metadata.rotation() == 270) {
        m_orientation = RIGHT_TOP_ORIGIN;
    } else {
        m_orientation = TOP_LEFT_ORIGIN;
    }
    m_fileSize = file.size();
    m_exposureTime = metadata.exposureTime();
    // Exposure time is stored as UTC on metadata
    m_exposureTime.setTimeSpec(Qt::UTC);
    m_size = metadata.frameSize();

    return true;
}

/*!
 * \brief MediaObjectFactory::addMedia creates a media object, and adds it to the
 * internal set. This is used for mediaFromDB().
 * \param mediaId
 * \param filename
 * \param size
 * \param timestamp
 * \param exposureTime
 * \param originalOrientation
 * \param filesize
 * \return
 */
void MediaObjectFactoryWorker::addMedia(qint64 mediaId, const QString &filename,
                                  const QSize &size, const QDateTime &timestamp,
                                  const QDateTime &exposureTime,
                                  Orientation originalOrientation, qint64 filesize)
{
    Q_UNUSED(filesize);

    QFileInfo file(filename);
    if (!file.exists()) {
        m_mediaTable->remove(mediaId);
        return;
    }

    MediaSource::MediaType mediaType = MediaSource::Photo;
    if (Video::isCameraVideo(file))
        mediaType = MediaSource::Video;

    MediaSource *media = 0;
    Photo *photo = 0;
    if (mediaType == MediaSource::Photo) {
        photo = new Photo(file);
        media = photo;
    } else {
        media = new Video(file);
    }
    media->setMediaTable(m_mediaTable);

    media->setSize(size);
    media->setFileTimestamp(timestamp);
    if (mediaType == MediaSource::Photo) {
        media->setExposureDateTime(exposureTime);
        photo->setOriginalOrientation(originalOrientation);
    } else {
        // Exposure time is stored as UTC on DB
        QDateTime utc(exposureTime);
        utc.setTimeSpec(Qt::UTC);
        media->setExposureDateTime(utc.toLocalTime());
    }
    media->setId(mediaId);

    media->moveToThread(QApplication::instance()->thread());
    m_mediaFromDB.insert(media);
}
