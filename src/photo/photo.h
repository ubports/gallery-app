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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

#ifndef GALLERY_PHOTO_H_
#define GALLERY_PHOTO_H_

// media
#include "media-source.h"

// util
#include "orientation.h"

/*!
 * \brief The Photo class
 */
class Photo : public MediaSource
{
    Q_OBJECT

    Q_PROPERTY(bool canBeEdited READ canBeEdited)
public:
    explicit Photo(const QFileInfo& file);
    virtual ~Photo();

    virtual MediaType type() const;

    static bool isValid(const QFileInfo& file);

    virtual Orientation orientation() const;

    bool canBeEdited() const;

    void setOriginalOrientation(Orientation orientation);
    Orientation originalOrientation() const;
    const QSize &originalSize();

    const QString &fileFormat() const;
    bool fileFormatHasMetadata() const;
    bool fileFormatHasOrientation() const;

protected:
    virtual void destroySource(bool destroyBacking, bool asOrphan);

private:
    void appendPathParams(QUrl* url, Orientation orientation, const int sizeLevel) const;

    QString m_fileFormat;

    // We cache this data to avoid an image read at various times.
    QSize m_originalSize;
    Orientation m_originalOrientation;
};

#endif  // GALLERY_PHOTO_H_
