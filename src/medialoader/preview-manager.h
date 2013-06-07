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
class MediaCollection;
class MediaSource;

/*!
 * \brief The PreviewManager class creates and removes thumbnails
 *
 * The thumbnail storage and creation is inspired by
 * http://specifications.freedesktop.org/thumbnail-spec/thumbnail-spec-latest.html
 * But it uses jpeg (for performance),
 * uses different sizes,
 * not the standard sub directoy name (because of the sizes),
 * does not save any meta information in the thumbnails,
 * does not use the fail directory
 */
class PreviewManager : public QObject
{
    Q_OBJECT

public:
    static const int PREVIEW_SIZE;
    static const int THUMBNAIL_SIZE;
    static const int PREVIEW_QUALITY;
    static const char* PREVIEW_FILE_FORMAT;
    static const char* PREVIEW_FILE_EXT;

    static const QString PREVIEW_DIR;
    static const QString THUMBNAIL_DIR;

    PreviewManager(const QString& thumbnailDirectory, MediaCollection *mediaCollection,
                   QObject* parent = 0);

    QString previewFileName(const QFileInfo& file) const;
    QString thumbnailFileName(const QFileInfo& file) const;

    bool ensurePreview(QFileInfo file, bool regen = false);

public slots:
    void onMediaAddedRemoved(const QSet<DataObject*>* added,
                                const QSet<DataObject*>* removed);
    void onMediaDestroying(const QSet<DataObject*>* destroying);

private slots:
    void updatePreview();

private:
    bool saveThumbnail(const QImage& image, const QString& fileName) const;
    void destroyPreviews(MediaSource* media);
    QImage generateThumbnail(const QImage& master) const;
    QString thumbnailFileName(const QString& fileName, const QString& levelName) const;
    bool updateNeeded(const QFileInfo& mediaFile, const QFileInfo& previewFile) const;

    static QMutex m_createMutex;
    QString m_thumbnailDir;
    MediaCollection *m_mediaCollection;
};

#endif  // GALLERY_PREVIEW_MANAGER_H_
