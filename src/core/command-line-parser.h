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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QDir>
#include <QHash>
#include <QSize>
#include <QString>

/*!
 * @brief The CommandLineParser is used to parse our commandline inputs and set parameters accordingly.
 */
class CommandLineParser
{
public:
    CommandLineParser(QHash<QString, QSize>* form_factors);

    bool process_args(const QStringList& args);

    QDir pictures_dir() const { return pictures_dir_; }
    bool is_portrait() const { return is_portrait_; }
    bool is_fullscreen() const { return is_fullscreen_; }
    bool startup_timer() const { return startup_timer_; }
    bool log_image_loading() const { return log_image_loading_; }

    QString form_factor() { return form_factor_; }

private:
    bool invalid_arg(QString arg);
    void usage();

    bool startup_timer_;
    bool is_fullscreen_;
    bool is_portrait_;
    QDir pictures_dir_;

    bool log_image_loading_;

    QHash<QString, QSize>* form_factors_;
    QString form_factor_;

    friend class tst_CommandLineParser;
};

#endif // COMMANDLINEPARSER_H
