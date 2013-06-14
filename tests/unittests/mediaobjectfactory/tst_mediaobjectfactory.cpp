/*
 * Copyright (C) 2013 Canonical, Ltd.
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
#include <QString>

#include "media-object-factory.h"

// database
#include "media-table.h"

class tst_MediaObjectFactory : public QObject
{
  Q_OBJECT

private slots:
    void init();
    void cleanup();

    void clearMetadata();
    void readPhotoMetadata();
    void readVideoMetadata();

private:
    MediaTable *m_mediaTable;
    MediaObjectFactory *m_factory;
};

void tst_MediaObjectFactory::init()
{
    m_mediaTable = 0;
    m_factory = new MediaObjectFactory(m_mediaTable);
}

void tst_MediaObjectFactory::cleanup()
{
    delete m_factory;
    m_factory = 0;
}

void tst_MediaObjectFactory::clearMetadata()
{
    m_factory->m_timeStamp = QDateTime::currentDateTime();
    m_factory->m_exposureTime = QDateTime::currentDateTime();
    m_factory->m_size = QSize(1, 1);
    m_factory->m_orientation = BOTTOM_RIGHT_ORIGIN;
    m_factory->m_fileSize = 999;

    m_factory->clearMetadata();

    QCOMPARE(m_factory->m_timeStamp, QDateTime());
    QCOMPARE(m_factory->m_exposureTime, QDateTime());
    QCOMPARE(m_factory->m_size, QSize());
    QCOMPARE(m_factory->m_orientation, TOP_LEFT_ORIGIN);
    QCOMPARE(m_factory->m_fileSize, (qint64)0);
}

void tst_MediaObjectFactory::readPhotoMetadata()
{
    bool success = m_factory->readPhotoMetadata(QFileInfo("no_valid_file"));
    QCOMPARE(success, false);

    success = m_factory->readPhotoMetadata(QFileInfo("/some/photo.jpg"));
    QCOMPARE(success, true);
    QCOMPARE(m_factory->m_exposureTime, QDateTime(QDate(2013, 01, 01), QTime(11, 11, 11)));
    QCOMPARE(m_factory->m_orientation, BOTTOM_LEFT_ORIGIN);
}

void tst_MediaObjectFactory::readVideoMetadata()
{
    bool success = m_factory->readPhotoMetadata(QFileInfo("no_valid_file"));
    QCOMPARE(success, false);
}

QTEST_MAIN(tst_MediaObjectFactory);

#include "tst_mediaobjectfactory.moc"
