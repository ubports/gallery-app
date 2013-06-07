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
    Q_PROPERTY(QUrl path READ path NOTIFY path_altered)
    Q_PROPERTY(QUrl previewPath READ preview_path NOTIFY preview_path_altered)
    Q_PROPERTY(QUrl thumbnailPath READ thumbnail_path NOTIFY thumbnail_path_altered)
    Q_PROPERTY(QUrl galleryPath READ gallery_path NOTIFY gallery_path_altered)
    Q_PROPERTY(QUrl galleryPreviewPath READ gallery_preview_path NOTIFY gallery_preview_path_altered)
    Q_PROPERTY(QUrl galleryThumbnailPath READ gallery_thumbnail_path NOTIFY gallery_thumbnail_path_altered)
    Q_PROPERTY(int orientation READ orientation NOTIFY orientation_altered)
    Q_PROPERTY(QDate exposureDate READ exposure_date NOTIFY exposure_date_time_altered)
    Q_PROPERTY(QTime exposureTimeOfDay READ exposure_time_of_day NOTIFY exposure_date_time_altered)
    Q_PROPERTY(int exposure_time_t READ exposure_time_t NOTIFY exposure_date_time_altered)
    Q_PROPERTY(QVariant event READ QmlFindEvent NOTIFY event_changed)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int width READ width NOTIFY size_altered)
    Q_PROPERTY(int height READ height NOTIFY size_altered)
    Q_PROPERTY(int maxSize READ maxSize NOTIFY maxSizeChanged)

signals:
    void path_altered();
    void preview_path_altered();
    void thumbnail_path_altered();
    void gallery_path_altered();
    void gallery_preview_path_altered();
    void gallery_thumbnail_path_altered();
    void orientation_altered();
    void exposure_date_time_altered();
    void event_changed();
    void data_altered();
    void size_altered();
    void busyChanged();
    void maxSizeChanged();

public:
    MediaSource();
    explicit MediaSource(const QFileInfo& file);

    static void RegisterType();

    QFileInfo file() const;
    QUrl path() const;
    virtual QUrl gallery_path() const;

    QString preview_file() const;
    QUrl preview_path() const;
    virtual QUrl gallery_preview_path() const;

    QString thumbnail_file() const;
    QUrl thumbnail_path() const;
    virtual QUrl gallery_thumbnail_path() const;

    virtual QImage Image(bool respect_orientation = true, const QSize &scaleSize=QSize());
    virtual Orientation orientation() const;
    virtual QDateTime exposure_date_time() const;

    const QSize& size();
    void set_size(const QSize& size);
    bool is_size_set() const;
    QDate exposure_date() const;
    QTime exposure_time_of_day() const;
    int exposure_time_t() const;

    Event* FindEvent();
    QVariant QmlFindEvent();

    void set_id(qint64 id);
    qint64 get_id() const;

    bool busy();
    void set_busy(bool busy);

    int maxSize() const;

protected:
    virtual void DestroySource(bool delete_backing, bool as_orphan);

    virtual void notify_data_altered();
    virtual void notify_size_altered();

private:
    int width() const {
        return size_.width();
    }

    int height() const {
        return size_.height();
    }

    QFileInfo file_;
    qint64 id_;
    QSize size_;
    bool busy_;
};

QML_DECLARE_TYPE(MediaSource)

#endif  // GALLERY_MEDIA_SOURCE_H_
