/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 * Eric Gregory <eric@yorba.org>
 * Clint Rogers <clinton@yorba.org>
 */

#include "photo.h"

// database
#include "database.h"
#include "media-table.h"

// media
#include "media-collection.h"

// medialoader
#include "photo-metadata.h"

// util
#include "imaging.h"

// src
#include "gallery-manager.h"

#include <QApplication>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

/*!
 * \brief Photo::isValid
 * \param file
 * \return
 */
bool Photo::isValid(const QFileInfo& file)
{
    QImageReader reader(file.filePath());
    QByteArray format = reader.format();

    if (QString(format).toLower() == "tiff") {
        // QImageReader.canRead() will detect some raw files as readable TIFFs,
        // though QImage will fail to load them.
        QString extension = file.suffix().toLower();
        if (extension != "tiff" && extension != "tif")
            return false;
    }

    return reader.canRead();
}

/*!
 * \brief Photo::Photo
 * \param file
 */
Photo::Photo(const QFileInfo& file)
    : MediaSource(file),
      m_originalSize(),
      m_originalOrientation(TOP_LEFT_ORIGIN)
{
    QByteArray format = QImageReader(file.filePath()).format();
    m_fileFormat = QString(format).toLower();
    if (m_fileFormat == "jpg") // Why does Qt expose two different names here?
        m_fileFormat = "jpeg";
}

/*!
 * \brief Photo::~Photo
 */
Photo::~Photo()
{
}

/*!
 * \reimp
 */
MediaSource::MediaType Photo::type() const
{
    return MediaSource::Photo;
}

/*!
 * \brief Photo::orientation
 * \return
 */
Orientation Photo::orientation() const
{
    return m_originalOrientation;
}

/*!
 * \brief Photo::DestroySource
 * \param destroyBacking
 * \param asOrphan
 */
void Photo::destroySource(bool destroyBacking, bool asOrphan)
{
    MediaSource::destroySource(destroyBacking, asOrphan);
}

/*!
 * \brief Photo::fileFormat returns the file format as QString
 * \return
 */
const QString &Photo::fileFormat() const
{
    return m_fileFormat;
}

/*!
 * \brief Photo::fileFormatHasMetadata
 * \return
 */
bool Photo::fileFormatHasMetadata() const
{
    return (m_fileFormat == "jpeg" || m_fileFormat == "tiff" ||
            m_fileFormat == "png");
}

/*!
 * \brief Photo::fileFormatHasOrientation
 * \return
 */
bool Photo::fileFormatHasOrientation() const
{
    return (m_fileFormat == "jpeg");
}

/*!
 * \brief Photo::setOriginalOrientation
 * \param orientation
 */
void Photo::setOriginalOrientation(Orientation orientation)
{
    m_originalOrientation = orientation;
}

/*!
 * \brief Photo::originalOrientation returns the original orientation
 * \return
 */
Orientation Photo::originalOrientation() const
{
    return m_originalOrientation;
}

/*!
 * \brief Photo::originalSize
 * \return
 */
const QSize &Photo::originalSize()
{
    return m_originalSize;
}

/*!
 * \brief Photo::originalSize
 * \return
 */
bool Photo::canBeEdited() const
{
    return QImageWriter::supportedImageFormats().contains(m_fileFormat.toUtf8());
}
