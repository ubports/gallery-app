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
 * Guenter Schwann <guenter.schwann@canonical.com>
*/

#ifndef GALLERYTHUMBNAILIMAGEPROVIDER_H
#define GALLERYTHUMBNAILIMAGEPROVIDER_H

#include <QFileInfo>
#include <QQuickImageProvider>
#include <QSize>
#include <QString>

class PreviewManager;

/*!
 * @brief The GalleryThumbailImageProvider class is used to load the small square thumbnails
 *
 * An extra image provider is used, so the image can be updated by adding parameters to the URL,
 * to update the image. As for local URL, no parameters are used (internally).
 * In future it might be used for more functionality, like custom caching of images.
 */
class GalleryThumbnailImageProvider : public QQuickImageProvider
{
public:
    static const char* PROVIDER_ID;
    static const char* PROVIDER_ID_SCHEME;
    static const char* REVISION_PARAM_NAME;

    GalleryThumbnailImageProvider();

    static QUrl toURL(const QFileInfo& file);

    virtual QImage requestImage(const QString& id, QSize* size,
                                const QSize& requestedSize);

    void setPreviewManager(PreviewManager* previewManager);
    void setLogging(bool enableLogging);

private:
    PreviewManager* m_previewManager;
    bool m_logImageLoading;
};

#endif // GALLERYTHUMBNAILIMAGEPROVIDER_H
