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

#include <QHash>
#include <QSize>
#include <QString>

class UrlHandler;

/*!
 * @brief The CommandLineParser is used to parse our commandline inputs and set
 * parameters accordingly.
 */
class CommandLineParser
{
public:
    CommandLineParser(const QHash<QString, QSize>& form_factors);

    bool processArguments(const QStringList& args);

    const QString &picturesDir() const { return m_picturesDir; }
    bool isPortrait() const { return m_isPortrait; }
    bool isFullscreen() const { return m_isFullscreen; }
    bool startupTimer() const { return m_startupTimer; }
    bool logImageLoading() const { return m_logImageLoading; }
    bool pickModeEnabled() const { return m_pickMode; }
    const QString &formFactor() const { return m_formFactor; }
    const QString &mediaFile() const { return m_mediaFile; }

private:
    bool invalidArg(QString arg);
    void usage();

    UrlHandler *m_urlHandler;
    bool m_startupTimer;
    bool m_isFullscreen;
    bool m_isPortrait;
    QString m_picturesDir;
    bool m_pickMode;
    bool m_logImageLoading;

    const QHash<QString, QSize> m_formFactors;
    QString m_formFactor;
    QString m_mediaFile;
};

#endif // COMMANDLINEPARSER_H
