/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

#ifndef GALLERY_ALBUM_H_
#define GALLERY_ALBUM_H_

#include "album-page.h"
#include "album-template.h"

// core
#include "container-source.h"

// media
#include "media-source.h"

#include <QDateTime>
#include <QQmlListProperty>
#include <QString>
#include <QVariant>

class AlbumTable;

/*!
 * \brief The AlbumTemplate class

 Note about page numbers: the first page (number 0) is the front cover.  It
 always shows up on the right side of the spread.  The first "content" page
 is number 1, and it shows up on the left.  There are always an even number
 of content pages.  The last page (after the last content page) is the back
 cover, which always shows up on the left.  You can use first/lastContentPage
 and total/contentPageCount to avoid off-by-one errors.

 Note about "current page number": the current page is the page on the left
 of the current spread.  This means that when the album is closed,
 currentPage is actually -1 (since the cover on the right is page 0).
 You can use first/lastValidCurrentPage to avoid off-by-one errors.
 */
class Album : public ContainerSource
{
    Q_OBJECT
    Q_PROPERTY(qint64 id READ id NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QDateTime creationDateTime READ creationDateTime
               NOTIFY creationDateTimeChanged)
    Q_PROPERTY(QQmlListProperty<AlbumPage> contentPages READ qmlPages
               NOTIFY contentPagesChanged)
    Q_PROPERTY(QQmlListProperty<MediaSource> allMediaSources
               READ qmlAllMediaSources NOTIFY albumContentsChanged)
    Q_PROPERTY(int firstContentPage READ firstContentPage NOTIFY pageCountChanged)
    Q_PROPERTY(int lastContentPage READ lastContentPage NOTIFY pageCountChanged)
    Q_PROPERTY(int lastPopulatedContentPage READ lastPopulatedContentPage NOTIFY pageCountChanged)
    Q_PROPERTY(int totalPageCount READ totalPageCount NOTIFY pageCountChanged)
    Q_PROPERTY(int contentPageCount READ contentPageCount NOTIFY pageCountChanged)
    Q_PROPERTY(int populatedContentPageCount READ populatedContentPageCount NOTIFY pageCountChanged)
    Q_PROPERTY(int firstValidCurrentPage READ firstValidCurrentPage NOTIFY pageCountChanged)
    Q_PROPERTY(int lastValidCurrentPage READ lastValidCurrentPage NOTIFY pageCountChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage
               NOTIFY currentPageChanged)
    Q_PROPERTY(bool closed READ isClosed WRITE setClosed NOTIFY closedChanged)
    Q_PROPERTY(bool newAlbum READ isNewAlbum WRITE setNewAlbum NOTIFY newAlbumChanged)
    Q_PROPERTY(QString coverNickname READ coverNickname WRITE setCoverNickname
               NOTIFY coverNicknameChanged)

signals:
    void albumContentsChanged();
    void creationDateTimeChanged();
    void currentPageChanged();
    void currentPageContentsChanged();
    void titleChanged();
    void subtitleChanged();
    void closedChanged();
    void newAlbumChanged();
    void contentPagesChanged();
    void pageCountChanged();
    void coverNicknameChanged();
    void idChanged();

public:
    static const char *DEFAULT_TITLE;
    static const char *DEFAULT_SUBTITLE;
    static const int PAGES_PER_COVER;
    static const int FIRST_VALID_CURRENT_PAGE;

    Album(QObject *parent = 0);
    Album(QObject *parent, const QString &title, const QString &subtitle,
          qint64 id, QDateTime creationTimestamp, bool closed, int currentPage,
          const QString &coverNickname);

    virtual ~Album();

    Q_INVOKABLE void addMediaSource(QVariant vmedia);
    Q_INVOKABLE QVariant addSelectedMediaSources(QVariant mediaList);
    Q_INVOKABLE void removeMediaSource(QVariant vmedia);
    Q_INVOKABLE void removeSelectedMediaSources(QVariant mediaList);
    Q_INVOKABLE QVariant getPage(int page) const;
    Q_INVOKABLE int getPageForMediaSource(QVariant vmedia) const;
    Q_INVOKABLE bool containsMedia(QVariant vmedia) const;
    Q_INVOKABLE bool containsAll(QVariant vContainerSource) const;

    const QString& title() const;
    void setTitle(QString title);
    const QString& subtitle() const;
    void setSubtitle(QString subtitle);
    const QDateTime& creationDateTime() const;
    void setCreationDateTime(QDateTime timestamp);
    AlbumTemplate* albumTemplate() const;
    void setAlbumTemplate(AlbumTemplate *albumTemplate);
    bool isClosed() const;
    bool isNewAlbum() const;
    int totalPageCount() const;
    int contentPageCount() const;
    int populatedContentPageCount() const;
    int firstContentPage() const;
    int lastContentPage() const;
    int lastPopulatedContentPage() const;
    int firstValidCurrentPage() const;
    int lastValidCurrentPage() const;
    int currentPage() const;
    void setCurrentPage(int page);
    void setClosed(bool closed);
    void setNewAlbum(bool newAlbum);
    void setId(qint64 id);
    qint64 id() const;
    QString coverNickname() const;
    void setCoverNickname(QString name);

    SourceCollection* contentPages();

    AlbumPage* getAlbumPage(int page) const;

    QQmlListProperty<AlbumPage> qmlPages();
    QQmlListProperty<MediaSource> qmlAllMediaSources();

    void setAlbumTable(AlbumTable* albumTable);

protected:
    virtual void destroySource(bool destroyBacking, bool asOrphan);

    virtual void notifyCurrentPageChanged();
    virtual void notifyClosedChanged();
    virtual void notifyPageCountChanged();
    virtual void notifyContentPagesChanged();
    virtual void notifyCurrentPageContentsChanged();
    virtual void notifyContainerContentsChanged(const QSet<DataObject*>* added,
                                                const QSet<DataObject*>* removed);

    int contentToAbsolutePage(int contentPage) const;
    int absoluteToContentPage(int absolutePage) const;

    void createAddPhotosPage();

private slots:
    void onAlbumPageContentChanged(const QSet<DataObject*>* added,
                                   const QSet<DataObject*>* removed,
                                   bool notify);

private:
    void initInstance();
    QSet<DataObject*> mediaList2ObjectSet(QVariant mediaList) const;

    AlbumTemplate *m_albumTemplate;
    QString m_title;
    QString m_subtitle;
    QDateTime m_creationDateDime;
    int m_currentPage; // Page number of the left page of the current spread.
    bool m_closed;
    bool m_newAlbum;
    int m_populatedPagesCount;
    SourceCollection* m_contentPages;
    QList<MediaSource*> m_allMediaSources;
    QList<AlbumPage*> m_allAlbumPages;
    bool m_refreshingContainer;
    qint64 m_id;
    QString m_coverNickname;
    AlbumTable *m_albumTable;
};

QML_DECLARE_TYPE(Album)

#endif  // GALLERY_ALBUM_H_
