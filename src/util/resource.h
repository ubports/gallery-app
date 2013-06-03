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
#include <QUrl>

class QQuickView;

/*!
 * \brief The Resource class
 */
class Resource
{
public:
    explicit Resource(const QString& pictureDir, QQuickView *view);

    static QUrl get_rc_url(const QString& path);

    const QString &picturesDirectory() const;
    const QString &databaseDirectory() const;

    int maxTextureSize() const;
    void setView(QQuickView* view);

private:
    QString m_pictureDirectory;
    mutable QString m_databaseDirectory;
    QQuickView* m_view;
    mutable int m_maxTextureSize;

    // Path to database, relative to application data path.
    static const QLatin1String DATABASE_DIR;

    friend class tst_Resource;
};

#endif // RESOURCE_H
