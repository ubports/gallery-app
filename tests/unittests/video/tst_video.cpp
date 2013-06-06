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

#include "video/video.h"

class tst_Video : public QObject
{
  Q_OBJECT

private slots:
    void dummy();
};

void tst_Video::dummy()
{
    Video video();
    QCOMPARE(0, 0);
}

QTEST_MAIN(tst_Video);

#include "tst_video.moc"
