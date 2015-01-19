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
 * Eric Gregory <eric@yorba.org>
 */

#include "resource.h"
#include "config.h"

#include <QDebug>
#include <QtGui/QOpenGLContext>
#include <QtQuick/QQuickView>
#include <QStandardPaths>

const QLatin1String Resource::DATABASE_DIR = QLatin1String("database");
const QLatin1String Resource::THUMBNAIL_DIR = QLatin1String("thumbnails");
const QLatin1String Resource::CAMERA_RECORD_DIR = QLatin1String("camera");

/*!
 * \brief Resource::Resource
 * \param application_dir the directory of where the executable is
 * \param install_dir the directory, where apps are installed to
 */
Resource::Resource(bool desktopMode, const QString &pictureDir)
    : m_mediaDirectories(),
      m_databaseDirectory(""),
      m_thumbnailDirectory("")
{
    if (isClick()) {
        setOrganization();
    }
    if (!pictureDir.isEmpty() && QDir(pictureDir).exists()) {
        m_mediaDirectories.append(pictureDir);
    } else {
        m_mediaDirectories.append(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

        m_mediaDirectories.append(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
        m_videoDirectories.append(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    }

    QString userName = qgetenv("USER");
    if (QDir("/media/" + userName).exists()) {
        m_mediaDirectories.append(QString("/media/" + userName));
        m_videoDirectories.append(QString("/media/" + userName));
    }
}

/*!
 * \brief Resource::getRcUrl
 * Converts a relative path in the resource folder to a fully-qualified URL.
 * Do NOT use this function if you need an absolute path.
 * \param path
 * \return
 */
QUrl Resource::getRcUrl(const QString& path)
{
    if (isClick() || isRunningInstalled()) {
        return QUrl::fromLocalFile(galleryDirectory() + "/rc/" + path);
    } else {
        return QUrl::fromLocalFile(galleryDirectory() + "/../rc/" + path);
    }
}

/*!
 * \brief Resource::picturesDirectory
 * \return Returns the directory for the pictures
 */
const QStringList &Resource::mediaDirectories() const
{
    return m_mediaDirectories;
}

/*!
 * \brief Resource::databaseDirectory directory for the database
 * \return the directory the database is stored
 */
const QString &Resource::databaseDirectory() const
{
    if (m_databaseDirectory.isEmpty()) {
        if (m_mediaDirectories.contains(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))) {
            m_databaseDirectory = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
                    QDir::separator() + DATABASE_DIR;
        } else {
            m_databaseDirectory = m_mediaDirectories.at(0) + QDir::separator() + "." + DATABASE_DIR;
        }
    }
    return m_databaseDirectory;
}

/*!
 * \brief Resource::thumbnailDirectory returns the base directory of the thumbnails
 * \return
 */
const QString &Resource::thumbnailDirectory() const
{
    if (m_thumbnailDirectory.isEmpty()) {
        if (m_mediaDirectories.contains(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))) {
            m_thumbnailDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                    QDir::separator() + THUMBNAIL_DIR;
        } else {
            m_thumbnailDirectory = m_mediaDirectories.at(0) + QDir::separator() + "." + THUMBNAIL_DIR;
        }
    }
    return m_thumbnailDirectory;
}

bool Resource::isVideoPath(const QString& filePath)
{
    foreach (const QString& videoPath, m_videoDirectories) {
        if (filePath.contains(videoPath)) {
            return true;
        }
    }

    return false;
}

void Resource::setVideoDirectories(const QStringList &dirs)
{
    m_videoDirectories = dirs;
}

