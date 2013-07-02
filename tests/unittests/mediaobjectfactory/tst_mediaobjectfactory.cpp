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

// photo
#include <photo.h>

// for controlling the fake MediaTable
extern void setOrientationOfFirstRow(Orientation orientation);

class tst_MediaObjectFactory : public QObject
{
  Q_OBJECT

private slots:
    void init();
    void cleanup();

    void create();
    void clearMetadata();
    void readPhotoMetadata();
    void readVideoMetadata();
    void enableContentLoadFilter();
    void addMedia();

private:
    MediaTable *m_mediaTable;
    MediaObjectFactory *m_factory;
};

void tst_MediaObjectFactory::init()
{
    m_mediaTable = new MediaTable(0, 0);
    m_factory = new MediaObjectFactory();
    m_factory->setMediaTable(m_mediaTable);
}

void tst_MediaObjectFactory::cleanup()
{
    delete m_factory;
    m_factory = 0;
    delete m_mediaTable;
    m_mediaTable = 0;
}

void tst_MediaObjectFactory::create()
{
    // invalid file
    MediaSource *media = m_factory->create(QFileInfo("no_valid_file"));
    QCOMPARE(media, (MediaSource*)0);

    // new file
    media = m_factory->create(QFileInfo("/some/photo.jpg"));
    Photo *photo = qobject_cast<Photo*>(media);
    QVERIFY(photo != 0);
    QCOMPARE(photo->id(), (qint64)0);
    QCOMPARE(photo->exposureDateTime(), QDateTime(QDate(2013, 01, 01), QTime(11, 11, 11)));
    QCOMPARE(photo->orientation(), BOTTOM_LEFT_ORIGIN);

    // another new file
    media = m_factory->create(QFileInfo("/some/other_photo.jpg"));
    photo = qobject_cast<Photo*>(media);
    QVERIFY(photo != 0);
    QCOMPARE(photo->id(), (qint64)1);

    // existing from DB
    media = m_factory->create(QFileInfo("/some/photo.jpg"));
    photo = qobject_cast<Photo*>(media);
    QVERIFY(photo != 0);
    QCOMPARE(photo->id(), (qint64)0);

    // update DB from file
    setOrientationOfFirstRow(TOP_RIGHT_ORIGIN); // change the DB

    media = m_factory->create(QFileInfo("/some/photo.jpg"));
    photo = qobject_cast<Photo*>(media);
    QVERIFY(photo != 0);
    QCOMPARE(photo->id(), (qint64)0);
    QCOMPARE(photo->orientation(), TOP_RIGHT_ORIGIN);
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

void tst_MediaObjectFactory::enableContentLoadFilter()
{
    MediaSource *media = m_factory->create(QFileInfo("/some/photo.jpg"));
    QVERIFY(media != 0);

    m_factory->enableContentLoadFilter(MediaSource::Video);

    media = m_factory->create(QFileInfo("/some/photo.jpg"));
    QVERIFY(media == 0);
}

void tst_MediaObjectFactory::addMedia()
{
    qint64 id = 123;
    QString filename("/some/photo.jpg");
    QSize size(320, 200);
    QDateTime timestamp(QDate(2013, 02, 03), QTime(12, 12, 12));
    QDateTime exposureTime(QDate(2013, 03, 04), QTime(1, 2, 3));
    Orientation originalOrientation(BOTTOM_RIGHT_ORIGIN);
    qint64 filesize = 2048;

    m_factory->addMedia(id, filename, size, timestamp,
                                             exposureTime, originalOrientation,
                                             filesize);

    QCOMPARE(m_factory->m_mediasFromDB.size(), 1);
    QSet<DataObject*>::iterator it;
    it = m_factory->m_mediasFromDB.begin();
    DataObject *obj = *it;
    Photo *photo = qobject_cast<Photo*>(obj);
    QVERIFY(photo != 0);
    QCOMPARE(photo->id(), id);
    QCOMPARE(photo->path().toLocalFile(), filename);
    QCOMPARE(photo->size(), size);
    QCOMPARE(photo->exposureDateTime(), exposureTime);
    QCOMPARE(photo->fileTimestamp(), timestamp);
    QCOMPARE(photo->orientation(), originalOrientation);
}

QTEST_MAIN(tst_MediaObjectFactory);

#include "tst_mediaobjectfactory.moc"
