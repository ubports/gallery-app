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
#include <QString>

#include "util/resource.h"

class tst_Resource : public QObject
{
  Q_OBJECT

private slots:
    void is_installed_data();
    void is_installed();

    void trailing_slash();

    void get_rc_url_data();
    void get_rc_url();

    void maxTextureSize();
};


void tst_Resource::is_installed_data()
{
    QTest::addColumn<QString>("appDir");
    QTest::addColumn<QString>("installDir");
    QTest::addColumn<bool>("installed");

    QTest::newRow("system") << QString("/usr/bin") << QString("/usr") << true;
    QTest::newRow("local") << QString("/home/user/dev/gallery") << QString("/usr") << false;
}
void tst_Resource::is_installed()
{
    QFETCH(QString, appDir);
    QFETCH(QString, installDir);
    QFETCH(bool, installed);

    Resource resource(appDir, installDir);
    QCOMPARE(resource.is_installed(), installed);
}

void tst_Resource::trailing_slash()
{
    Resource resource("", "");
    QCOMPARE(resource.trailing_slash("/usr"), QString("/usr/"));
    QCOMPARE(resource.trailing_slash("/usr/"), QString("/usr/"));
}

void tst_Resource::get_rc_url_data()
{
    QTest::addColumn<QString>("appDir");
    QTest::addColumn<QString>("installDir");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");

    QTest::newRow("system") << QString("/usr/bin") << QString("/usr") << QString("img/icon.png")
                            << QUrl("file:///usr/share/gallery-app/rc/img/icon.png");
    QTest::newRow("local") << QString("/home/user/dev/gallery/src") << QString("/usr") << QString("img/icon.png")
                           << QUrl("file:///home/user/dev/gallery/src/../rc/img/icon.png");
}
void tst_Resource::get_rc_url()
{
    QFETCH(QString, appDir);
    QFETCH(QString, installDir);
    QFETCH(QString, path);
    QFETCH(QUrl, url);

    Resource resource(appDir, installDir);
    QCOMPARE(resource.get_rc_url(path), url);
}

void tst_Resource::maxTextureSize()
{
    Resource resource("", "");
    QCOMPARE(resource.maxTextureSize(), 2048);
}

QTEST_MAIN(tst_Resource);

#include "tst_resource.moc"
