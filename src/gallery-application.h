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

#include "media-source.h"

class CommandLineParser;
class ContentCommunicator;
class GalleryManager;

class QQuickView;

/*!
 * \brief The GalleryApplication class
 */
class GalleryApplication : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(bool pickModeEnabled READ pickModeEnabled NOTIFY pickModeEnabledChanged)
    Q_PROPERTY(MediaSource::MediaType mediaTypeFilter READ mediaTypeFilter NOTIFY mediaTypeFilterChanged)
    Q_PROPERTY(bool desktopMode READ isDesktopMode CONSTANT)
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)

public:
    enum UiMode{
        BrowseContentMode,
        PickContentMode
    };

    explicit GalleryApplication(int& argc, char** argv);
    virtual ~GalleryApplication();

    int exec();

    void setDefaultUiMode(UiMode mode);
    UiMode defaultUiMode() const;
    void setUiMode(UiMode mode);
    bool pickModeEnabled() const;
    bool isDesktopMode() const;
    bool isFullScreen() const;
    MediaSource::MediaType mediaTypeFilter() const;

    Q_INVOKABLE void returnPickedContent(QVariant variant);
    Q_INVOKABLE void contentPickingCanceled();

    static void startStartupTimer();

signals:
    void mediaLoaded();
    void pickModeEnabledChanged();
    void fullScreenChanged();
    void mediaTypeFilterChanged();

private slots:
    void initCollections();
    void switchToPickMode(QString mediaTypeFilter);
    void setFullScreen(bool fullScreen);
    void consistencyCheckFinished();

private:
    void registerQML();
    void createView();

    QQuickView *m_view;
    GalleryManager *m_galleryManager;
    CommandLineParser* m_cmdLineParser;
    ContentCommunicator *m_contentCommunicator;
    QHash<QString, QSize> m_formFactors;
    int m_bguSize;
    bool m_pickModeEnabled;
    UiMode m_defaultUiMode;
    MediaSource::MediaType m_mediaTypeFilter;

    static QElapsedTimer *m_timer;
};

#endif // GALLERYAPPLICATION_H
