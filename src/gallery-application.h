/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Charles Lindsay <chaz@yorba.org>
 */

#ifndef GALLERYAPPLICATION_H
#define GALLERYAPPLICATION_H

#include <QApplication>
#include <QElapsedTimer>
#include <QFileInfo>

class CommandLineParser;
class GalleryManager;

class QQuickView;

/*!
 * \brief The GalleryApplication class
 */
class GalleryApplication : public QApplication
{
    Q_OBJECT

public:
    explicit GalleryApplication(int& argc, char** argv);
    virtual ~GalleryApplication();

    int exec();

    Q_INVOKABLE bool runCommand(const QString &cmd, const QString &arg);

    static void startStartupTimer();

signals:
    void mediaLoaded();

private slots:
    void initCollections();

private:
    void registerQML();
    void createView();

    QQuickView *m_view;
    GalleryManager *m_galleryManager;
    CommandLineParser* m_cmdLineParser;
    QHash<QString, QSize> m_formFactors;
    int m_bguSize;

    static QElapsedTimer *m_timer;
};

#endif // GALLERYAPPLICATION_H
