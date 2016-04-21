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
#include <QTimer>

#include "media-source.h"

class CommandLineParser;
class UrlHandler;
class GalleryManager;

class QQuickView;

/*!
 * \brief The GalleryApplication class
 */
class GalleryApplication : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(bool pickModeEnabled READ pickModeEnabled WRITE setPickModeEnabled NOTIFY pickModeEnabledChanged)
    Q_PROPERTY(MediaSource::MediaType mediaTypeFilter READ mediaTypeFilter NOTIFY mediaTypeFilterChanged)
    Q_PROPERTY(bool desktopMode READ isDesktopMode CONSTANT)
    Q_PROPERTY(bool fullScreenAppMode READ isFullScreenAppMode WRITE setFullScreenAppMode NOTIFY fullScreenAppModeChanged)
    Q_PROPERTY(bool fullScreenUserMode READ isFullScreenUserMode WRITE setFullScreenUserMode NOTIFY fullScreenUserModeChanged)
    Q_PROPERTY(QString mediaFile READ getMediaFile WRITE setMediaFile NOTIFY mediaFileChanged)

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
    bool isFullScreenAppMode() const;
    bool isFullScreenUserMode() const;
    MediaSource::MediaType mediaTypeFilter() const;
    const QString &getMediaFile() const;

    Q_INVOKABLE void parseUri(const QString &arg);
    Q_INVOKABLE void handleImportedFile(const QUrl &url);

    static void startStartupTimer();

signals:
    void mediaLoaded();
    void pickModeEnabledChanged();
    void fullScreenAppModeChanged();
    void fullScreenUserModeChanged();
    void mediaTypeFilterChanged();
    void mediaFileChanged();

private slots:
    void initCollections();
    void setPickModeEnabled(bool pickModeEnabled);
    void setFullScreenAppMode(bool fullScreen);
    void setFullScreenUserMode(bool fullScreen);
    void setMediaFile(const QString &mediaFile);
    void onMediaLoaded();
    void onCollectionChanged();

private:
    void registerQML();
    void createView();

    QQuickView *m_view;
    GalleryManager *m_galleryManager;
    CommandLineParser* m_cmdLineParser;
    UrlHandler *m_urlHandler;
    QHash<QString, QSize> m_formFactors;
    int m_bguSize;
    bool m_pickModeEnabled;
    UiMode m_defaultUiMode;
    MediaSource::MediaType m_mediaTypeFilter;
    QString m_mediaFile;
    QTimer m_mediaLoadedTimer;
    bool m_mediaLoaded;

    static QElapsedTimer *m_timer;
};

#endif // GALLERYAPPLICATION_H
