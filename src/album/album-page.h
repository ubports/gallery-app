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
 */

#ifndef GALLERY_ALBUM_PAGE_H_
#define GALLERY_ALBUM_PAGE_H_

#include "album-template-page.h"

// core
#include "container-source.h"

// media
#include "media-source.h"

#include <QObject>
#include <QUrl>

class Album;

/*!
 * \brief The AlbumPage class
 */
class AlbumPage : public ContainerSource
{
    Q_OBJECT
    Q_PROPERTY(int pageNumber READ pageNumber NOTIFY pageNumberChanged);
    Q_PROPERTY(QUrl qmlRc READ qmlRc NOTIFY qmlRcChanged);
    Q_PROPERTY(QQmlListProperty<MediaSource> mediaSourceList
               READ qmlMediaSourceList NOTIFY mediaSourceListChanged);
    Q_PROPERTY(QVariant owner READ qmlOwner NOTIFY ownerChanged);

signals:
    void pageNumberChanged();
    void qmlRcChanged();
    void mediaSourceListChanged();
    void ownerChanged();

public:
    AlbumPage(QObject * parent = 0);
    explicit AlbumPage(Album* owner);
    AlbumPage(Album* owner, int pageNumber, AlbumTemplatePage* templatePage);

    int pageNumber() const;
    AlbumTemplatePage* templatePage() const;
    QUrl qmlRc() const;

    QQmlListProperty<MediaSource> qmlMediaSourceList();
    QVariant qmlOwner() const;

protected:
    virtual void destroySource(bool destroyBacking, bool asOrphan);

    virtual void notifyContainerContentsChanged(const QSet<DataObject *> *added,
                                                const QSet<DataObject *> *removed);

private:
    Album* m_owner;
    int m_pageNumber;
    AlbumTemplatePage* m_templatePage;
    QList<MediaSource*> m_sourceList;
};

QML_DECLARE_TYPE(AlbumPage);

#endif  // GALLERY_ALBUM_PAGE_H_
