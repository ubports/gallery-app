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

#include "imaging.h"

class tst_Imaging : public QObject
{
  Q_OBJECT

private slots:
    void transform_pixel_data();
    void transform_pixel();
};


void tst_Imaging::transform_pixel_data()
{
    QTest::addColumn<QColor>("color");
    QTest::addColumn<qreal>("brightness");
    QTest::addColumn<qreal>("contrast");
    QTest::addColumn<qreal>("saturation");
    QTest::addColumn<qreal>("hue");
    QTest::addColumn<QColor>("result");

    QTest::newRow("Unchanged") <<
        QColor(255, 0, 0) << (qreal)1.0 << (qreal)1.0 << (qreal)1.0 << (qreal)0.0 << QColor(255, 0, 0);
    QTest::newRow("Brightness0.5") <<
        QColor(255, 128, 64) << (qreal)0.5 << (qreal)1.0 << (qreal)1.0 << (qreal)0.0 << QColor(127, 64, 32);
    QTest::newRow("Contrast0.5") <<
        QColor(0, 255, 0) << (qreal)1.0 << (qreal)0.5 << (qreal)1.0 << (qreal)0.0 << QColor(63, 191, 63);
    QTest::newRow("Saturation0.5") <<
        QColor(0, 0, 255) << (qreal)1.0 << (qreal)1.0 << (qreal)0.5 << (qreal)0.0 << QColor(10, 10, 137);
    QTest::newRow("Hue180") <<
        QColor(255, 0, 0) << (qreal)1.0 << (qreal)1.0 << (qreal)1.0 << (qreal)180.0 << QColor(0, 169, 169);
}

void tst_Imaging::transform_pixel()
{
    QFETCH(QColor, color);
    QFETCH(qreal, brightness);
    QFETCH(qreal, contrast);
    QFETCH(qreal, saturation);
    QFETCH(qreal, hue);
    QFETCH(QColor, result);

    ColorBalance cb(brightness, contrast, saturation, hue);
    QCOMPARE(cb.transformPixel(color), result);
}

QTEST_MAIN(tst_Imaging);

#include "tst_imaging.moc"
