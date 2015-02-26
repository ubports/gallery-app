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
    bool isFullScreen() const;
    MediaSource::MediaType mediaTypeFilter() const;
    const QString &getMediaFile() const;

    Q_INVOKABLE void returnPickedContent(QVariant variant);
    Q_INVOKABLE void contentPickingCanceled();
    Q_INVOKABLE void parseUri(const QString &arg);

    static void startStartupTimer();

signals:
    void mediaLoaded();
    void pickModeEnabledChanged();
    void fullScreenChanged();
    void mediaTypeFilterChanged();
    void mediaFileChanged();
    void eventsViewRequested();

private slots:
    void initCollections();
    void switchToPickMode(QString mediaTypeFilter);
    void switchToEventsView();
    void setFullScreen(bool fullScreen);
    void consistencyCheckFinished();
    void setMediaFile(const QString &mediaFile);
    void onMediaLoaded();
    void onCollectionChanged();

private:
    void registerQML();
    void createView();
    static QObject* exportFileUtilsSingleton(QQmlEngine *engine,
                                             QJSEngine *scriptEngine);

    QQuickView *m_view;
    GalleryManager *m_galleryManager;
    CommandLineParser* m_cmdLineParser;
    UrlHandler *m_urlHandler;
    ContentCommunicator *m_contentCommunicator;
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
