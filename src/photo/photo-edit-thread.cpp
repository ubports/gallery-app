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

#include "photo-edit-thread.h"
#include "photo.h"

// medialoader
#include "photo-metadata.h"

// util
#include "imaging.h"

#include <QDebug>

/*!
 * \brief PhotoEditThread::PhotoEditThread
 */
PhotoEditThread::PhotoEditThread(Photo *photo, const PhotoEditState &editState)
    : QThread(),
      m_photo(photo),
      m_editState(editState),
      m_caches(photo->file()),
      m_oldOrientation(photo->orientation())
{
}

/*!
 * \brief PhotoEditThread::editState resturns the editing stse used for this processing
 * \return
 */
const PhotoEditState &PhotoEditThread::editState() const
{
    return m_editState;
}

/*!
 * \brief PhotoEditThread::oldOrientation returns the orientation of the photo before the editing
 * \return
 */
Orientation PhotoEditThread::oldOrientation() const
{
    return m_oldOrientation;
}

/*!
 * \brief PhotoEditThread::run \reimp
 */
void PhotoEditThread::run()
{
    // As a special case, if editing to the original version, we simply restore
    // from the original and call it a day.
    if (m_editState.isOriginal()) {
        if (!m_caches.restoreOriginal())
            qWarning() << "Error restoring original for" << m_photo->file().filePath();
        else
            Q_EMIT resetToOriginalSize();

        // As a courtesy, when the original goes away, we get rid of the other
        // cached files too.
        m_caches.discardCachedEnhanced();
        return;
    }

    if (!m_caches.cacheOriginal())
        qWarning() << "Error caching original for" << m_photo->file().filePath();

    if (m_editState.is_enhanced_ && !m_caches.hasCachedEnhanced())
        createCachedEnhanced();

    if (!m_caches.overwriteFromCache(m_editState.is_enhanced_))
        qWarning() << "Error overwriting" << m_photo->file().filePath() << "from cache";

    // Have we been rotated and _not_ cropped?
    if (m_photo->fileFormatHasOrientation() && (!m_editState.crop_rectangle_.isValid()) &&
            m_editState.exposureCompensation_ == 0 &&
            (m_editState.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)) {
        // Yes; skip out on decoding and re-encoding the image.
        handleSimpleMetadataRotation(m_editState);
        return;
    }

    // TODO: we might be able to avoid reading/writing pixel data (and other
    // more general optimizations) under certain conditions here.  Might be worth
    // doing if it doesn't make the code too much worse.
    //
    // At the moment, we are skipping at least one decode and one encode in cases
    // where a .jpeg file has been rotated, but not cropped, since rotation can be
    // controlled by manipulating its metadata without having to modify pixel data;
    // please see the method handle_simple_metadata_rotation() for details.

    QImage image(m_photo->file().filePath(), m_photo->fileFormat().toStdString().c_str());
    if (image.isNull()) {
        qWarning() << "Error loading" << m_photo->file().filePath() << "for editing";
        return;
    }
    PhotoMetadata* metadata = PhotoMetadata::fromFile(m_photo->file());

    if (m_photo->fileFormatHasOrientation() &&
            m_editState.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)
        metadata->setOrientation(m_editState.orientation_);

    if (m_photo->fileFormatHasOrientation() &&
            metadata->orientation() != TOP_LEFT_ORIGIN)
        image = image.transformed(metadata->orientationTransform());
    else if (m_editState.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION &&
             m_editState.orientation_ != TOP_LEFT_ORIGIN)
        image = image.transformed(
                    OrientationCorrection::fromOrientation(m_editState.orientation_).toTransform());

    if (m_editState.crop_rectangle_.isValid())
        image = image.copy(m_editState.crop_rectangle_);

    // exposure compensation
    if (m_editState.exposureCompensation_ != 0.0) {
        image = compensateExposure(image, m_editState.exposureCompensation_);
    }

    // exposure compensation
    if (!m_editState.colorBalance_.isNull()) {
        const QVector4D &v = m_editState.colorBalance_;
        image = doColorBalance(image, v.x(), v.y(), v.z(), v.w());
    }

    QSize new_size = image.size();

    // We need to apply the reverse transformation so that when we reload the
    // file and reapply the transformation it comes out correctly.
    if (m_photo->fileFormatHasOrientation() &&
            metadata->orientation() != TOP_LEFT_ORIGIN)
        image = image.transformed(metadata->orientationTransform().inverted());

    bool saved = image.save(m_photo->file().filePath(),  m_photo->fileFormat().toStdString().c_str(), 90);
    if (saved && m_photo->fileFormatHasMetadata()) {
        metadata->updateThumbnail(image);
        saved = metadata->save();
    }
    if (!saved)
        qWarning() << "Error saving edited" << m_photo->file().filePath();

    delete metadata;

    Q_EMIT newSize(new_size);
}

/*!
 * \brief PhotoEditThread::handleSimpleMetadataRotation
 * Handler for the case of an image whose only change is to its
 * orientation; used to skip re-encoding of JPEGs.
 * \param state
 */
void PhotoEditThread::handleSimpleMetadataRotation(const PhotoEditState& state)
{
    PhotoMetadata* metadata = PhotoMetadata::fromFile(m_photo->file());
    metadata->setOrientation(state.orientation_);

    metadata->save();
    delete(metadata);

    OrientationCorrection orig_correction =
            OrientationCorrection::fromOrientation(m_photo->originalOrientation());
    OrientationCorrection dest_correction =
            OrientationCorrection::fromOrientation(state.orientation_);

    QSize new_size = m_photo->originalSize();
    int angle = dest_correction.getNormalizedRotationDifference(orig_correction);

    if ((angle == 90) || (angle == 270)) {
        new_size = m_photo->originalSize().transposed();
    }

    Q_EMIT newSize(new_size);
}

/*!
 * \brief PhotoEditThread::createCachedEnhanced
 */
void PhotoEditThread::createCachedEnhanced()
{
    if (!m_caches.cacheEnhancedFromOriginal()) {
        qWarning() << "Error creating enhanced file for" << m_photo->file().filePath();
        return;
    }

    QFileInfo to_enhance = m_photo->enhancedFile();
    PhotoMetadata* metadata = PhotoMetadata::fromFile(to_enhance);

    QImage unenhanced_img(to_enhance.filePath(), m_photo->fileFormat().toStdString().c_str());
    int width = unenhanced_img.width();
    int height = unenhanced_img.height();

    QImage sample_img = (unenhanced_img.width() > 400) ?
                unenhanced_img.scaledToWidth(400) : unenhanced_img;

    AutoEnhanceTransformation enhance_txn = AutoEnhanceTransformation(sample_img);

    QImage::Format dest_format = unenhanced_img.format();

    // Can't write into indexed images, due to a limitation in Qt.
    if (dest_format == QImage::Format_Indexed8)
        dest_format = QImage::Format_RGB32;

    QImage enhanced_image(width, height, dest_format);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            QColor px = enhance_txn.transformPixel(
                        QColor(unenhanced_img.pixel(i, j)));
            enhanced_image.setPixel(i, j, px.rgb());
        }
    }

    bool saved = enhanced_image.save(to_enhance.filePath(),
                                     m_photo->fileFormat().toStdString().c_str(), 90);
    if (saved && m_photo->fileFormatHasMetadata())
        saved = metadata->save();
    if (!saved) {
        qWarning() << "Error saving enhanced file for" << m_photo->file().filePath();
        m_caches.discardCachedEnhanced();
    }

    delete metadata;
}

/*!
 * \brief PhotoEditThread::compensateExposure Compensates the exposure
 * Compensating the exposure is a change in brightnes
 * \param image Image to change the brightnes
 * \param compansation -1.0 is total dark, +1.0 is total bright
 * \return The image with adjusted brightnes
 */
QImage PhotoEditThread::compensateExposure(const QImage &image, qreal compansation)
{
    int shift = qBound(-255, (int)(255*compansation), 255);
    QImage result(image.width(), image.height(), image.format());

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i <image.width(); i++) {
            QColor px = image.pixel(i, j);
            int red = qBound(0, px.red() + shift, 255);
            int green = qBound(0, px.green() + shift, 255);
            int blue = qBound(0, px.blue() + shift, 255);
            result.setPixel(i, j, qRgb(red, green, blue));
        }
    }

    return result;
}

/*!
 * \brief PhotoEditThread::colorBalance
 * \param image
 * \param brightness 0 is total dark, 1 is as the original, grater than 1 is brigther
 * \param contrast from 0 maybe 5. 1 is as the original
 * \param saturation from 0 maybe 5. 1 is as the original
 * \param hue from 0 to 360. 0 and 360 is as the original
 * \return
 */
QImage PhotoEditThread::doColorBalance(const QImage &image, qreal brightness, qreal contrast, qreal saturation, qreal hue)
{
    QImage result(image.width(), image.height(), image.format());

    ColorBalance cb(brightness, contrast, saturation, hue);

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i <image.width(); i++) {
            QColor px = image.pixel(i, j);
            QColor tpx = cb.transformPixel(px);
            result.setPixel(i, j, tpx.rgb());
        }
    }

    return result;
}

