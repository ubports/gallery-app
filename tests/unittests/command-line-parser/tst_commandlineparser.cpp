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

#include "command-line-parser.h"

class tst_CommandLineParser : public QObject
{
    Q_OBJECT
    QHash<QString, QSize> form_factors_;
    CommandLineParser* cmd_line_parser_;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void is_portrait_test();
    void is_fullscreen_test();
    void startup_timer_test();
    void log_image_loading_test();

    void process_args_test();
    void process_args_test_data();
};

void tst_CommandLineParser::initTestCase()
{
    cmd_line_parser_ = new CommandLineParser(form_factors_);
}

void tst_CommandLineParser::cleanupTestCase()
{
    delete cmd_line_parser_;
}

void tst_CommandLineParser::is_portrait_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_->isPortrait(), expect);
}

void tst_CommandLineParser::is_fullscreen_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_->isFullscreen(), expect);
}

void tst_CommandLineParser::startup_timer_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_->startupTimer(), expect);
}

void tst_CommandLineParser::log_image_loading_test()
{
    bool expect = false;

    QCOMPARE(cmd_line_parser_->logImageLoading(), expect);
}

void tst_CommandLineParser::process_args_test_data()
{
    QTest::addColumn<QStringList>("process_args");
    QTest::addColumn<bool>("is_portrait");
    QTest::addColumn<bool>("is_fullscreen");
    QTest::addColumn<bool>("startup_timer");
    QTest::addColumn<bool>("log_image_loading");
    QTest::addColumn<bool>("pick_mode_enabled");
    QTest::addColumn<bool>("invalid_arg");

    QStringList boolean_test;
    boolean_test.append("gallery");
    boolean_test.append("--portrait");
    boolean_test.append("--fullscreen");
    boolean_test.append("--startup-timer");
    boolean_test.append("--log-image-loading");
    boolean_test.append("--pick-mode");

    QStringList invalid_arg_test;
    invalid_arg_test.append("gallery");
    invalid_arg_test.append("rAnD0m");

    QStringList help_test;
    help_test.append("gallery");
    help_test.append("-h");
    help_test.append("--landscape");

    QTest::newRow("Boolean member test") << boolean_test << true << true << true << true
                                         << true << true;
    QTest::newRow("Invalid arg test") << invalid_arg_test << false << false << false << false
                                      << false << false;
    QTest::newRow("Help test") << help_test << false << false << false << false
                               << false << false;
}

void tst_CommandLineParser::process_args_test()
{
    QHash<QString, QSize> form_factors;

    CommandLineParser test(form_factors);

    QFETCH(QStringList, process_args);
    QFETCH(bool, is_portrait);
    QFETCH(bool, is_fullscreen);
    QFETCH(bool, startup_timer);
    QFETCH(bool, log_image_loading);
    QFETCH(bool, pick_mode_enabled);
    QFETCH(bool, invalid_arg);

    bool result = test.processArguments(process_args);

    QCOMPARE(test.isPortrait(), is_portrait);
    QCOMPARE(test.isFullscreen(), is_fullscreen);
    QCOMPARE(test.startupTimer(), startup_timer);
    QCOMPARE(test.logImageLoading(), log_image_loading);
    QCOMPARE(test.pickModeEnabled(), pick_mode_enabled);
    QCOMPARE(result, invalid_arg);
}

QTEST_APPLESS_MAIN(tst_CommandLineParser)

#include "tst_commandlineparser.moc"
