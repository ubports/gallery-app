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
#include "photo-edit-thread.h"

// database
#include "database.h"
#include "media-table.h"
#include "photo-edit-table.h"

// media
#include "media-collection.h"

// medialoader
#include "gallery-standard-image-provider.h"
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

    return reader.canRead();
}

/*!
 * \brief Photo::Photo
 * \param file
 */
Photo::Photo(const QFileInfo& file)
    : MediaSource(file),
      m_editRevision(0),
      m_editThread(0),
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
    if (m_editThread) {
        m_editThread->wait();
        finishEditing();
    }
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
        if (!isSizeSet()) {
            setSize(image.size());
            qWarning() << "++++++++ setsize from Photo::image";
        }
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
    return MediaSource::galleryPreviewPath();
}

/*!
 * \reimp
 */
QUrl Photo::galleryThumbnailPath() const
{
    return MediaSource::galleryThumbnailPath();
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
    if (busy()) {
        qWarning() << "Don't start edit operation, while another one is running";
        return;
    }

    PhotoEditState prev = d->editStack()->current();
    PhotoEditState next = d->editStack()->undo();
    if (next != prev) {
        asyncEdit(next);
    }
}

/*!
 * \brief Photo::redo
 */
void Photo::redo()
{
    Q_D(Photo);
    if (busy()) {
        qWarning() << "Don't start edit operation, while another one is running";
        return;
    }

    PhotoEditState prev = d->editStack()->current();
    PhotoEditState next = d->editStack()->redo();
    if (next != prev) {
        asyncEdit(next);
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

bool Photo::canBeEdited() const
{
    return QImageWriter::supportedImageFormats().contains(m_fileFormat.toUtf8());
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
 * \brief Photo::resetToOriginalSize set the size to the one of the orifinal photo
 */
void Photo::resetToOriginalSize()
{
    qWarning() << "++++++++ setsize from Photo::resetToOriginalSize";
    setSize(originalSize(PhotoEditState::ORIGINAL_ORIENTATION));
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
    if (busy()) {
        qWarning() << "Don't start edit operation, while another one is running";
        return;
    }

    Q_D(Photo);
    d->editStack()->pushEdit(state);
    asyncEdit(state);
}

/*!
 * \brief Photo::asyncEdit does edit the photo according to the given state
 * in a background task
 * \param state the new editing state
 */
void Photo::asyncEdit(const PhotoEditState& state)
{
    setBusy(true);
    m_editThread = new PhotoEditThread(this, state);
    connect(m_editThread, SIGNAL(finished()), this, SLOT(finishEditing()));
    connect(m_editThread, SIGNAL(newSize(QSize)), this, SLOT(setSize(QSize)));
    connect(m_editThread, SIGNAL(resetToOriginalSize()), this, SLOT(resetToOriginalSize()));
    m_editThread->start();
}

/*!
 * \brief Photo::finishEditing do all the updates once the editing is done
 */
void Photo::finishEditing()
{
    if (!m_editThread || m_editThread->isRunning())
        return;

    const PhotoEditState &state = m_editThread->editState();
    GalleryManager::instance()->database()->getPhotoEditTable()->setEditState(id(), state);

    if (orientation() != m_editThread->oldOrientation())
        emit orientationChanged();
    notifyDataChanged();

    ++m_editRevision;

    emit galleryPathChanged();
    emit galleryPreviewPathChanged();
    emit galleryThumbnailPathChanged();
    m_editThread->deleteLater();
    m_editThread = 0;
    setBusy(false);
    emit editStackChanged();
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
 * \brief Photo::fileFormat returns the file format as QString
 * \return
 */
const QString &Photo::fileFormat() const
{
    return m_fileFormat;
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

/*!
 * \brief Photo::originalOrientation returns the original orientation
 * \return
 */
Orientation Photo::originalOrientation() const
{
    return m_originalOrientation;
}

/*!
 * \brief Photo::originalSize
 * \return
 */
const QSize &Photo::originalSize()
{
    originalSize(PhotoEditState::ORIGINAL_ORIENTATION);
    return m_originalSize;
}
