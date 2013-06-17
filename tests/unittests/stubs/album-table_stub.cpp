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
 * Eric Gregory <eric@yorba.org>
 */

#include "album-table.h"
#include "database.h"

// album
#include "album.h"
#include "album-default-template.h"

AlbumTable::AlbumTable(Database* db, QObject* parent)
    : QObject(parent),
      m_db(db)
{
}

void AlbumTable::getAlbums(QList<Album*>* albumSet)
{
    Q_UNUSED(albumSet);
}

void AlbumTable::addAlbum(Album* album)
{
    Q_UNUSED(album);
}

void AlbumTable::removeAlbum(Album* album)
{
    Q_UNUSED(album);
}

void AlbumTable::attachToAlbum(qint64 albumId, qint64 mediaId)
{
    Q_UNUSED(albumId);
    Q_UNUSED(mediaId);
}

void AlbumTable::detachFromAlbum(qint64 albumId, qint64 mediaId)
{
    Q_UNUSED(albumId);
    Q_UNUSED(mediaId);
}

void AlbumTable::mediaForAlbum(qint64 albumId, QList<qint64>* list) const
{
    Q_UNUSED(albumId);
    Q_UNUSED(list);
}

void AlbumTable::setIsClosed(qint64 albumId, bool isClosed)
{
    Q_UNUSED(albumId);
    Q_UNUSED(isClosed);
}

void AlbumTable::setCurrentPage(qint64 albumId, int page)
{
    Q_UNUSED(albumId);
    Q_UNUSED(page);
}

void AlbumTable::setCoverNickname(qint64 albumId, QString coverNickname)
{
    Q_UNUSED(albumId);
    Q_UNUSED(coverNickname);
}

void AlbumTable::setTitle(qint64 albumId, QString title)
{
    Q_UNUSED(albumId);
    Q_UNUSED(title);
}

void AlbumTable::setSubtitle(qint64 albumId, QString subtitle)
{
    Q_UNUSED(albumId);
    Q_UNUSED(subtitle);
}

