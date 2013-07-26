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

#ifndef GALLERY_PHOTO_EDIT_THREAD_H_
#define GALLERY_PHOTO_EDIT_THREAD_H_

#include "photo-caches.h"
#include "photo-edit-state.h"

// util
#include "orientation.h"

#include <QImage>
#include <QThread>
#include <QUrl>

class Photo;

/*!
 * \brief The PhotoEditThread class
 */
class PhotoEditThread: public QThread
{
    Q_OBJECT
public:
    PhotoEditThread(Photo *photo, const PhotoEditState& editState);

    const PhotoEditState& editState() const;
    Orientation oldOrientation() const;

Q_SIGNALS:
    void newSize(QSize size);
    void resetToOriginalSize();

protected:
    void run() Q_DECL_OVERRIDE;

private:
    void createCachedEnhanced();
    QImage compensateExposure(const QImage& image, qreal compansation);
    QImage doColorBalance(const QImage& image, qreal brightness, qreal contrast, qreal saturation, qreal hue);
    void handleSimpleMetadataRotation(const PhotoEditState& state);

    Photo *m_photo;
    PhotoEditState m_editState;
    PhotoCaches m_caches;
    Orientation m_oldOrientation;
};

#endif
