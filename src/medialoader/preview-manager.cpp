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

#include "preview-manager.h"
#include "photo-metadata.h"
#include "video-metadata.h"

// media
#include "media-source.h"

#include <QCryptographicHash>
#include <QDir>
#include <QImageReader>
#include <QMutexLocker>

#include <gst/gst.h>

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
PreviewManager::PreviewManager(const QString &thumbnailDirectory, QObject *parent)
    : QObject(parent),
      m_thumbnailDir(thumbnailDirectory)
{
    if (m_thumbnailDir.right(1) != QDir::separator())
        m_thumbnailDir += QDir::separator();

    // create the thumbnail directory if not already present
    QDir dir;
    dir.mkpath(m_thumbnailDir + PREVIEW_DIR);
    dir.mkpath(m_thumbnailDir + THUMBNAIL_DIR);

    gst_init(0, 0);
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

            QObject::connect(source, SIGNAL(dataChanged()),
                             this, SLOT(updatePreview()), Qt::UniqueConnection);
        }
    }

    if (removed != NULL) {
        DataObject* object;
        foreach (object, *removed) {
            MediaSource* source = qobject_cast<MediaSource*>(object);
            QObject::disconnect(source, SIGNAL(dataChanged()),
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
        QSize previewSize(PREVIEW_SIZE, PREVIEW_SIZE);
        QImage fullsized;
        if (file.suffix().compare("mp4", Qt::CaseInsensitive) == 0) {
            fullsized = grabVideoThumbnail(file.canonicalFilePath());
        } else {
            fullsized = loadPhoto(file.canonicalFilePath(), previewSize);
        }
        if (fullsized.isNull()) {
            qDebug() << "Unable to generate fullsized image for " << file.filePath()
                     << "not generating preview";
            return false;
        }

        fullsized.setText("Thumb::URI", QUrl::fromLocalFile(file.canonicalFilePath()).toEncoded());
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
    if (!ok) {
        qWarning() << "Unanble to save the thumbnail" << temporaryName;
        return false;
    }

    if (QFile::exists(fileName)) {
        QFile::remove(fileName);
    }

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

/*!
 * \brief PreviewManager::loadPhoto load a photo with correct orientation
 * The size can be limited as well, to speed upt the loading
 * \param fileName file name of the photo
 * \param maxSize limits the size, so that one side fits into maxSize. If an
 * invalid size is passed, the image is loaded in full size.
 * \return the photo corrctly sized and oriented
 */
QImage PreviewManager::loadPhoto(const QString &fileName, const QSize& maxSize) const
{
    QImageReader imageReader(fileName);

    QSize imageSize = imageReader.size();
    if (maxSize.isValid()) {
        QSize size = imageSize;
        size.scale(maxSize, Qt::KeepAspectRatioByExpanding);
        imageReader.setScaledSize(size);
    }

    QImage image = imageReader.read();
    if (!image.isNull()) {
        PhotoMetadata* metadata = PhotoMetadata::fromFile(fileName.toStdString().c_str());
        image = image.transformed(
                    OrientationCorrection::fromOrientation(metadata->orientation())
                    .toTransform());
        delete metadata;
    } else {
        qWarning() << "Could not load the image" << fileName << "for thumbnail generation";
    }

    return image;
}

/*!
 * \brief destroyFrameData cleanup function for the video thumbnail QImage
 * \param data
 */
static void destroyFrameData (void *data)
{
    gst_buffer_unref(GST_BUFFER (data));
}

/*!
 * \brief PreviewManager::grabVideoThumbnail grabs a frame from the given video file
 * \param fileName
 * \return
 */
QImage PreviewManager::grabVideoThumbnail(const QString &fileName) const
{
    QUrl url("file://"+fileName);
    gchar *guri = g_strdup(url.toString().toUtf8().data());

    GstElement *asink;
    GstElement *vsink;

    GstElement *pipeline = gst_element_factory_make("playbin2", "play");
    GstCaps *caps = gst_caps_new_simple("video/x-raw-rgb",
                                        "bpp", G_TYPE_INT, 24,
                                        "depth", G_TYPE_INT, 24,
                                        "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
                                        "endianness", G_TYPE_INT, G_BIG_ENDIAN,
                                        "red_mask", G_TYPE_INT, 0xff0000,
                                        "green_mask", G_TYPE_INT, 0x00ff00,
                                        "blue_mask", G_TYPE_INT, 0x0000ff,
                                        NULL);
    asink = gst_element_factory_make("fakesink", "audio-fake-sink");
    vsink = gst_element_factory_make("fakesink", "video-fake-sink");
    g_object_set(vsink, "sync", TRUE, NULL);
    g_object_set(pipeline,
                 "flags", 0x00000001, // Make sure to render only the video stream (we do not need audio here)
                 "audio-sink", asink,
                 "video-sink", vsink,
                 NULL);

    g_object_set(pipeline, "uri", guri, NULL);
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
    switch (ret)
    {
        case GST_STATE_CHANGE_FAILURE:
            qWarning() << "Fail to start thumbnail pipeline";
            return QImage();
        case GST_STATE_CHANGE_NO_PREROLL:
            qWarning() << "Thumbnail not supported for live sources";
            return QImage();
        default:
            break;
    }

    gst_element_seek(pipeline, 1.0,
                     GST_FORMAT_TIME,  static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                     GST_SEEK_TYPE_SET, 100 * GST_MSECOND,
                     GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
    /* And wait for this seek to complete */
    gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);

    GstBuffer *buf = 0;
    g_signal_emit_by_name(pipeline, "convert-frame", caps, &buf);

    QImage thumb;
    if (buf && GST_BUFFER_CAPS(buf)) {
        gint width, height;
        GstStructure *s = gst_caps_get_structure(GST_BUFFER_CAPS (buf), 0);
        gboolean res = gst_structure_get_int(s, "width", &width);
        res |= gst_structure_get_int (s, "height", &height);
        if (!res) {
            qWarning() << "could not get snapshot dimension";
            return QImage();
        }

        thumb = QImage(GST_BUFFER_DATA(buf), width, height, QImage::Format_RGB888, destroyFrameData, buf);
    }

    gst_caps_unref(caps);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_free(guri);

    VideoMetadata metadata;
    metadata.parseMetadata(QFileInfo(fileName));
    int rotation = metadata.rotation();
    if (rotation != 0) {
        QTransform transform;
        transform.rotate(rotation);
        thumb = thumb.transformed(transform);
    }

    return thumb;
}
