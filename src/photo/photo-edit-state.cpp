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
QRect PhotoEditState::rotate_crop_rectangle(Orientation new_orientation,
                                            int image_width,
                                            int image_height) const
{

    OrientationCorrection old_correction =
            OrientationCorrection::FromOrientation(orientation_);
    OrientationCorrection new_correction =
            OrientationCorrection::FromOrientation(new_orientation);

    int x = crop_rectangle_.x();
    int y = crop_rectangle_.y();
    int width = crop_rectangle_.width();
    int height = crop_rectangle_.height();

    if (old_correction.is_flipped_from(new_correction))
        x = image_width - x - width;

    int degrees_rotation =
            new_correction.get_normalized_rotation_difference(old_correction);

    // Rotate in 90 degree increments.  This seemed easier than a switch
    // statement but may mean a few more cycles.
    while(degrees_rotation > 0) {
        int new_x = image_height - y - height;
        int new_y = x;

        x = new_x;
        y = new_y;
        std::swap(width, height);

        degrees_rotation -= 90;
        std::swap(image_width, image_height);
    }

    return QRect(x, y, width, height);
}
