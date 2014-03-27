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
 * \param view the view is used to determine the max texture size
 */
Resource::Resource(bool desktopMode, const QString &pictureDir, QQuickView *view)
    : m_mediaDirectories(),
      m_databaseDirectory(""),
      m_thumbnailDirectory(""),
      m_view(view),
      m_maxTextureSize(0)
{
    if (isClick()) {
        setOrganization();
    }
    if (!pictureDir.isEmpty() && QDir(pictureDir).exists()) {
        m_mediaDirectories.append(pictureDir);
    } else {
        m_mediaDirectories.append(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
        if (desktopMode) {
            m_mediaDirectories.append(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
        } else {
            // If running on device, we must just display videos recorded by Camera, those videos
            // are gonna be saved on a specific directory
            QString moviesPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
            if (!moviesPath.isEmpty()) {
                moviesPath += "/" + CAMERA_RECORD_DIR;
                m_mediaDirectories.append(moviesPath);
            }
        }
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

/*!
 * \brief maxTextureSize
 * \return max texture size provided by OpenGL
 */
int Resource::maxTextureSize() const
{
    if (m_maxTextureSize == 0 && m_view != 0) {
        // QtQuick uses a dedicated rendering thread we can't access. A temporary
        // graphics context has to be created to access implementation limits.
        QOpenGLContext context;
        context.create();
        m_view->winId();  // Ensure the QPA window is created.
        context.makeCurrent(m_view);
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
        if (m_maxTextureSize == 0) {
            m_maxTextureSize = 1024;
        }
    }

    return m_maxTextureSize;
}
