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

#include <QStringList>
#include <QtTest>

#include "core/command-line-parser.h"

class tst_CommandLineParser : public QObject
{
    Q_OBJECT
    CommandLineParser cmd_line_parser_;

public:
    tst_CommandLineParser();
    ~tst_CommandLineParser();

private slots:
    void is_portrait_test();
    void is_fullscreen_test();
    void startup_timer_test();
    void log_image_loading_test();

    void process_args_test();
    void process_args_test_data();
};

tst_CommandLineParser::tst_CommandLineParser() : cmd_line_parser_(new QHash<QString, QSize>())
{
    cmd_line_parser_.form_factors_->insert("desktop", QSize(160, 100)); // In BGU.
    cmd_line_parser_.form_factors_->insert("tablet", QSize(160, 100));
    cmd_line_parser_.form_factors_->insert("phone", QSize(71, 40));
    cmd_line_parser_.form_factors_->insert("sidebar", QSize(71, 40));
}

tst_CommandLineParser::~tst_CommandLineParser()
{
    delete cmd_line_parser_.form_factors_;
}

void tst_CommandLineParser::is_portrait_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_.is_portrait(), expect);
}

void tst_CommandLineParser::is_fullscreen_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_.is_fullscreen(), expect);
}

void tst_CommandLineParser::startup_timer_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_.startup_timer(), expect);
}

void tst_CommandLineParser::log_image_loading_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_.log_image_loading(), expect);
}

void tst_CommandLineParser::process_args_test_data()
{
    QTest::addColumn<QStringList>("process_args");
    QTest::addColumn<bool>("is_portrait");
    QTest::addColumn<bool>("is_fullscreen");
    QTest::addColumn<bool>("startup_timer");
    QTest::addColumn<bool>("log_image_loading");
    QTest::addColumn<bool>("invalid_arg");

    QStringList boolean_test;
    boolean_test.append("gallery");
    boolean_test.append("--portrait");
    boolean_test.append("--fullscreen");
    boolean_test.append("--startup-timer");
    boolean_test.append("--log-image-loading");

    QStringList invalid_arg_test;
    invalid_arg_test.append("gallery");
    invalid_arg_test.append("rAnD0m");

    QStringList help_test;
    help_test.append("gallery");
    help_test.append("-h");
    help_test.append("--landscape");

    QTest::newRow("Boolean member test") << boolean_test << true << true << true << true << true;
    QTest::newRow("Invalid arg test") << invalid_arg_test << false << false << false << false << false;
    QTest::newRow("Help test") << help_test << false << false << false << false << false;
}

void tst_CommandLineParser::process_args_test()
{
    QHash<QString, QSize> form_factors;

    CommandLineParser test(&form_factors);

    QFETCH(QStringList, process_args);
    QFETCH(bool, is_portrait);
    QFETCH(bool, is_fullscreen);
    QFETCH(bool, startup_timer);
    QFETCH(bool, log_image_loading);
    QFETCH(bool, invalid_arg);

    bool result = test.process_args(process_args);

    QCOMPARE(test.is_portrait(), is_portrait);
    QCOMPARE(test.is_fullscreen(), is_fullscreen);
    QCOMPARE(test.startup_timer(), startup_timer);
    QCOMPARE(test.log_image_loading(), log_image_loading);
    QCOMPARE(result, invalid_arg);
}

QTEST_APPLESS_MAIN(tst_CommandLineParser)

#include "tst_commandlineparser.moc"
