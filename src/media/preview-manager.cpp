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

#include <QCryptographicHash>
#include <QDir>
#include <QMutexLocker>

#include "preview-manager.h"
#include "media-collection.h"
#include "photo/photo.h"

// FIXME adapt to different sizes. This is fixed size for the demo device
const int PreviewManager::PREVIEW_SIZE = 360; // in pixel
const int PreviewManager::THUMBNAIL_SIZE = 216; // in pixel
const int PreviewManager::PREVIEW_QUALITY = 70;
const char* PreviewManager::PREVIEW_FILE_FORMAT = "JPEG";
const char* PreviewManager::PREVIEW_FILE_EXT = ".jpg";
const QString PreviewManager::PREVIEW_DIR = "preview";
const QString PreviewManager::THUMBNAIL_DIR = "square";

QMutex PreviewManager::m_createMutex;

/*!
 * \brief PreviewManager::PreviewManager
 */
PreviewManager::PreviewManager(const QString &thumbnailDirectory,
                               MediaCollection *mediaCollection, QObject *parent)
    : QObject(parent),
      m_thumbnailDir(thumbnailDirectory),
      m_mediaCollection(mediaCollection)
{
    Q_ASSERT(m_mediaCollection);

    if (m_thumbnailDir.right(1) != QDir::separator())
        m_thumbnailDir += QDir::separator();

    // create the thumbnail directory if not already present
    QDir dir;
    dir.mkpath(m_thumbnailDir + PREVIEW_DIR);
    dir.mkpath(m_thumbnailDir + THUMBNAIL_DIR);
}

/*!
 * \brief PreviewManager::onMediaAddedRemoved
 * \param added
 * \param removed
 */
void PreviewManager::onMediaAddedRemoved(const QSet<DataObject*>* added,
                                            const QSet<DataObject*>* removed)
{
    if (added != NULL) {
        DataObject* object;
        foreach (object, *added) {
            MediaSource* source = qobject_cast<MediaSource*>(object);

            QObject::connect(source, SIGNAL(data_altered()),
                             this, SLOT(updatePreview()), Qt::UniqueConnection);
        }
    }

    if (removed != NULL) {
        DataObject* object;
        foreach (object, *removed) {
            MediaSource* source = qobject_cast<MediaSource*>(object);
            QObject::disconnect(source, SIGNAL(data_altered()),
                                this, SLOT(updatePreview()));
        }
    }
}

/*!
 * \brief PreviewManager::onMediaDestroying
 * \param destroying
 */
void PreviewManager::onMediaDestroying(const QSet<DataObject*>* destroying)
{
    if (destroying != NULL) {
        DataObject* object;
        foreach (object, *destroying)
            destroyPreviews(qobject_cast<MediaSource*>(object));
    }
}

/*!
 * \brief PreviewManager::updatePreview
 */
void PreviewManager::updatePreview()
{
    QObject* object = QObject::sender();
    MediaSource* source = qobject_cast<MediaSource*>(object);

    ensurePreview(source->file(), true);
}

/*!
 * \brief PreviewManager::previewFileName
 * \param file
 * \return
 */
QString PreviewManager::previewFileName(const QFileInfo& file) const
{
    return thumbnailFileName(file.canonicalFilePath(), PREVIEW_DIR);
}

/*!
 * \brief PreviewManager::thumbnailFileName
 * \param file
 * \return
 */
QString PreviewManager::thumbnailFileName(const QFileInfo& file) const
{
    return thumbnailFileName(file.canonicalFilePath(), THUMBNAIL_DIR);
}

/*!
 * \brief PreviewManager::ensurePreview
 * \param file
 * \param regen
 * \return
 */
bool PreviewManager::ensurePreview(QFileInfo file, bool regen)
{
    QMutexLocker locker(&m_createMutex);

    // If preview file exists, considered valid (unless we're regenerating it).
    QString preview = previewFileName(file);
    QString thumbnail = thumbnailFileName(file);

    QImage thumbMaster;
    if (updateNeeded(file, QFileInfo(preview)) || regen) {
        Photo* photo = m_mediaCollection->photoFromFileinfo(file);
        QSize previewSize(PREVIEW_SIZE, PREVIEW_SIZE);
        QImage fullsized(photo->Image(true, previewSize));
        if (fullsized.isNull()) {
            qDebug() << "Unable to generate fullsized image for " << file.filePath() << "not generating preview";
            return false;
        }

        // scale the preview so it will fill the viewport specified by PREVIEW_*_MAX
        // these values are replicated in the QML so that the preview will fill each
        // grid cell, cropping down to the center of the image if necessary
        QImage scaled = (fullsized.height() > fullsized.width())
                ? fullsized.scaledToWidth(PREVIEW_SIZE, Qt::SmoothTransformation)
                : fullsized.scaledToHeight(PREVIEW_SIZE, Qt::SmoothTransformation);

        if (scaled.isNull()) {
            qDebug() << "Unable to scale " << file.filePath() << "for preview";
            return false;
        }

        if (!saveThumbnail(scaled, preview)) {
            qDebug("Unable to save preview %s", qPrintable(preview));
            return false;
        }
        thumbMaster = scaled;
    }

    if (updateNeeded(file, QFileInfo(thumbnail)) || regen) {
        if (thumbMaster.isNull()) {
            thumbMaster.load(preview);
            if (thumbMaster.isNull()) {
                qDebug("Unable load preview image for %s, not generating thumbnail",
                       qPrintable(preview));
                return false;
            }
        }

        QImage square = generateThumbnail(thumbMaster);
        if (!saveThumbnail(square, thumbnail)) {
            qDebug("Unable to save preview %s", qPrintable(thumbnail));
            return false;
        }
    }

    return true;
}

/*!
 * \brief PreviewManager::saveThumbnail saves the thumbnail in a safe way
 * The image is written to a temporary file, and then moved (the move is atomic)
 * \param image thumbnail to save
 * \param fileName filename of the thumbnail
 * \return true, if saving the file was successful
 */
bool PreviewManager::saveThumbnail(const QImage &image, const QString &fileName) const
{
    QString temporaryName(fileName+".tmp");
    bool ok;
    ok = image.save(temporaryName, PREVIEW_FILE_FORMAT, PREVIEW_QUALITY);
    if (!ok)
        return false;

    QFile thumbnail(temporaryName);
    return thumbnail.rename(fileName);
}

/*!
 * \brief PreviewManager::destroyPreviews
 * \param media
 */
void PreviewManager::destroyPreviews(MediaSource* media)
{
    QString filename = previewFileName(media->file());
    if (!QFile::remove(filename))
        qDebug("Unable to remove preview %s", qPrintable(filename));
    filename = thumbnailFileName(media->file());
    if (!QFile::remove(filename))
        qDebug("Unable to remove thumbnail %s", qPrintable(filename));
}

/*!
 * \brief PreviewManager::generateThumbnail
 * \param master
 * \return
 */
QImage PreviewManager::generateThumbnail(const QImage &master) const
{
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

/*!
 * \brief PreviewManager::thumbnailFileName generates the file name for a given
 * file and it's level (size/type)
 * \param fileName full filename URI (file:///directory/file.name.png)
 * \param levelName correclates to the sub directory inside of the thumbnail directory
 * \return full filePath of the thumbnail
 */
QString PreviewManager::thumbnailFileName(const QString &fileName,
                                          const QString &levelName) const
{
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QUrl::fromLocalFile(fileName).toEncoded());
    QString previewDir = m_thumbnailDir + levelName + QDir::separator();
    return QString(previewDir + md5.result().toHex() + PREVIEW_FILE_EXT);
}

/*!
 * \brief PreviewManager::updateNeeded checks if the thumbnail needs to be (re-)created
 * \param mediaFile the original photo/video
 * \param previewFile the preview file
 * \return true if the thumbnail needs to be created
 */
bool PreviewManager::updateNeeded(const QFileInfo &mediaFile, const QFileInfo &previewFile) const
{
    if (!previewFile.exists())
        return true;

    return mediaFile.lastModified() > previewFile.lastModified();
}
