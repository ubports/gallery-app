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

Resource::Resource(const QString& application_dir, const QString& install_dir)
{
    app_dir_ = QDir(application_dir);
    if (trailing_slash(app_dir_.absolutePath()).endsWith("/bin/"))
        app_dir_.cdUp();
  
    install_dir_ = QDir(install_dir);
}

bool Resource::is_installed() const
{
    return app_dir_ == install_dir_;
}

QString Resource::trailing_slash(QString path) const
{
    return path.endsWith("/") ? path : path + "/";
}

QUrl Resource::get_rc_url(const QString& path) const
{
    return is_installed() ?
        QUrl::fromLocalFile(trailing_slash(install_dir_.path()) + "share/gallery/rc/" + path) :
        QUrl::fromLocalFile(trailing_slash(app_dir_.path()) + "../rc/" + path);
}

int Resource::maxTextureSize() const
{
    // FIXME tm: OpenGL query to get
    // max texture size returns 0 if not in render thread
    return 2048;
}
