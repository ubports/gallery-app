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
    EditStack() : base_(), undoable_(), redoable_() {
    }

    void set_base(const PhotoEditState& base) {
        base_ = base;
    }

    void push_edit(const PhotoEditState& edit_state) {
        clear_redos();
        undoable_.push(edit_state);
    }

    void clear_redos() {
        redoable_.clear();
    }

    const PhotoEditState& undo() {
        if (!undoable_.isEmpty())
            redoable_.push(undoable_.pop());
        return current();
    }

    const PhotoEditState& redo() {
        if (!redoable_.isEmpty())
            undoable_.push(redoable_.pop());
        return current();
    }

    const PhotoEditState& current() const {
        return (undoable_.isEmpty() ? base_ : undoable_.top());
    }

    int canUndo() const {
        return !undoable_.isEmpty();
    }

    int canRedo() const {
        return !redoable_.isEmpty();
    }

private:
    PhotoEditState base_; // What to return when we have no undo-able edits.
    QStack<PhotoEditState> undoable_;
    QStack<PhotoEditState> redoable_;
};


/*!
 * \brief The PhotoPrivate class implements some private data and functions for Photo
 */
class PhotoPrivate {
    PhotoPrivate(Photo* q=0)
        : edits_(0),
          q_ptr(q)
    {
    }
    ~PhotoPrivate()
    {
        delete edits_;
    }

    EditStack* editStack() const;
    void setBaseEditStack(const PhotoEditState& base);

private:
    mutable EditStack* edits_;

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
    if (edits_ == 0) {
        Q_Q(const Photo);
        edits_ = new EditStack;
        Database* database = GalleryManager::instance()->database();
        PhotoEditState editState = database->getPhotoEditTable()->editState(q->id());
        edits_->set_base(editState);
    }
    return edits_;
}

void PhotoPrivate::setBaseEditStack(const PhotoEditState &base)
{
    if(edits_ == 0) {
        edits_ = new EditStack;
    }

    edits_->set_base(base);
}


/*!
 * \brief Photo::IsValid
 * \param file
 * \return
 */
bool Photo::IsValid(const QFileInfo& file)
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
 * \brief Photo::Load Loads a photo object from the given file
 * Loads a photo object from the given file.  If it's not already
 * present in the database, it will be added.  If the file is not
 * valid return null.
 * \param file
 * \return
 */
Photo* Photo::Load(const QFileInfo& file)
{
    bool needs_update = false;
    QDateTime timestamp;
    QDateTime exposure_time;
    QSize size;
    Orientation orientation;
    qint64 filesize;

    // Look for photo in the database.
    MediaTable *mediaTable = GalleryManager::instance()->database()->getMediaTable();
    qint64 id = mediaTable->getIdForMedia(file.absoluteFilePath());

    if (id == INVALID_ID && !IsValid(file))
        return NULL;

    Photo* p = new Photo(file);

    // Check for legacy rows.
    if (id != INVALID_ID)
        needs_update = mediaTable->rowNeedsUpdate(id);

    // If we don't have the photo, add it to the DB.  If we have the photo but the
    // row is from a previous version of the DB, update the row.
    if (id == INVALID_ID || needs_update) {
        PhotoMetadata* metadata = PhotoMetadata::fromFile(p->caches_.pristine_file());
        if (metadata == NULL) {
            delete p;
            return NULL;
        }

        timestamp = p->caches_.pristine_file().lastModified();
        orientation = p->file_format_has_orientation()
                ? metadata->orientation() : TOP_LEFT_ORIGIN;
        filesize = p->caches_.pristine_file().size();
        exposure_time = metadata->exposureTime().isValid() ?
                    QDateTime(metadata->exposureTime()) : timestamp;

        if (needs_update) {
            // Update DB.
            mediaTable->updateMedia(id, file.absoluteFilePath(), timestamp, exposure_time,
                                    orientation, filesize);
        } else {
            // Add to DB.
            id = mediaTable->createIdForMedia(file.absoluteFilePath(), timestamp,
                                              exposure_time, orientation, filesize);
        }

        PhotoEditState edit_state;
        p->set_base_edit_state(edit_state);

        delete metadata;
    } else {
        // Load metadata from DB.
        mediaTable->getRow(id, size, orientation, timestamp, exposure_time);
    }

    // Populate photo object.
    if (size.isValid())
        p->setSize(size);
    p->set_original_orientation(orientation);
    p->setFileTimestamp(timestamp);
    p->setExposureDateTime(exposure_time);

    // We set the id last so we don't save the info we just read in back out to
    // the DB.
    p->setId(id);

    return p;
}

/*!
 * \brief Photo::Fetch Loads a photo object from the given file and generates a thumbnail for it
 * Loads a photo object from the given file and generates a thumbnail for it
 * if and only if it hasn't already been loaded; otherwise, it attempts to
 * return the existing object instead. Uses Photo.Load() to do its work.
 * \param file
 * \return
 */
Photo* Photo::Fetch(const QFileInfo& file)
{
    GalleryManager* gallery_mgr = GalleryManager::instance();

    Photo* p = 0;
    MediaSource* media = gallery_mgr->media_collection()->mediaFromFileinfo(file);
    if (media == 0) {
        p = Load(file);
    } else {
        p = qobject_cast<Photo*>(media);
    }

    return p;
}

/*!
 * \brief Photo::Photo
 * \param file
 */
Photo::Photo(const QFileInfo& file)
    : MediaSource(file),
      edit_revision_(0),
      caches_(file),
      original_size_(),
      original_orientation_(TOP_LEFT_ORIGIN),
      d_ptr(new PhotoPrivate(this))
{
    QByteArray format = QImageReader(file.filePath()).format();
    file_format_ = QString(format).toLower();
    if (file_format_ == "jpg") // Why does Qt expose two different names here?
        file_format_ = "jpeg";
}

/*!
 * \brief Photo::~Photo
 */
Photo::~Photo()
{
    delete(d_ptr);
}

/*!
 * \brief Photo::Image Returns the photo in full size.
 * \param respect_orientation if set to true, the photo is rotated according to the EXIF information
 * \return The image in full size
 */
QImage Photo::image(bool respect_orientation, const QSize &scaleSize)
{
    QImageReader imageReader(file().filePath(), file_format_.toStdString().c_str());
    QSize imageSize = imageReader.size();
    if (scaleSize.isValid()) {
        QSize size = imageSize;
        size.scale(scaleSize, Qt::KeepAspectRatioByExpanding);
        imageReader.setScaledSize(size);
    }
    QImage image = imageReader.read();
    if (!image.isNull() && respect_orientation && file_format_has_orientation()) {
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
    return (current_state().orientation_ == PhotoEditState::ORIGINAL_ORIENTATION) ?
                original_orientation_ : current_state().orientation_;
}

/*!
 * \reimp
 */
QUrl Photo::galleryPath() const
{
    QUrl url = MediaSource::galleryPath();
    // We don't pass the orientation in if we saved the file already rotated,
    // which is the case if the file format can't store rotation metadata.
    append_path_params(&url, (file_format_has_orientation() ? orientation() : TOP_LEFT_ORIGIN), 0);

    return url;
}

/*!
 * \reimp
 */
QUrl Photo::galleryPreviewPath() const
{
    QUrl url = MediaSource::galleryPreviewPath();
    // previews are always stored fully transformed

    append_path_params(&url, TOP_LEFT_ORIGIN, 1);

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
    if (edit_revision_ != 0) {
        query.addQueryItem(GalleryThumbnailImageProvider::REVISION_PARAM_NAME,
                           QString::number(edit_revision_));
    }
    url.setQuery(query);
    return url;
}

/*!
 * \brief Photo::set_base_edit_state
 * The "base state" is the PhotoEditState of the file when Gallery starts.
 * It's the bottom of the undo stack.  Comes from the DB.
 * \param base
 */
void Photo::set_base_edit_state(const PhotoEditState& base)
{
    Q_D(Photo);
    d->editStack()->set_base(base);
}

/*!
 * \brief Photo::revertToOriginal
 */
void Photo::revertToOriginal()
{
    if (!current_state().is_original())
        make_undoable_edit(PhotoEditState());
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
    return current_state().is_original();
}

/*!
 * \brief Photo::rotateRight
 */
void Photo::rotateRight()
{
    Orientation new_orientation =
            OrientationCorrection::rotateOrientation(orientation(), false);

    QSize size = get_original_size(orientation());

    // A PhotoEditState object with an invalid orientation value (i.e. <
    // MIN_ORIENTATION) means "use the existing (original) orientation", so
    // set the current edit state's orientation to this photo object's
    // orientation
    PhotoEditState curr_state = current_state();
    if (curr_state.orientation_ < MIN_ORIENTATION)
        curr_state.orientation_ = orientation();

    PhotoEditState next_state = curr_state.rotate(new_orientation, size.width(),
                                                  size.height());

    make_undoable_edit(next_state);
}

/*!
 * \brief Photo::autoEnhance
 */
void Photo::autoEnhance()
{
    if (current_state().is_enhanced_)
        return;

    PhotoEditState next_state = current_state();
    next_state.is_enhanced_ = true;

    make_undoable_edit(next_state);
}

/*!
 * \brief Photo::exposureCompensation Changes the brightnes of the image
 * \param value Value for the compensation. -1.0 moves the image into total black.
 * +1.0 to total white. 0.0 leaves it as it is.
 */
void Photo::exposureCompensation(qreal value)
{
    PhotoEditState next_state = current_state();
    next_state.exposureCompensation_ = value;
    make_undoable_edit(next_state);
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
    PhotoEditState next_state = current_state();
    next_state.colorBalance_ = QVector4D(brightness, contrast, saturation, hue);
    make_undoable_edit(next_state);
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
    if (current_state().crop_rectangle_.isValid()) {
        QSize image_size = get_original_size(current_state().orientation_);

        QRect rect = current_state().crop_rectangle_;
        qreal x = (qreal)rect.x() / image_size.width();
        qreal y = (qreal)rect.y() / image_size.height();
        qreal width = (qreal)rect.width() / image_size.width();
        qreal height = (qreal)rect.height() / image_size.height();

        if (x >= 0.0 && y >= 0.0 && width > 0.0 && height > 0.0 &&
                x + width <= 1.0 && y + height <= 1.0)
            ratio_crop_rect = QRectF(x, y, width, height);
    }

    PhotoEditState next_state = current_state();
    next_state.crop_rectangle_ = QRect();

    make_undoable_edit(next_state);

    return QVariant::fromValue(ratio_crop_rect);
}

/*!
 * \brief Photo::cancelCropping
 */
void Photo::cancelCropping()
{
    Q_D(Photo);
    undo();
    d->editStack()->clear_redos();
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

    QSize image_size = get_original_size(current_state().orientation_);

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

    PhotoEditState next_state = current_state();
    next_state.crop_rectangle_ = QRect(x, y, width, height);

    // We replace the top of the undo stack (which came from prepareForCropping)
    // with the cropped version.
    d->editStack()->undo();
    make_undoable_edit(next_state);
}

/*!
 * \brief Photo::DestroySource
 * \param destroy_backing
 * \param as_orphan
 */
void Photo::destroySource(bool destroy_backing, bool as_orphan)
{
    MediaSource::destroySource(destroy_backing, as_orphan);

    caches_.discard_all();
}

/*!
 * \brief Photo::current_state
 * \return
 */
const PhotoEditState& Photo::current_state() const
{
    Q_D(const Photo);
    return d->editStack()->current();
}

/*!
 * \brief Photo::get_original_size Returns the original image size translated to the desired orientation
 * \param orientation
 * \return Returns the original image size translated to the desired orientation
 */
QSize Photo::get_original_size(Orientation orientation)
{
    if (!original_size_.isValid()) {
        QImage original(caches_.pristine_file().filePath(),
                        file_format_.toStdString().c_str());
        if (file_format_has_orientation()) {
            original =
                    original.transformed(OrientationCorrection::fromOrientation(
                                             original_orientation_).toTransform());
        }

        original_size_ = original.size();
    }

    QSize rotated_size = original_size_;

    if (orientation != PhotoEditState::ORIGINAL_ORIENTATION) {
        OrientationCorrection original_correction =
                OrientationCorrection::fromOrientation(original_orientation_);
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
 * \brief Photo::make_undoable_edit
 * \param state
 */
void Photo::make_undoable_edit(const PhotoEditState& state)
{
    Q_D(Photo);
    Orientation old_orientation = orientation();

    d->editStack()->push_edit(state);
    save(state, old_orientation);
    emit editStackChanged();
}

/*!
 * \brief Photo::save
 * \param state
 * \param old_orientation
 */
void Photo::save(const PhotoEditState& state, Orientation old_orientation)
{
    edit_file(state);
    GalleryManager::instance()->database()->getPhotoEditTable()->setEditState(id(), state);

    if (orientation() != old_orientation)
        emit orientationChanged();
    notifyDataChanged();

    ++edit_revision_;

    emit galleryPathChanged();
    emit galleryPreviewPathChanged();
    emit galleryThumbnailPathChanged();
}

/*!
 * \brief Photo::handle_simple_metadata_rotation
 * Handler for the case of an image whose only change is to its
 * orientation; used to skip re-encoding of JPEGs.
 * \param state
 */
void Photo::handle_simple_metadata_rotation(const PhotoEditState& state)
{
    PhotoMetadata* metadata = PhotoMetadata::fromFile(file());
    metadata->setOrientation(state.orientation_);

    metadata->save();
    delete(metadata);

    OrientationCorrection orig_correction =
            OrientationCorrection::fromOrientation(original_orientation_);
    OrientationCorrection dest_correction =
            OrientationCorrection::fromOrientation(state.orientation_);

    QSize new_size = original_size_;
    int angle = dest_correction.getNormalizedRotationDifference(orig_correction);

    if ((angle == 90) || (angle == 270)) {
        new_size = original_size_.transposed();
    }

    setSize(new_size);
}

/*!
 * \brief Photo::edit_file
 * \param state
 */
void Photo::edit_file(const PhotoEditState& state)
{
    // As a special case, if editing to the original version, we simply restore
    // from the original and call it a day.
    if (state.is_original()) {
        if (!caches_.restore_original())
            qDebug("Error restoring original for %s", qPrintable(file().filePath()));
        else
            setSize(get_original_size(PhotoEditState::ORIGINAL_ORIENTATION));

        // As a courtesy, when the original goes away, we get rid of the other
        // cached files too.
        caches_.discard_cached_enhanced();
        return;
    }

    if (!caches_.cache_original())
        qDebug("Error caching original for %s", qPrintable(file().filePath()));

    if (state.is_enhanced_ && !caches_.has_cached_enhanced())
        create_cached_enhanced();

    if (!caches_.overwrite_from_cache(state.is_enhanced_))
        qDebug("Error overwriting %s from cache", qPrintable(file().filePath()));

    // Have we been rotated and _not_ cropped?
    if (file_format_has_orientation() && (!state.crop_rectangle_.isValid()) &&
            state.exposureCompensation_ == 0 &&
            (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)) {
        // Yes; skip out on decoding and re-encoding the image.
        handle_simple_metadata_rotation(state);
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

    QImage image(file().filePath(), file_format_.toStdString().c_str());
    if (image.isNull()) {
        qDebug("Error loading %s for editing", qPrintable(file().filePath()));
        return;
    }
    PhotoMetadata* metadata = PhotoMetadata::fromFile(file());

    if (file_format_has_orientation() &&
            state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION)
        metadata->setOrientation(state.orientation_);

    if (file_format_has_orientation() &&
            metadata->orientation() != TOP_LEFT_ORIGIN)
        image = image.transformed(metadata->orientationTransform());
    else if (state.orientation_ != PhotoEditState::ORIGINAL_ORIENTATION &&
             state.orientation_ != TOP_LEFT_ORIGIN)
        image = image.transformed(
                    OrientationCorrection::fromOrientation(state.orientation_).toTransform());

    // Cache this here so we may be able to avoid another JPEG decode later just
    // to find the dimensions.
    if (!original_size_.isValid())
        original_size_ = image.size();

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
    if (file_format_has_orientation() &&
            metadata->orientation() != TOP_LEFT_ORIGIN)
        image = image.transformed(metadata->orientationTransform().inverted());

    bool saved = image.save(file().filePath(), file_format_.toStdString().c_str(), 90);
    if (saved && file_format_has_metadata())
        saved = metadata->save();
    if (!saved)
        qDebug("Error saving edited %s", qPrintable(file().filePath()));

    delete metadata;

    setSize(new_size);
}

/*!
 * \brief Photo::create_cached_enhanced
 */
void Photo::create_cached_enhanced()
{
    if (!caches_.cache_enhanced_from_original()) {
        qDebug("Error creating enhanced file for %s", qPrintable(file().filePath()));
        return;
    }

    setBusy(true);

    QFileInfo to_enhance = caches_.enhanced_file();
    PhotoMetadata* metadata = PhotoMetadata::fromFile(to_enhance);

    QImage unenhanced_img(to_enhance.filePath(), file_format_.toStdString().c_str());
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
                                     file_format_.toStdString().c_str(), 99);
    if (saved && file_format_has_metadata())
        saved = metadata->save();
    if (!saved) {
        qDebug("Error saving enhanced file for %s", qPrintable(file().filePath()));
        caches_.discard_cached_enhanced();
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
 * \brief Photo::append_path_params is called by either gallery_path or gallery_preview_path depending on what kind of photo.
 * \brief This sets our size_level parameter which will dictate what sort of image is eventually created.
 * \param url is the picture's url.
 * \param orientation of the image.
 * \param size_level dictates whether or not the image is a full sized picture or a thumbnail. 0 == full sized, 1 == preview.
 */
void Photo::append_path_params(QUrl* url, Orientation orientation, const int size_level) const
{
    QUrlQuery query;
    query.addQueryItem(GalleryStandardImageProvider::SIZE_KEY, QString::number(size_level));
    query.addQueryItem(GalleryStandardImageProvider::ORIENTATION_PARAM_NAME, QString::number(orientation));

    // Because of QML's aggressive, opaque caching of loaded images, we need to
    // add an arbitrary URL parameter to gallery_path and gallery_preview_path so
    // that loading the same image after an edit will go back to disk instead of
    // just hitting the cache.
    if (edit_revision_ != 0) {
        query.addQueryItem(GalleryStandardImageProvider::REVISION_PARAM_NAME,
                           QString::number(edit_revision_));
    }

    url->setQuery(query);
}

/*!
 * \brief Photo::file_format_has_metadata
 * \return
 */
bool Photo::file_format_has_metadata() const
{
    return (file_format_ == "jpeg" || file_format_ == "tiff" ||
            file_format_ == "png");
}

/*!
 * \brief Photo::file_format_has_orientation
 * \return
 */
bool Photo::file_format_has_orientation() const
{
    return (file_format_ == "jpeg");
}

/*!
 * \brief Photo::set_original_orientation
 * \param orientation
 */
void Photo::set_original_orientation(Orientation orientation)
{
    original_orientation_ = orientation;
}
