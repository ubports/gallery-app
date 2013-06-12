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

#ifndef ALBUMTABLE_H
#define ALBUMTABLE_H

#include <QList>
#include <QObject>

class Album;
class Database;

/*!
 * \brief The AlbumTable class
 */
class AlbumTable : public QObject
{
    Q_OBJECT

public:
    explicit AlbumTable(Database* db, QObject* parent = 0);

    void getAlbums(QList<Album*>* albumSet);

    void addAlbum(Album* album);
    void removeAlbum(Album* album);

    void attachToAlbum(qint64 albumId, qint64 mediaId);
    void detachFromAlbum(qint64 albumId, qint64 mediaId);

    void mediaForAlbum(qint64 albumId, QList<qint64>* list) const;

    void setIsClosed(qint64 albumId, bool isClosed);

    void setCurrentPage(qint64 albumId, int page);

    void setCoverNickname(qint64 albumId, QString coverNickname);
    void setTitle(qint64 albumId, QString title);
    void setSubtitle(qint64 albumId, QString subtitle);

private:
    Database* m_db;

    bool isAttachedToAlbum(qint64 albumId, qint64 mediaId) const;
};

#endif // ALBUMTABLE_H
