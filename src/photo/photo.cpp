/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 * Clint Rogers <clinton@yorba.org>
 */

#include "photo.h"
#include "photo-edit-state.h"

// database
#include "database.h"
#include "media-table.h"
#include "photo-edit-table.h"

// media
#include "media-collection.h"

// medialoader
#include "gallery-standard-image-provider.h"
#include "gallery-thumbnail-image-provider.h"
#include "photo-metadata.h"

// util
#include "imaging.h"

// src
#include "gallery-manager.h"

#include <QApplication>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QStack>

// A simple class for dealing with an undo-/redo-able stack of applied edits.
class EditStack {
public:
    EditStack() : m_base(), m_undoable(), m_redoable() {
    }

    void setBase(const PhotoEditState& base) {
        m_base = base;
    }

    void pushEdit(const PhotoEditState& edit_state) {
        clearRedos();
        m_undoable.push(edit_state);
    }

    void clearRedos() {
        m_redoable.clear();
    }

    const PhotoEditState& undo() {
        if (!m_undoable.isEmpty())
            m_redoable.push(m_undoable.pop());
        return current();
    }

    const PhotoEditState& redo() {
        if (!m_redoable.isEmpty())
            m_undoable.push(m_redoable.pop());
        return current();
    }

    const PhotoEditState& current() const {
        return (m_undoable.isEmpty() ? m_base : m_undoable.top());
    }

    int canUndo() const {
        return !m_undoable.isEmpty();
    }

    int canRedo() const {
        return !m_redoable.isEmpty();
    }

private:
    PhotoEditState m_base; // What to return when we have no undo-able edits.
    QStack<PhotoEditState> m_undoable;
    QStack<PhotoEditState> m_redoable;
};


/*!
 * \brief The PhotoPrivate class implements some private data and functions for Photo
 */
class PhotoPrivate {
    PhotoPrivate(Photo* q=0)
        : m_edits(0),
          q_ptr(q)
    {
    }
    ~PhotoPrivate()
    {
        delete m_edits;
    }

    EditStack* editStack() const;
    void setBaseEditStack(const PhotoEditState& base);

private:
    mutable EditStack* m_edits;

    Photo * const q_ptr;
    Q_DECLARE_PUBLIC(Photo)
};

/*!
 * \brief PhotoPrivate::editStack returns the editStack.
 * As the edit stack is lazy loaded, it might take a while to load the last state from the DB
 * \return the edit stack
 */
EditStack* PhotoPrivate::editStack() const
{
    if (m_edits == 0) {
        Q_Q(const Photo);
        m_edits = new EditStack;
        Database* database = GalleryManager::instance()->database();
        PhotoEditState editState = database->getPhotoEditTable()->editState(q->id());
        m_edits->setBase(editState);
    }
    return m_edits;
}

void PhotoPrivate::setBaseEditStack(const PhotoEditState &base)
{
    if(m_edits == 0) {
        m_edits = new EditStack;
    }

    m_edits->setBase(base);
}


/*!
 * \brief Photo::isValid
 * \param file
 * \return
 */
bool Photo::isValid(const QFileInfo& file)
{
    QImageReader reader(file.filePath());
    QByteArray format = reader.format();

    if (QString(format).toLower() == "tiff") {
        // QImageReader.canRead() will detect some raw files as readable TIFFs,
        // though QImage will fail to load them.
        QString extension = file.suffix().toLower();
        if (extension != "tiff" && extension != "tif")
            return false;
    }

    PhotoMetadata* tmp = PhotoMetadata::fromFile(file);
    if (tmp == NULL)
        return false;

    delete tmp;
    return reader.canRead() &&
            QImageWriter::supportedImageFormats().contains(reader.format());
}

/*!
 * \brief Photo::Photo
 * \param file
 */
Photo::Photo(const QFileInfo& file)
    : MediaSource(file),
      m_editRevision(0),
      m_caches(file),
      m_originalSize(),
      m_originalOrientation(TOP_LEFT_ORIGIN),
      d_ptr(new PhotoPrivate(this))
{
    QByteArray format = QImageReader(file.filePath()).format();
    m_fileFormat = QString(format).toLower();
    if (m_fileFormat == "jpg") // Why does Qt expose two different names here?
        m_fileFormat = "jpeg";
}

/*!
 * \brief Photo::~Photo
 */
Photo::~Photo()
{
    delete(d_ptr);
}

/*!
 * \reimp
 */
MediaSource::MediaType Photo::type() const
{
    return MediaSource::Photo;
}

/*!
 * \brief Photo::image Returns the photo in full size.
 * \param respectOrientation if set to true, the photo is rotated according to the EXIF information
 * \return The image in full size
 */
QImage Photo::image(bool respectOrientation, const QSize &scaleSize)
{
    QImageReader imageReader(file().filePath(), m_fileFormat.toStdString().c_str());
    QSize imageSize = imageReader.size();
    if (scaleSize.isValid()) {
        QSize size = imageSize;
        size.scale(scaleSize, Qt::KeepAspectRatioByExpanding);
        imageReader.setScaledSize(size);
    }
    QImage image = imageReader.read();
    if (!image.isNull() && respectOrientation && fileFormatHasOrientation()) {
        image = image.transformed(
                    OrientationCorrection::fromOrientation(orientation())
                    .toTransform());

        // Cache this here since the image is already loaded.
        if (!isSizeSet())
            setSize(image.size());
    }

    return image;
}

/*!
 * \brief Photo::orientation
 * \return
 */
Orientation Photo::orientation() const
{
    return (currentState().orientation_ == PhotoEditState::ORIGINAL_ORIENTATION) ?
                m_originalOrientation : currentState().orientation_;
}

/*!
 * \reimp
 */
QUrl Photo::galleryPath() const
{
    QUrl url = MediaSource::galleryPath();
    // We don't pass the orientation in if we saved the file already rotated,
    // which is the case if the file format can't store rotation metadata.
    appendPathParams(&url, (fileFormatHasOrientation() ? orientation() : TOP_LEFT_ORIGIN), 0);

    return url;
}

/*!
 * \reimp
 */
QUrl Photo::galleryPreviewPath() const
{
    QUrl url = MediaSource::galleryPreviewPath();
    // previews are always stored fully transformed

    appendPathParams(&url, TOP_LEFT_ORIGIN, 1);

    return url;
}

/*!
 * \reimp
 */
QUrl Photo::galleryThumbnailPath() const
{
    QUrl url = MediaSource::galleryThumbnailPath();
    // same as in append_path_params() this is needed to trigger an update of the image in QML
    // so the URL is changed by adding/chageing the edit parameter
    QUrlQuery query;
    if (m_editRevision != 0) {
        query.addQueryItem(GalleryThumbnailImageProvider::REVISION_PARAM_NAME,
                           QString::number(m_editRevision));
    }
    url.setQuery(query);
    return url;
}

/*!
 * \brief Photo::setBaseEditState
 * The "base state" is the PhotoEditState of the file when Gallery starts.
 * It's the bottom of the undo stack.  Comes from the DB.
 * \param base
 */
void Photo::setBaseEditState(const PhotoEditState& base)
{
    Q_D(Photo);
    d->editStack()->setBase(base);
}

/*!
 * \brief Photo::originalFile
 * \return
 */
const QFileInfo &Photo::originalFile() const
{
    return m_caches.originalFile();
}

/*!
 * \brief Photo::enhancedFile
 * \return
 */
const QFileInfo &Photo::enhancedFile() const
{
    return m_caches.enhancedFile();
}

/*!
 * \brief Photo::pristineFile
 * \return
 */
const QFileInfo &Photo::pristineFile() const
{
    return m_caches.pristineFile();
}

/*!
 * \brief Photo::revertToOriginal
 */
void Photo::revertToOriginal()
{
    if (!currentState().isOriginal())
        makeUndoableEdit(PhotoEditState());
}

/*!
 * \brief Photo::undo
 */
void Photo::undo()
{
    Q_D(Photo);
    Orientation old_orientation = orientation();

    PhotoEditState prev = d->editStack()->current();
    PhotoEditState next = d->editStack()->undo();
    if (next != prev) {
        save(next, old_orientation);
        emit editStackChanged();
    }
}

/*!
 * \brief Photo::redo
 */
void Photo::redo()
{
    Q_D(Photo);
    Orientation old_orientation = orientation();

    PhotoEditState prev = d->editStack()->current();
    PhotoEditState next = d->editStack()->redo();
    if (next != prev) {
        save(next, old_orientation);
        emit editStackChanged();
    }
}

/*!
 * \brief Photo::canUndo
 */
bool Photo::canUndo() const
{
    Q_D(const Photo);
    return d->editStack()->canUndo();
}

/*!
 * \brief Photo::canRedo
 */
bool Photo::canRedo() const
{
    Q_D(const Photo);
    return d->editStack()->canRedo();
}

/*!
 * \brief Photo::isOriginal
 */
bool Photo::isOriginal() const
{
    return currentState().isOriginal();
}

/*!
 * \brief Photo::rotateRight
 */
void Photo::rotateRight()
{
    Orientation new_orientation =
            OrientationCorrection::rotateOrientation(orientation(), false);

    QSize size = originalSize(orientation());

    // A PhotoEditState object with an invalid orientation value (i.e. <
    // MIN_ORIENTATION) means "use the existing (original) orientation", so
    // set the current edit state's orientation to this photo object's
    // orientation
    PhotoEditState curr_state = currentState();
    if (curr_state.orientation_ < MIN_ORIENTATION)
        curr_state.orientation_ = orientation();

    PhotoEditState next_state = curr_state.rotate(new_orientation, size.width(),
                                                  size.height());

    makeUndoableEdit(next_state);
}

/*!
 * \brief Photo::autoEnhance
 */
void Photo::autoEnhance()
{
    if (currentState().is_enhanced_)
        return;

    PhotoEditState next_state = currentState();
    next_state.is_enhanced_ = true;

    makeUndoableEdit(next_state);
}

/*!
 * \brief Photo::exposureCompensation Changes the brightnes of the image
 * \param value Value for the compensation. -1.0 moves the image into total black.
 * +1.0 to total white. 0.0 leaves it as it is.
 */
void Photo::exposureCompensation(qreal value)
{
    PhotoEditState next_state = currentState();
    next_state.exposureCompensation_ = value;
    makeUndoableEdit(next_state);
}

/*!
 * \brief Photo::colorBalance adjusts the colors
 * \param brightness 0 is total dark, 1 is as the original, grater than 1 is brigther
 * \param contrast from 0 maybe 5. 1 is as the original
 * \param saturation from 0 maybe 5. 1 is as the original
 * \param hue from 0 to 360. 0 and 360 is as the original
 */
void Photo::colorBalance(qreal brightness, qreal contrast, qreal saturation, qreal hue)
{
    PhotoEditState next_state = currentState();
    next_state.colorBalance_ = QVector4D(brightness, contrast, saturation, hue);
    makeUndoableEdit(next_state);
}

/*!
 * \brief Photo::prepareForCropping
 * Edits the image to original size so you can recrop it.  Returns crop
 * coords in [0,1].  Should be followed by either cancelCropping() or crop().
 *
 * TODO: We COULD optimize this out if it hasn't been cropped yet, but I rely on
 *      this creating an undoable edit (to make the cancel button in the crop tool
 *      function correctly) so we do an edit anyway.
 * \return
 */
QVariant Photo::prepareForCropping()
{
    QRectF ratio_crop_rect(0.0, 0.0, 1.0, 1.0);
    if (currentState().crop_rectangle_.isValid()) {
        QSize image_size = originalSize(currentState().orientation_);

        QRect rect = currentState().crop_rectangle_;
        qreal x = (qreal)rect.x() / image_size.width();
        qreal y = (qreal)rect.y() / image_size.height();
        qreal width = (qreal)rect.width() / image_size.width();
        qreal height = (qreal)rect.height() / image_size.height();

        if (x >= 0.0 && y >= 0.0 && width > 0.0 && height > 0.0 &&
                x + width <= 1.0 && y + height <= 1.0)
            ratio_crop_rect = QRectF(x, y, width, height);
    }

    PhotoEditState next_state = currentState();
    next_state.crop_rectangle_ = QRect();

    makeUndoableEdit(next_state);

    return QVariant::fromValue(ratio_crop_rect);
}

/*!
 * \brief Photo::cancelCropping
 */
void Photo::cancelCropping()
{
    Q_D(Photo);
    undo();
    d->editStack()->clearRedos();
}

/*!
 * \brief Photo::crop
 * You should call prepareForCropping() before calling this.  Specify all
 * coords in [0,1].
 * \param vrect
 */
void Photo::crop(QVariant vrect)
{
    Q_D(Photo);
    QRectF ratio_crop_rect = vrect.toRectF();

    QSize image_size = originalSize(currentState().orientation_);

    // Integer truncation is good enough here.
    int x = ratio_crop_rect.x() * image_size.width();
    int y = ratio_crop_rect.y() * image_size.height();
    int width = ratio_crop_rect.width() * image_size.width();
    int height = ratio_crop_rect.height() * image_size.height();

    if (x < 0 || y < 0 || width <= 0 || height <= 0
            || x + width > image_size.width() || y + height > image_size.height()) {
        qDebug("Invalid cropping rectangle");
        undo(); // Go back to the state before prepareForCropping() was called.
        return;
    }

    PhotoEditState next_state = currentState();
    next_state.crop_rectangle_ = QRect(x, y, width, height);

    // We replace the top of the undo stack (which came from prepareForCropping)
    // with the cropped version.
    d->editStack()->undo();
    makeUndoableEdit(next_state);
}

/*!
 * \brief Photo::DestroySource
 * \param destroyBacking
 * \param asOrphan
 */
void Photo::destroySource(bool destroyBacking, bool asOrphan)
{
    MediaSource::destroySource(destroyBacking, asOrphan);

    m_caches.discardAll();
}

/*!
 * \brief Photo::currentState
 * \return
 */
const PhotoEditState& Photo::currentState() const
{
    Q_D(const Photo);
    return d->editStack()->current();
}

/*!
 * \brief Photo::originalSize Returns the original image size translated to the desired orientation
 * \param orientation
 * \return Returns the original image size translated to the desired orientation
 */
QSize Photo::originalSize(Orientation orientation)
{
    if (!m_originalSize.isValid()) {
        QImage original(m_caches.pristineFile().filePath(),
                        m_fileFormat.toStdString().c_str());
        if (fileFormatHasOrientation()) {
            original =
                    original.transformed(OrientationCorrection::fromOrientation(
                                             m_originalOrientation).toTransform());
        }

        m_originalSize = original.size();
    }

    QSize rotated_size = m_originalSize;

    if (orientation != PhotoEditState::ORIGINAL_ORIENTATION) {
        OrientationCorrection original_correction =
                OrientationCorrection::fromOrientation(m_originalOrientation);
        OrientationCorrection out_correction =
                OrientationCorrection::fromOrientation(orientation);
        int degrees_rotation =
                original_correction.getNormalizedRotationDifference(out_correction);

        if (degrees_rotation == 90 || degrees_rotation == 270)
            rotated_size.transpose();
    }

    return rotated_size;
}

/*!
 * \brief Photo::makeUndoableEdit
 * \param state
 */
void Photo::makeUndoableEdit(const PhotoEditState& state)
{
    Q_D(Photo);
    Orientation old_orientation = orientation();

    d->editStack()->pushEdit(state);
    save(state, old_orientation);
    emit editStackChanged();
}

/*!
 * \brief Photo::save
 * \param state
 * \param oldOrientation
 */
void Photo::save(const PhotoEditState& state, Orientation oldOrientation)
{
    editFile(state);
    GalleryManager::instance()->database()->getPhotoEditTable()->setEditState(id(), state);

    if (orientation() != oldOrientation)
        emit orientationChanged();
    notifyDataChanged();

    ++m_editRevision;

    emit galleryPathChanged();
    emit galleryPreviewPathChanged();
    emit galleryThumbnailPathChanged();
}

/*!
 * \brief Photo::handleSimpleMetadataRotation
 * Handler for the case of an image whose only change is to its
 * orientation; used to skip re-encoding of JPEGs.
 * \param state
 */
void Photo::handleSimpleMetadataRotation(const PhotoEditState& state)
{
    PhotoMetadata* metadata = PhotoMetadata::fromFile(file());
    metadata->setOrientation(state.orientation_);

    metadata->save();
    delete(metadata);

    OrientationCorrection orig_correction =
            OrientationCorrection::fromOrientation(m_originalOrientation);
    OrientationCorrection dest_correction =
            OrientationCorrection::fromOrientation(state.orientation_);

    QSize new_size = m_originalSize;
    int angle = dest_correction.getNormalizedRotationDifference(orig_correction);

    if ((angle == 90) || (angle == 270)) {
        new_size = m_originalSize.transposed();
    }

    setSize(new_size);
}

/*!
 * \brief Photo::editFile
 * \param state
 */
void Photo::editFile(const PhotoEditState& state)
{
    // As a special case, if editing to the original version, we simply restore
    // from the original and call it a day.
    if (state.isOriginal()) {
        if (!m_caches.restoreOriginal())
            qDebug("Error restoring original for %s", qPrintable(file().filePath()));
        else
            setSize(originalSize(PhotoEditState::ORIGINAL_ORIENTATION));

        // As a courtesy, when the original goes away, we get rid of the other
        // cached files too.
        m_caches.discardCachedEnhanced();
        return;
    }

    if (!m_caches.cacheOriginal())
        qDebug("Error caching original for %s", qPrintable(file().filePath()));

    if (state.is_enhanced_ && !m_caches.hasCachedEnhanced())
        createCachedEnhanced();

    if (!m_caches.overwriteFromCache(state.is_enhanced_))
        qDebug("Error overwriting %s from cache", qPrintable(file().filePath()));

    // Have we been rotated and _not_ cropped?
    if (fileFormatHasOrientation() && (!state.crop_rectangle_.isValid()) &&
            state.exposureCompensation_ == 0 &&
            (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)) {
        // Yes; skip out on decoding and re-encoding the image.
        handleSimpleMetadataRotation(state);
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

    QImage image(file().filePath(), m_fileFormat.toStdString().c_str());
    if (image.isNull()) {
        qDebug("Error loading %s for editing", qPrintable(file().filePath()));
        return;
    }
    PhotoMetadata* metadata = PhotoMetadata::fromFile(file());

    if (fileFormatHasOrientation() &&
            state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)
        metadata->setOrientation(state.orientation_);

    if (fileFormatHasOrientation() &&
            metadata->orientation() != TOP_LEFT_ORIGIN)
        image = image.transformed(metadata->orientationTransform());
    else if (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION &&
             state.orientation_ != TOP_LEFT_ORIGIN)
        image = image.transformed(
                    OrientationCorrection::fromOrientation(state.orientation_).toTransform());

    // Cache this here so we may be able to avoid another JPEG decode later just
    // to find the dimensions.
    if (!m_originalSize.isValid())
        m_originalSize = image.size();

    if (state.crop_rectangle_.isValid())
        image = image.copy(state.crop_rectangle_);

    // exposure compensation
    if (state.exposureCompensation_ != 0.0) {
        image = compensateExposure(image, state.exposureCompensation_);
    }

    // exposure compensation
    if (!state.colorBalance_.isNull()) {
        const QVector4D &v = state.colorBalance_;
        image = doColorBalance(image, v.x(), v.y(), v.z(), v.w());
    }

    QSize new_size = image.size();

    // We need to apply the reverse transformation so that when we reload the
    // file and reapply the transformation it comes out correctly.
    if (fileFormatHasOrientation() &&
            metadata->orientation() != TOP_LEFT_ORIGIN)
        image = image.transformed(metadata->orientationTransform().inverted());

    bool saved = image.save(file().filePath(), m_fileFormat.toStdString().c_str(), 90);
    if (saved && fileFormatHasMetadata())
        saved = metadata->save();
    if (!saved)
        qDebug("Error saving edited %s", qPrintable(file().filePath()));

    delete metadata;

    setSize(new_size);
}

/*!
 * \brief Photo::createCachedEnhanced
 */
void Photo::createCachedEnhanced()
{
    if (!m_caches.cacheEnhancedFromOriginal()) {
        qDebug("Error creating enhanced file for %s", qPrintable(file().filePath()));
        return;
    }

    setBusy(true);

    QFileInfo to_enhance = m_caches.enhancedFile();
    PhotoMetadata* metadata = PhotoMetadata::fromFile(to_enhance);

    QImage unenhanced_img(to_enhance.filePath(), m_fileFormat.toStdString().c_str());
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
        QApplication::processEvents();
        for (int i = 0; i < width; i++) {
            QColor px = enhance_txn.transformPixel(
                        QColor(unenhanced_img.pixel(i, j)));
            enhanced_image.setPixel(i, j, px.rgb());
        }
    }

    bool saved = enhanced_image.save(to_enhance.filePath(),
                                     m_fileFormat.toStdString().c_str(), 99);
    if (saved && fileFormatHasMetadata())
        saved = metadata->save();
    if (!saved) {
        qDebug("Error saving enhanced file for %s", qPrintable(file().filePath()));
        m_caches.discardCachedEnhanced();
    }

    setBusy(false);

    delete metadata;
}

/*!
 * \brief Photo::compensateExposure Compensates the exposure
 * Compensating the exposure is a change in brightnes
 * \param image Image to change the brightnes
 * \param compansation -1.0 is total dark, +1.0 is total bright
 * \return The image with adjusted brightnes
 */
QImage Photo::compensateExposure(const QImage &image, qreal compansation)
{
    setBusy(true);

    int shift = qBound(-255, (int)(255*compansation), 255);
    QImage result(image.width(), image.height(), image.format());

    for (int j = 0; j < image.height(); j++) {
        QApplication::processEvents();
        for (int i = 0; i <image.width(); i++) {
            QColor px = image.pixel(i, j);
            int red = qBound(0, px.red() + shift, 255);
            int green = qBound(0, px.green() + shift, 255);
            int blue = qBound(0, px.blue() + shift, 255);
            result.setPixel(i, j, qRgb(red, green, blue));
        }
    }

    setBusy(false);
    return result;
}

/*!
 * \brief Photo::colorBalance
 * \param image
 * \param brightness 0 is total dark, 1 is as the original, grater than 1 is brigther
 * \param contrast from 0 maybe 5. 1 is as the original
 * \param saturation from 0 maybe 5. 1 is as the original
 * \param hue from 0 to 360. 0 and 360 is as the original
 * \return
 */
QImage Photo::doColorBalance(const QImage &image, qreal brightness, qreal contrast, qreal saturation, qreal hue)
{
    setBusy(true);
    QImage result(image.width(), image.height(), image.format());

    ColorBalance cb(brightness, contrast, saturation, hue);

    for (int j = 0; j < image.height(); j++) {
        QApplication::processEvents();
        for (int i = 0; i <image.width(); i++) {
            QColor px = image.pixel(i, j);
            QColor tpx = cb.transformPixel(px);
            result.setPixel(i, j, tpx.rgb());
        }
    }

    setBusy(false);
    return result;
}

/*!
 * \brief Photo::appendPathParams is called by either gallery_path or gallery_preview_path depending on what kind of photo.
 * \brief This sets our size_level parameter which will dictate what sort of image is eventually created.
 * \param url is the picture's url.
 * \param orientation of the image.
 * \param sizeLevel dictates whether or not the image is a full sized picture or a thumbnail. 0 == full sized, 1 == preview.
 */
void Photo::appendPathParams(QUrl* url, Orientation orientation, const int sizeLevel) const
{
    QUrlQuery query;
    query.addQueryItem(GalleryStandardImageProvider::SIZE_KEY, QString::number(sizeLevel));
    query.addQueryItem(GalleryStandardImageProvider::ORIENTATION_PARAM_NAME, QString::number(orientation));

    // Because of QML's aggressive, opaque caching of loaded images, we need to
    // add an arbitrary URL parameter to gallery_path and gallery_preview_path so
    // that loading the same image after an edit will go back to disk instead of
    // just hitting the cache.
    if (m_editRevision != 0) {
        query.addQueryItem(GalleryStandardImageProvider::REVISION_PARAM_NAME,
                           QString::number(m_editRevision));
    }

    url->setQuery(query);
}

/*!
 * \brief Photo::fileFormatHasMetadata
 * \return
 */
bool Photo::fileFormatHasMetadata() const
{
    return (m_fileFormat == "jpeg" || m_fileFormat == "tiff" ||
            m_fileFormat == "png");
}

/*!
 * \brief Photo::fileFormatHasOrientation
 * \return
 */
bool Photo::fileFormatHasOrientation() const
{
    return (m_fileFormat == "jpeg");
}

/*!
 * \brief Photo::setOriginalOrientation
 * \param orientation
 */
void Photo::setOriginalOrientation(Orientation orientation)
{
    m_originalOrientation = orientation;
}
