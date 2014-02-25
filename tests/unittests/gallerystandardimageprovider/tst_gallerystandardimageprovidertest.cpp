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
#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include "gallery-standard-image-provider.h"
#include "media-collection.h"

class tst_GalleryStandardImageProvider : public QObject
{
    Q_OBJECT
public:
    tst_GalleryStandardImageProvider();

private slots:
    void ToURL();
    void idToFile_data();
    void idToFile();

private:
    GalleryStandardImageProvider gallery_standard_image_provider;
};

tst_GalleryStandardImageProvider::tst_GalleryStandardImageProvider()
    : gallery_standard_image_provider()
{
}


void tst_GalleryStandardImageProvider::ToURL()
{
    QFileInfo fi("/tmp/test.jpg");
    QUrl url = gallery_standard_image_provider.toURL(fi);
    QUrl expect("image://gallery-standard//tmp/test.jpg");
    QCOMPARE(url, expect);
}

void tst_GalleryStandardImageProvider::idToFile_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("fileName");

    QTest::newRow("FullSize") << "/home/user/Pictures/logo.jpg?size_level=0&orientation=1" <<
                                 "/home/user/Pictures/logo.jpg";
}

void tst_GalleryStandardImageProvider::idToFile()
{
    QFETCH(QString, id);
    QFETCH(QString, fileName);

    GalleryStandardImageProvider provider;
    QCOMPARE(provider.idToFile(id), fileName);
}

QTEST_MAIN(tst_GalleryStandardImageProvider);

#include "tst_gallerystandardimageprovidertest.moc"
