/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Nicolas d'Offay <nicolas.doffay@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "photo-metadata.h"

#include <QDateTime>

PhotoMetadata::PhotoMetadata(const char* filepath)
    : m_fileSourceInfo(filepath)
{
}

Orientation PhotoMetadata::orientation() const
{
    return BOTTOM_LEFT_ORIGIN;
}

PhotoMetadata* PhotoMetadata::fromFile(const char* filepath)
{
    return new PhotoMetadata(filepath);
}

PhotoMetadata* PhotoMetadata::fromFile(const QFileInfo &file)
{
    if (file.suffix() == QLatin1String("jpg")) {
        return PhotoMetadata::fromFile(file.absoluteFilePath().toStdString().c_str());
    } else {
        return 0;
    }
}

QDateTime PhotoMetadata::exposureTime() const
{
    return QDateTime(QDate(2013, 01, 01), QTime(11, 11, 11));
}

QTransform PhotoMetadata::orientationTransform() const
{
    return QTransform();
}

void PhotoMetadata::setOrientation(Orientation orientation)
{
    Q_UNUSED(orientation);
}

bool PhotoMetadata::save() const
{
    return true;
}
