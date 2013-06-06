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

#ifndef GALLERY_PHOTO_EDIT_STATE_H_
#define GALLERY_PHOTO_EDIT_STATE_H_

#include <QRect>
#include <QVector4D>

#include "orientation.h"

/*!
 * \brief The PhotoEditState class
 *
 * Edits that can be applied to a photo.  The default-constructed
 * PhotoEditState is considered the "original" edit state, which we use to mean
 * that the photo has no edits applied to it.
 */
class PhotoEditState
{
public:
    // An orientation outside the range [MIN_ORIENTATION,MAX_ORIENTATION] (also,
    // must match the DB's default orientation).
    static const Orientation ORIGINAL_ORIENTATION = (Orientation)0;

    Orientation orientation_;
    QRect crop_rectangle_;
    bool is_enhanced_;
    qreal exposureCompensation_;
    /// The color balance parameters are stored here in the order:
    /// brightness (x), contrast(y), saturation(z), hue(w)
    QVector4D colorBalance_;

    PhotoEditState() : orientation_(ORIGINAL_ORIENTATION), crop_rectangle_(),
        is_enhanced_(false), exposureCompensation_(0.0) {
    }

    bool is_original() const {
        return (orientation_ < MIN_ORIENTATION && !crop_rectangle_.isValid() &&
                !is_enhanced_ && exposureCompensation_ == 0.0 &&
                colorBalance_.isNull());
    }

    // Returns a new PhotoEditState the same as this one but rotated.  Needed
    // because rotating the crop_rectangle isn't trivial.  Note that image_width/
    // height must be specified in this PhotoEditState's orientation, not the
    // new_orientation.
    PhotoEditState rotate(Orientation new_orientation,
                          int image_width, int image_height) const {
        PhotoEditState new_state = *this;
        new_state.orientation_ = new_orientation;
        if (crop_rectangle_.isValid())
            new_state.crop_rectangle_ =
                    rotate_crop_rectangle(new_orientation, image_width, image_height);
        return new_state;
    }

    bool operator==(const PhotoEditState& other) {
        return (orientation_ == other.orientation_ &&
                crop_rectangle_ == other.crop_rectangle_ &&
                is_enhanced_ == other.is_enhanced_ &&
                exposureCompensation_ == other.exposureCompensation_ &&
                colorBalance_ == other.colorBalance_);
    }
    bool operator!=(const PhotoEditState& other) { return !(*this == other); }

private:
    QRect rotate_crop_rectangle(Orientation new_orientation,
                                int image_width, int image_height) const;
};

#endif
