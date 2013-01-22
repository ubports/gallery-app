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

#include "photo/photo-metadata.h"

Orientation PhotoMetadata::orientation() const
{
    return Orientation();
}

PhotoMetadata* PhotoMetadata::FromFile(const QFileInfo &file)
{
    QFileInfo test = file;
    return NULL;
}

OrientationCorrection OrientationCorrection::FromOrientation(Orientation o)
{
    OrientationCorrection test = OrientationCorrection::FromOrientation(o);
    return test;
}

QTransform OrientationCorrection::to_transform() const
{
  QTransform result;
  return result;
}