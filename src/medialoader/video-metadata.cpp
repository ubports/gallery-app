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

#include "video-metadata.h"

#include <QDebug>
#include <QFileInfo>
#include <QUrl>

#include <MediaInfo/MediaInfo.h>

const QString DURATION_KEY("Duration");
const QString ENCODED_DATE_KEY("EncodedDate");
const QString ROTATION_KEY("Rotation");
const QString FRAME_SIZE_KEY("FrameSize");

using namespace MediaInfoLib;

/*!
 * \brief VideoMetadata::VideoMetadata
 * \param parent
 */
VideoMetadata::VideoMetadata(const QFileInfo &file, QObject *parent)
    :QObject(parent)
{
    m_file = file;
}

/*!
 * \brief VideoMetadata::parseMetadata reads the metadata from a
 * video file
 * \param file the video file
 * \return true if the parsing was successful, false if an error occured
 */
bool VideoMetadata::parseMetadata()
{
    String filename = m_file.absoluteFilePath().toStdWString();
    MediaInfo mediaInfo;
    size_t ret = mediaInfo.Open(filename);
    if (ret == 0) {
        qWarning() << "Can't read metadata from file" << m_file.absoluteFilePath();
        return false;
    }

    String value;
    value = mediaInfo.Get(Stream_Video, 0, __T("Rotation"));
    QString qvalue = QString::fromStdWString(value);
    m_tags[ROTATION_KEY] = QVariant((int)qvalue.toDouble());

    value = mediaInfo.Get(Stream_General, 0, __T("Duration"));
    qvalue = QString::fromStdWString(value);
    m_tags[DURATION_KEY] = QVariant((int)qvalue.toDouble());

    value = mediaInfo.Get(Stream_General, 0, __T("Encoded_Date"));
    qvalue = QString::fromStdWString(value);
    QString format = "UTC yyyy-MM-dd hh:mm:ss";
    QDateTime dt = QDateTime::fromString(qvalue, format);
    if (dt.isValid())
        m_tags[ENCODED_DATE_KEY] = QVariant(dt);

    value = mediaInfo.Get(Stream_Video, 0, __T("Width"));
    qvalue = QString::fromStdWString(value);
    int width = qvalue.toInt();
    value = mediaInfo.Get(Stream_Video, 0, __T("Height"));
    qvalue = QString::fromStdWString(value);
    int height = qvalue.toInt();
    m_tags[FRAME_SIZE_KEY] = QVariant(QSize(width, height));

    mediaInfo.Close();
    return true;
}

/*!
 * \brief VideoMetadata::exposureTime returns the date and time, this video
 * was captured
 * \return
 */
QDateTime VideoMetadata::exposureTime() const
{
    QMap<QString, QVariant>::const_iterator it;
    it = m_tags.find(ENCODED_DATE_KEY);
    if ( it == m_tags.end()) {
        // Fallback for date that the file was created
        return m_file.created();
    }

    return it.value().toDateTime();
}

/*!
 * \brief VideoMetadata::rotation returns the rotation for the
 * compostion matrix in degrees
 * \return
 */
int VideoMetadata::rotation() const
{
    QMap<QString, QVariant>::const_iterator it;
    it = m_tags.find(ROTATION_KEY);
    if ( it == m_tags.end())
        return 0;

    return it.value().toInt();
}

/*!
 * \brief VideoMetadata::duration returns the video's duration in ms
 * \return
 */
int VideoMetadata::duration() const
{
    QMap<QString, QVariant>::const_iterator it;
    it = m_tags.find(DURATION_KEY);
    if ( it == m_tags.end())
        return 0;

    return it.value().toInt();
}

/*!
 * \brief VideoMetadata::frameSize return the width and height of
 * the video frames
 * \return
 */
QSize VideoMetadata::frameSize() const
{
    QMap<QString, QVariant>::const_iterator it;
    it = m_tags.find(FRAME_SIZE_KEY);
    if ( it == m_tags.end())
        return QSize();

    return it.value().toSize();
}
