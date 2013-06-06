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

#include <cstdio>

#include "orientation.h"

/*!
 * \brief OrientationCorrection::FromOrientation
 * \param o
 * \return
 */
OrientationCorrection OrientationCorrection::FromOrientation(Orientation o)
{
    double rotation_angle = 0.0;
    double horizontal_scale_factor = 1.0;

    switch (o) {
    case TOP_RIGHT_ORIGIN:
        horizontal_scale_factor = -1.0;
        break;

    case BOTTOM_RIGHT_ORIGIN:
        rotation_angle = 180.0;
        break;

    case BOTTOM_LEFT_ORIGIN:
        horizontal_scale_factor = -1.0;
        rotation_angle = 180.0;
        break;

    case LEFT_TOP_ORIGIN:
        horizontal_scale_factor = -1.0;
        rotation_angle = -90.0;
        break;

    case RIGHT_TOP_ORIGIN:
        rotation_angle = 90.0;
        break;

    case RIGHT_BOTTOM_ORIGIN:
        horizontal_scale_factor = -1.0;
        rotation_angle = 90.0;
        break;

    case LEFT_BOTTOM_ORIGIN:
        rotation_angle = -90.0;
        break;

    default:
        ; // do nothing
        break;
    }

    return OrientationCorrection(rotation_angle, horizontal_scale_factor);
}

/*!
 * \brief OrientationCorrection::Identity
 * \return
 */
OrientationCorrection OrientationCorrection::Identity()
{
    return OrientationCorrection(0.0, 1.0);
}

/*!
 * \brief OrientationCorrection::rotate_orientation
 * \param orientation
 * \param left
 * \return
 */
Orientation OrientationCorrection::rotate_orientation(Orientation orientation, bool left)
{
    QVector<Orientation> sequence_a;
    QVector<Orientation> sequence_b;
    sequence_a <<
                  TOP_LEFT_ORIGIN << LEFT_BOTTOM_ORIGIN << BOTTOM_RIGHT_ORIGIN << RIGHT_TOP_ORIGIN;
    sequence_b <<
                  TOP_RIGHT_ORIGIN << RIGHT_BOTTOM_ORIGIN << BOTTOM_LEFT_ORIGIN << LEFT_TOP_ORIGIN;

    const QVector<Orientation>& sequence = (
                sequence_a.contains(orientation) ? sequence_a : sequence_b);

    int current = sequence.indexOf(orientation);
    int jump = (left ? 1 : sequence.count() - 1);
    int next = (current + jump) % sequence.count();

    return sequence[next];
}

/*!
 * \brief OrientationCorrection::to_transform
 * Returns the correction as a QTransform.
 * \return Returns the correction as a QTransform.
 */
QTransform OrientationCorrection::to_transform() const
{
    QTransform result;
    result.scale(horizontal_scale_factor_, 1.0);
    result.rotate(rotation_angle_);

    return result;
}

/*!
 * \brief OrientationCorrection::is_flipped_from
 * Returns whether the two orientations are flipped relative to each other.
 * Ignores rotation_angle; only checks horizontal_scale_factor_.
 * \param other
 * \return
 */
bool OrientationCorrection::is_flipped_from(
        const OrientationCorrection& other) const
{
    return (horizontal_scale_factor_ != other.horizontal_scale_factor_);
}

/*!
 * \brief OrientationCorrection::get_normalized_rotation_difference
 * Returns the rotation difference in degrees (this - other), normalized to
 * 0, 90, 180, or 270.  Ignores the horizontal_scale_factor_.
 * \param other
 * \return
 */
int OrientationCorrection::get_normalized_rotation_difference(
        const OrientationCorrection& other) const
{
    int degrees_rotation = (int)rotation_angle_ - (int)other.rotation_angle_;
    if (degrees_rotation < 0)
        degrees_rotation += 360;

    Q_ASSERT(degrees_rotation == 0 || degrees_rotation == 90 ||
             degrees_rotation == 180 || degrees_rotation == 270);
    return degrees_rotation;
}
