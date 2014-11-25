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
