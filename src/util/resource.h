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
#ifndef RESOURCE_H
#define RESOURCE_H

#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QUrl>

class QQuickView;

/*!
 * \brief The Resource class
 */
class Resource
{
public:
    explicit Resource(bool desktopMode, const QString& pictureDir);

    static QUrl getRcUrl(const QString& path);

    const QStringList &mediaDirectories() const;
    const QString &databaseDirectory() const;
    const QString &thumbnailDirectory() const;

    bool isVideoPath(const QString& filePath);

    void setVideoDirectories(const QStringList &dirs);

private:
    QStringList m_mediaDirectories;
    QStringList m_videoDirectories;
    mutable QString m_databaseDirectory;
    mutable QString m_thumbnailDirectory;

    // Path to database, relative to application data path.
    static const QLatin1String DATABASE_DIR;
    // Path to database, relative to application cache path.
    static const QLatin1String THUMBNAIL_DIR;
    // Path to camera recording videos, relative to movies standard path.
    static const QLatin1String CAMERA_RECORD_DIR;

    friend class tst_Resource;
};

#endif // RESOURCE_H
