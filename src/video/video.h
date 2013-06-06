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

#ifndef GALLERY_VIDEO_H_
#define GALLERY_VIDEO_H_

#include <QFileInfo>

#include "media/media-source.h"

/*!
 * \brief The Video class represents one video file
 */
class Video : public MediaSource
{
    Q_OBJECT

public:
    explicit Video(const QFileInfo& file);

    virtual QImage Image(bool respect_orientation = true, const QSize &scaleSize=QSize());
    virtual QDateTime exposure_date_time() const;
};

#endif  // GALLERY_VIDEO_H_
