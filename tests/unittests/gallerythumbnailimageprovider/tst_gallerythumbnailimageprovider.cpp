/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Guenter Schwann <guenter.schwann@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtTest/QtTest>
#include <QFileInfo>
#include <QUrl>

#include "qml/gallery-thumbnail-image-provider.h"

class tst_GalleryThumbnailImageProvider : public QObject
{
  Q_OBJECT
public:
    tst_GalleryThumbnailImageProvider();

private slots:
    void ToURL();

private:
    GalleryThumbnailImageProvider gallery_thumbnail_image_provider;
};

tst_GalleryThumbnailImageProvider::tst_GalleryThumbnailImageProvider()
    : gallery_thumbnail_image_provider(false)
{
}

void tst_GalleryThumbnailImageProvider::ToURL()
{
    QFileInfo fi("/tmp/test.jpg");
    QUrl url = gallery_thumbnail_image_provider.ToURL(fi);
    QUrl expect("image://gallery-thumbnail//tmp/test.jpg");
    QCOMPARE(url, expect);
}

QTEST_MAIN(tst_GalleryThumbnailImageProvider);

#include "tst_gallerythumbnailimageprovider.moc"
