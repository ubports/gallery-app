/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 */

#ifndef GALLERY_PHOTO_METADATA_H_
#define GALLERY_PHOTO_METADATA_H_

#include <QObject>
#include <QDateTime>
#include <QFileInfo>
#include <QString>
#include <QSet>
#include <QTransform>

#include <exiv2/exiv2.hpp>

enum Orientation {
    MIN_ORIENTATION = 1,
    TOP_LEFT_ORIGIN = 1,
    TOP_RIGHT_ORIGIN = 2,
    BOTTOM_RIGHT_ORIGIN = 3,
    BOTTOM_LEFT_ORIGIN = 4,
    LEFT_TOP_ORIGIN = 5,
    RIGHT_TOP_ORIGIN = 6,
    RIGHT_BOTTOM_ORIGIN = 7,
    LEFT_BOTTOM_ORIGIN = 8,
    MAX_ORIENTATION = 8
};

/*!
 * \brief The OrientationCorrection struct
 */
class OrientationCorrection
{
public:
    static OrientationCorrection FromOrientation(Orientation o);
    static OrientationCorrection Identity();

    const double rotation_angle_;
    const double horizontal_scale_factor_;

    QTransform to_transform() const;

    bool is_flipped_from(const OrientationCorrection& other) const;
    int get_normalized_rotation_difference(const OrientationCorrection& other) const;

private:
    OrientationCorrection(double rotation_angle, double horizontal_scale_factor)
        : rotation_angle_(rotation_angle),
          horizontal_scale_factor_(horizontal_scale_factor) { }
};

/*!
 * \brief The PhotoMetadata class
 */
class PhotoMetadata : public QObject
{
    Q_OBJECT

public:
    static PhotoMetadata* FromFile(const char* filepath);
    static PhotoMetadata* FromFile(const QFileInfo& file);

    static Orientation rotate_orientation(Orientation orientation, bool left);
    
    QDateTime exposure_time() const;
    Orientation orientation() const;
    QTransform orientation_transform() const;
    OrientationCorrection orientation_correction() const;

    void set_orientation(Orientation orientation);

    bool save() const;

private:
    PhotoMetadata(const char* filepath);
    
    Exiv2::Image::AutoPtr image_;
    QSet<QString> keys_present_;
    QFileInfo file_source_info_;
};

#endif // GALLERY_PHOTO_METADATA_H_
