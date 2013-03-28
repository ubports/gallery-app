/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Nicolas d'Offay <nicolas.doffay@canonical.com>
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
 *
 */

#include <QtTest/QtTest>
#include <QFileInfo>
#include <QUrl>

#include "qml/gallery-standard-image-provider.h"
#include "core/gallery-manager.h"
#include "media/preview-manager.h"

class tst_GalleryStandardImageProvider : public QObject
{
    Q_OBJECT
public:
    tst_GalleryStandardImageProvider();

private slots:
    void ToURL();
    void Fullsize();
    void Thumbnail();

private:
    GalleryStandardImageProvider gallery_standard_image_provider;
};

tst_GalleryStandardImageProvider::tst_GalleryStandardImageProvider()
    : gallery_standard_image_provider(false)
{
}


void tst_GalleryStandardImageProvider::ToURL()
{
    QFileInfo fi("/tmp/test.jpg");
    QUrl url = gallery_standard_image_provider.ToURL(fi);
    QUrl expect("image://gallery-standard//tmp/test.jpg");
    QCOMPARE(url, expect);
}

void tst_GalleryStandardImageProvider::Fullsize()
{
    QString id = "/home/user/Pictures/logo.jpg?size_level=0&orientation=1";
    QString fileName = "/home/user/Pictures/logo.jpg";
    QCOMPARE(GalleryStandardImageProvider::CachedImage::idToFile(id), fileName);
}

void tst_GalleryStandardImageProvider::Thumbnail()
{
    QString id = "/home/user/Pictures/logo.jpg?size_level=1&orientation=1";
    QString fileName = "/home/user/Pictures/.thumbs/logo_th.JPG";
    QCOMPARE(GalleryStandardImageProvider::CachedImage::idToFile(id), fileName);
}

QTEST_MAIN(tst_GalleryStandardImageProvider);

#include "tst_gallerystandardimageprovidertest.moc"
