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

/*!
 * \brief Resource::Resource
 * \param application_dir the directory of where the executable is
 * \param install_dir the directory, where apps are installed to
 * \param view the view is used to determine the max texture size
 */
Resource::Resource(QQuickView *view)
    : view_(view),
      max_texture_size_(0)
{
}

/*!
 * \brief Resource::get_rc_url
 * Converts a relative path in the resource folder to a fully-qualified URL.
 * Do NOT use this function if you need an absolute path.
 * \param path
 * \return
 */
QUrl Resource::get_rc_url(const QString& path) const
{
    return isRunningInstalled() ?
                QUrl::fromLocalFile(galleryDirectory() + "/rc/" + path) :
                QUrl::fromLocalFile(galleryDirectory() + "/../rc/" + path);
}

/*!
 * \brief maxTextureSize
 * \return max texture size provided by OpenGL
 */
int Resource::maxTextureSize() const
{
    if (max_texture_size_ == 0 && view_ != 0) {
        // QtQuick uses a dedicated rendering thread we can't access. A temporary
        // graphics context has to be created to access implementation limits.
        QOpenGLContext context;
        context.create();
        view_->winId();  // Ensure the QPA window is created.
        context.makeCurrent(view_);
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size_);
        if (max_texture_size_ == 0) {
            max_texture_size_ = 1024;
        }
    }

    return max_texture_size_;
}
