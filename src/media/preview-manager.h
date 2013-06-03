/*
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef GALLERY_PREVIEW_MANAGER_H_
#define GALLERY_PREVIEW_MANAGER_H_

#include <QFileInfo>
#include <QMutex>
#include <QObject>
#include <QSet>
#include <QString>

class QImage;
class DataObject;
class MediaSource;

/*!
 * \brief The PreviewManager class
 */
class PreviewManager : public QObject
{
    Q_OBJECT

public:
    static const int PREVIEW_WIDTH_MAX;
    static const int PREVIEW_HEIGHT_MAX;
    static const int THUMBNAIL_SIZE;
    static const int PREVIEW_QUALITY;
    static const char* PREVIEW_FILE_FORMAT;
    static const char* PREVIEW_FILE_EXT;

    static const QString PREVIEW_DIR;

    PreviewManager();

    static QFileInfo PreviewFileFor(const QFileInfo& file);
    static QFileInfo ThumbnailFileFor(const QFileInfo& file);

    bool ensure_preview_for_media(QFileInfo file, bool regen = false);

private slots:
    void on_media_added_removed(const QSet<DataObject*>* added,
                                const QSet<DataObject*>* removed);
    void on_media_destroying(const QSet<DataObject*>* destroying);
    void on_media_data_altered();

private:
    void DestroyPreview(MediaSource* media);
    QImage generate_Thumbnail(const QImage& master) const;

    static QMutex createMutex_;
};

#endif  // GALLERY_PREVIEW_MANAGER_H_
