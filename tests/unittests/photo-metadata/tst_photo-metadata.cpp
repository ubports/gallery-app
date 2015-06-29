/*
 * Copyright (C) 2015 Canonical, Ltd.
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

#include "photoeditor/photo-metadata.h"

class tst_PhotoMetadata : public QObject
{
  Q_OBJECT

private slots:
    void exposureTime();

private:
    PhotoMetadata *m_metadata;
};

void tst_PhotoMetadata::exposureTime()
{
    m_metadata = PhotoMetadata::fromFile(SAMPLE_IMAGE_DIR "/sample01.jpg");
    // Check that the original exposure time is returned
    QCOMPARE(m_metadata->exposureTime(), QDateTime(QDate(2015, 5, 8), QTime(1, 51, 48)));

    // Set DateTimeDigitized the same way gallery does for imported files
    m_metadata->setDateTimeDigitized(QDateTime(QDate(2015, 12, 31), QTime(23, 59, 59)));
    // Check that the new exposure time is returned
    QCOMPARE(m_metadata->exposureTime(), QDateTime(QDate(2015, 12, 31), QTime(23, 59, 59)));
}

QTEST_MAIN(tst_PhotoMetadata);

#include "tst_photo-metadata.moc"
