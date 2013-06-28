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
 */

#ifndef GALLERY_MEDIA_SOURCE_H_
#define GALLERY_MEDIA_SOURCE_H_

// core
#include "data-source.h"

// util
#include "orientation.h"

#include <QDate>
#include <QDateTime>
#include <QFileInfo>
#include <QImage>
#include <QSize>
#include <QTime>
#include <QUrl>
#include <QVariant>
#include <QtQml>

class GalleryManager;

/*!
 * \brief The MediaSource class
 */
class MediaSource : public DataSource
{
    Q_OBJECT
    Q_PROPERTY(MediaType type READ type NOTIFY typeChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(QUrl previewPath READ previewPath NOTIFY previewPathChanged)
    Q_PROPERTY(QUrl thumbnailPath READ thumbnailPath NOTIFY thumbnailPathChanged)
    Q_PROPERTY(QUrl galleryPath READ galleryPath NOTIFY galleryPathChanged)
    Q_PROPERTY(QUrl galleryPreviewPath READ galleryPreviewPath NOTIFY galleryPreviewPathChanged)
    Q_PROPERTY(QUrl galleryThumbnailPath READ galleryThumbnailPath NOTIFY galleryThumbnailPathChanged)
    Q_PROPERTY(int orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(QDate exposureDate READ exposureDate NOTIFY exposureDateTimeChanged)
    Q_PROPERTY(QTime exposureTimeOfDay READ exposureTimeOfDay NOTIFY exposureDateTimeChanged)
    Q_PROPERTY(int exposureTime_t READ exposureTime_t NOTIFY exposureDateTimeChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int width READ width NOTIFY sizeChanged)
    Q_PROPERTY(int height READ height NOTIFY sizeChanged)
    Q_ENUMS(MediaType)

signals:
    void typeChanged();
    void pathChanged();
    void previewPathChanged();
    void thumbnailPathChanged();
    void galleryPathChanged();
    void galleryPreviewPathChanged();
    void galleryThumbnailPathChanged();
    void orientationChanged();
    void exposureDateTimeChanged();
    void dataChanged();
    void sizeChanged();
    void busyChanged();

public:
    MediaSource();
    explicit MediaSource(const QFileInfo& file);

    enum MediaType {
        None = 0,
        Photo = 1,
        Video = 2
    };

    virtual MediaType type() const;

    QFileInfo file() const;
    QUrl path() const;
    virtual QUrl galleryPath() const;

    QString previewFile() const;
    QUrl previewPath() const;
    virtual QUrl galleryPreviewPath() const;

    QString thumbnailFile() const;
    QUrl thumbnailPath() const;
    virtual QUrl galleryThumbnailPath() const;

    virtual QImage image(bool respectOrientation = true, const QSize &scaleSize=QSize());
    virtual Orientation orientation() const;

    const QDateTime& exposureDateTime() const;
    QDate exposureDate() const;
    QTime exposureTimeOfDay() const;
    int exposureTime_t() const;
    void setExposureDateTime(const QDateTime& exposureTime);

    const QDateTime& fileTimestamp() const;
    void setFileTimestamp(const QDateTime& timestamp);

    const QSize& size();
    void setSize(const QSize& size);

    qint64 id() const;
    void setId(qint64 id);

    bool busy() const;

protected:
    bool isSizeSet() const;

    virtual void destroySource(bool deleteBacking, bool asOrphan);

    virtual void notifyDataChanged();
    virtual void notifySizeChanged();

    void setBusy(bool busy);

private:
    int width() const {
        return m_size.width();
    }

    int height() const {
        return m_size.height();
    }

    QFileInfo m_file;
    qint64 m_id;
    QSize m_size;
    QDateTime m_exposureDateTime;
    QDateTime m_fileTimestamp;
    bool m_busy;
};

QML_DECLARE_TYPE(MediaSource)

#endif  // GALLERY_MEDIA_SOURCE_H_
