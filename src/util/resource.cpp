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

#include <QtGui/QOpenGLContext>
#include <QtQuick/QQuickView>
#include <QStandardPaths>

const QLatin1String Resource::DATABASE_DIR = QLatin1String("database");

/*!
 * \brief Resource::Resource
 * \param application_dir the directory of where the executable is
 * \param install_dir the directory, where apps are installed to
 * \param view the view is used to determine the max texture size
 */
Resource::Resource(const QString &pictureDir, QQuickView *view)
    : m_pictureDirectory(pictureDir),
      m_databaseDirectory(""),
      m_view(view),
      m_maxTextureSize(0)
{
    if (m_pictureDirectory.isEmpty())
        m_pictureDirectory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}

/*!
 * \brief Resource::get_rc_url
 * Converts a relative path in the resource folder to a fully-qualified URL.
 * Do NOT use this function if you need an absolute path.
 * \param path
 * \return
 */
QUrl Resource::get_rc_url(const QString& path)
{
    return isRunningInstalled() ?
                QUrl::fromLocalFile(galleryDirectory() + "/rc/" + path) :
                QUrl::fromLocalFile(galleryDirectory() + "/../rc/" + path);
}

/*!
 * \brief Resource::picturesDirectory
 * \return Returns the directory for the pictures
 */
const QString &Resource::picturesDirectory() const
{
    return m_pictureDirectory;
}

/*!
 * \brief Resource::databaseDirectory directory for the database
 * \return the directory the database is stored
 */
const QString &Resource::databaseDirectory() const
{
    if (m_databaseDirectory.isEmpty()) {
        if (m_pictureDirectory == QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)) {
            m_databaseDirectory = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
                    QDir::separator() + DATABASE_DIR;
        } else {
            m_databaseDirectory = m_pictureDirectory + QDir::separator() + "." + DATABASE_DIR;
        }
    }
    return m_databaseDirectory;
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
