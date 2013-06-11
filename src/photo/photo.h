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

#include "photo-caches.h"

// media
#include "media-source.h"

// util
#include "orientation.h"

class PhotoEditState;
class PhotoPrivate;

/*!
 * \brief The Photo class
 */
class Photo : public MediaSource
{
    Q_OBJECT

    Q_PROPERTY(bool canUndo READ canUndo NOTIFY editStackChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY editStackChanged)
    Q_PROPERTY(bool isOriginal READ isOriginal NOTIFY editStackChanged)

public:
    static bool isValid(const QFileInfo& file);

    static Photo* load(const QFileInfo& file);

    static Photo* fetch(const QFileInfo& file);

    virtual ~Photo();

    virtual QImage image(bool respect_orientation, const QSize &scaleSize=QSize());
    virtual Orientation orientation() const;

    virtual QUrl galleryPath() const;
    virtual QUrl galleryPreviewPath() const;
    virtual QUrl galleryThumbnailPath() const;

    void setBaseEditState(const PhotoEditState& base);

    bool canUndo() const;
    bool canRedo() const;
    bool isOriginal() const;

    Q_INVOKABLE void revertToOriginal();
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void rotateRight();
    Q_INVOKABLE void autoEnhance();
    Q_INVOKABLE void exposureCompensation(qreal value);
    Q_INVOKABLE void colorBalance(qreal brightness, qreal contrast, qreal saturation, qreal hue);
    Q_INVOKABLE QVariant prepareForCropping();
    Q_INVOKABLE void cancelCropping();
    Q_INVOKABLE void crop(QVariant vrect);

signals:
    void editStackChanged();

protected:
    virtual void destroySource(bool destroyBacking, bool asOrphan);

private:
    explicit Photo(const QFileInfo& file);

    const PhotoEditState& currentState() const;
    QSize originalSize(Orientation orientation);
    void makeUndoableEdit(const PhotoEditState& state);
    void save(const PhotoEditState& state, Orientation oldOrientation);
    void editFile(const PhotoEditState& state);
    void createCachedEnhanced();
    QImage compensateExposure(const QImage& image, qreal compansation);
    QImage doColorBalance(const QImage& image, qreal brightness, qreal contrast, qreal saturation, qreal hue);
    void appendPathParams(QUrl* url, Orientation orientation, const int sizeLevel) const;
    void handleSimpleMetadataRotation(const PhotoEditState& state);
    bool fileFormatHasMetadata() const;
    bool fileFormatHasOrientation() const;
    void setOriginalOrientation(Orientation orientation);

    QString m_fileFormat;
    int m_editRevision; // How many times the pixel data has been modified by us.
    PhotoCaches m_caches;

    // We cache this data to avoid an image read at various times.
    QSize m_originalSize;
    Orientation m_originalOrientation;

    PhotoPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Photo)
};

#endif  // GALLERY_PHOTO_H_
