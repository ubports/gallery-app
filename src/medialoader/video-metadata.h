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

#ifndef GALLERY_VIDEO_METADATA_H_
#define GALLERY_VIDEO_METADATA_H_

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>
#include <QFileInfo>

/*!
 * \brief The VideoMetadata class
 */
class VideoMetadata : public QObject
{
    Q_OBJECT

public:
    VideoMetadata(const QFileInfo& file, QObject *parent=0);

    bool parseMetadata();

    QDateTime exposureTime() const;
    int rotation() const;
    int duration() const;
    QSize frameSize() const;

private:
    QMap<QString, QVariant> m_tags;
    QFileInfo m_file;
};

#endif // GALLERY_VIDEO_METADATA_H_
