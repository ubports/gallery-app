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

class Event;
class GalleryManager;

/*!
 * \brief The MediaSource class
 */
class MediaSource : public DataSource
{
    Q_OBJECT
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
    Q_PROPERTY(QVariant event READ QmlFindEvent NOTIFY eventChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int width READ width NOTIFY sizeChanged)
    Q_PROPERTY(int height READ height NOTIFY sizeChanged)
    Q_PROPERTY(int maxSize READ maxSize NOTIFY maxSizeChanged)

signals:
    void pathChanged();
    void previewPathChanged();
    void thumbnailPathChanged();
    void galleryPathChanged();
    void galleryPreviewPathChanged();
    void galleryThumbnailPathChanged();
    void orientationChanged();
    void exposureDateTimeChanged();
    void eventChanged();
    void dataChanged();
    void sizeChanged();
    void busyChanged();
    void maxSizeChanged();

public:
    MediaSource();
    explicit MediaSource(const QFileInfo& file);

    QFileInfo file() const;
    QUrl path() const;
    virtual QUrl galleryPath() const;

    QString previewFile() const;
    QUrl previewPath() const;
    virtual QUrl galleryPreviewPath() const;

    QString thumbnailFile() const;
    QUrl thumbnailPath() const;
    virtual QUrl galleryThumbnailPath() const;

    virtual QImage image(bool respect_orientation = true, const QSize &scaleSize=QSize());
    virtual Orientation orientation() const;
    const QDateTime& exposureDateTime() const;
    const QDateTime& fileTimestamp() const;

    const QSize& size();
    QDate exposureDate() const;
    QTime exposureTimeOfDay() const;
    int exposureTime_t() const;

    Event* FindEvent();
    QVariant QmlFindEvent();

    qint64 id() const;

    bool busy() const;

    int maxSize() const;

protected:
    void setId(qint64 id);
    void setExposureDateTime(const QDateTime& exposureTime);
    void setFileTimestamp(const QDateTime& timestamp);
    void setSize(const QSize& size);
    bool isSizeSet() const;

    virtual void DestroySource(bool delete_backing, bool as_orphan);

    virtual void notify_data_altered();
    virtual void notify_size_altered();

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
