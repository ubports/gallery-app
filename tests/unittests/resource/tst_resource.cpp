/*
 * Copyright (C) 2013 Canonical, Ltd.
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
#include <QStandardPaths>
#include <QString>

#include "resource.h"

class tst_Resource : public QObject
{
  Q_OBJECT

private slots:
    void picturesDirectory();
    void databaseDirectory();
    void thumbnailDirectory();
};

void tst_Resource::picturesDirectory()
{
    Resource resource(false, "");
    QCOMPARE(resource.mediaDirectories().size(), 2);
    QCOMPARE(resource.mediaDirectories().at(0), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    QString picDir("/tmp");
    Resource resource2(false, picDir);
    QCOMPARE(resource2.mediaDirectories().size(), 1);
    QCOMPARE(resource2.mediaDirectories().at(0), picDir);
}

void tst_Resource::databaseDirectory()
{
    Resource resource(false, "");
    QString dbDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            QDir::separator() + Resource::DATABASE_DIR;
    QCOMPARE(resource.databaseDirectory(), dbDir);

    QString picDir("/tmp");
    Resource resource2(false, picDir);
    dbDir = picDir + "/." + Resource::DATABASE_DIR;
    QCOMPARE(resource2.databaseDirectory(), dbDir);
}

void tst_Resource::thumbnailDirectory()
{
    Resource resource(false, "");
    QString dbDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            QDir::separator() + Resource::THUMBNAIL_DIR;
    QCOMPARE(resource.thumbnailDirectory(), dbDir);

    QString picDir("/tmp");
    Resource resource2(false, picDir);
    dbDir = picDir + "/." + Resource::THUMBNAIL_DIR;
    QCOMPARE(resource2.thumbnailDirectory(), dbDir);
}

QTEST_MAIN(tst_Resource);

#include "tst_resource.moc"
