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

#include <QFileInfo>

VideoMetadata::VideoMetadata(const QFileInfo& file, QObject *parent)
    :QObject(parent)
{
    Q_UNUSED(file);
}

bool VideoMetadata::parseMetadata()
{
    return true;
}

QDateTime VideoMetadata::exposureTime() const
{
    return QDateTime();
}

int VideoMetadata::rotation() const
{
    return 0;
}

int VideoMetadata::duration() const
{
    return 0;
}

QSize VideoMetadata::frameSize() const
{
    return QSize();
}
