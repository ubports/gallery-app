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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "photo-edit-state.h"

/*!
 * \brief PhotoEditState::rotate_crop_rectangle
 * Returns a new crop rectangle oriented with the new_orientation.  Note that
 * image_width/height must be specified in this PhotoEditState's orientation,
 * not the new_orientation.
 *
 * TODO: this code could be significantly simplified since
 *       OrientationCorrection objects can be asked to provide a QTransform
 *       encoding the required correction transformation. Since QTransform
 *       objects can act directly on QRects and QRectFs, this function
 *       doesn't really need the image_width and image_height. It should
 *       be possible to simply invert the QTransform associated with the
 *       old correction, apply it to the crop_rectangle_ QRect and then
 *       apply the QTransform associated with new_correction to the result.
 * \param new_orientation
 * \param image_width
 * \param image_height
 * \return
 */
QRect PhotoEditState::rotateCropRectangle(Orientation newOrientation,
                                            int imageWidth,
                                            int imageHeight) const
{

    OrientationCorrection old_correction =
            OrientationCorrection::fromOrientation(orientation_);
    OrientationCorrection new_correction =
            OrientationCorrection::fromOrientation(newOrientation);

    int x = crop_rectangle_.x();
    int y = crop_rectangle_.y();
    int width = crop_rectangle_.width();
    int height = crop_rectangle_.height();

    if (old_correction.isFlippedFrom(new_correction))
        x = imageWidth - x - width;

    int degrees_rotation =
            new_correction.getNormalizedRotationDifference(old_correction);

    // Rotate in 90 degree increments.  This seemed easier than a switch
    // statement but may mean a few more cycles.
    while(degrees_rotation > 0) {
        int new_x = imageHeight - y - height;
        int new_y = x;

        x = new_x;
        y = new_y;
        std::swap(width, height);

        degrees_rotation -= 90;
        std::swap(imageWidth, imageHeight);
    }

    return QRect(x, y, width, height);
}
