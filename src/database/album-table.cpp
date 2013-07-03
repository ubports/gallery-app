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

#include <QtSql>

/*!
 * \brief AlbumTable::AlbumTable
 * \param db
 * \param parent
 */
AlbumTable::AlbumTable(Database* db, QObject* parent)
    : QObject(parent),
      m_db(db)
{
}

/*!
 * \brief AlbumTable::get_albums returns a set of all albums
 * Returns a set of all getAlbums
 * \param albumSet
 */
void AlbumTable::getAlbums(QList<Album*>* albumSet)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT id, title, subtitle, time_added, is_closed, current_page, "
                  "cover_nickname FROM AlbumTable ORDER BY time_added DESC");
    if (!query.exec())
        m_db->logSqlError(query);

    while (query.next()) {
        QDateTime timestamp;

        qint64 id = query.value(0).toLongLong();
        QString title = query.value(1).toString();
        QString subtitle = query.value(2).toString();
        timestamp.setMSecsSinceEpoch(query.value(3).toLongLong());
        bool is_closed = query.value(4).toBool();
        int current_page = query.value(5).toInt();
        QString cover_nickname = query.value(6).toString();

        Album* a = new Album(this, title, subtitle, id,
                             timestamp, is_closed, current_page, cover_nickname);
        a->setAlbumTable(this);
        albumSet->append(a);
    }
}

/*!
 * \brief AlbumTable::addAlbum adds an album to the DB
 * \param album
 */
void AlbumTable::addAlbum(Album* album)
{
    if (album->id() != INVALID_ID)
        return; // Nothing to do here.

    QSqlQuery query(*m_db->getDB());
    query.prepare("INSERT INTO AlbumTable (title, subtitle, time_added, is_closed, "
                  "current_page, cover_nickname) "
                  "VALUES (:title, :subtitle, :time_added, :is_closed, :page, "
                  ":cover_nickname)");
    query.bindValue(":title", album->title());
    query.bindValue(":subtitle", album->subtitle());
    query.bindValue(":time_added", album->creationDateTime().toMSecsSinceEpoch());
    query.bindValue(":is_closed", album->isClosed());
    query.bindValue(":page", album->currentPage());
    query.bindValue(":cover_nickname", album->coverNickname());
    if (!query.exec())
        m_db->logSqlError(query);

    album->setId(query.lastInsertId().toLongLong());
}

/*!
 * \brief AlbumTable::removeAlbum removes an album from the DB.
 * \param album
 */
void AlbumTable::removeAlbum(Album* album)
{
    if (album->id() == INVALID_ID)
        return; // Nothing to remove.

    QSqlQuery query(*m_db->getDB());
    query.prepare("DELETE FROM AlbumTable WHERE id = :id");
    query.bindValue(":id", album->id());
    if (!query.exec())
        m_db->logSqlError(query);

    album->setId(INVALID_ID);
}

/*!
 * \brief AlbumTable::isAttachedToAlbum check if a photo is attached to album
 * \param albumId
 * \param mediaId
 */
bool AlbumTable::isAttachedToAlbum(qint64 albumId, qint64 mediaId) const
{
    QSqlQuery query(*m_db->getDB());

    query.prepare("SELECT COUNT(*) FROM MediaAlbumTable WHERE album_id = :album_id AND media_id = :media_id");
    query.bindValue(":album_id", albumId);
    query.bindValue(":media_id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);
    else if (query.next() && (query.value(0).toInt() > 0))
        return true;

    return false;
}

/*!
 * \brief AlbumTable::attachToAlbum adds a photo to an album.
 * \param albumId
 * \param mediaId
 */
void AlbumTable::attachToAlbum(qint64 albumId, qint64 mediaId)
{
    if (isAttachedToAlbum(albumId, mediaId))
        return;

    QSqlQuery query(*m_db->getDB());
    query.prepare("INSERT INTO MediaAlbumTable (album_id, media_id) "
                  "VALUES (:album_id, :media_id)");
    query.bindValue(":album_id", albumId);
    query.bindValue(":media_id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief AlbumTable::detachFromAlbum removes a photo from an album.
 * \param albumId
 * \param mediaId
 */
void AlbumTable::detachFromAlbum(qint64 albumId, qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("DELETE FROM MediaAlbumTable WHERE album_id = :album_id AND "
                  "media_id = :media_id");
    query.bindValue(":album_id", albumId);
    query.bindValue(":media_id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief AlbumTable::mediaForAlbum returns a list of photos for an album.
 * \param albumId
 * \param list
 */
void AlbumTable::mediaForAlbum(qint64 albumId, QList<qint64>* list) const
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT media_id FROM MediaAlbumTable WHERE "
                  "album_id = :album_id");
    query.bindValue(":album_id", albumId);
    if (!query.exec())
        m_db->logSqlError(query);

    while (query.next())
        list->append(query.value(0).toLongLong());
}

/*!
 * \brief AlbumTable::setIsClosed Sets whether or not an album is open
 * \param albumId
 * \param isClosed
 */
void AlbumTable::setIsClosed(qint64 albumId, bool isClosed)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE AlbumTable SET is_closed = :is_closed WHERE "
                  "id = :album_id");
    query.bindValue(":is_closed", isClosed);
    query.bindValue(":album_id", albumId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief AlbumTable::setCurrentPage Sets the current page of the album.
 * \param albumId
 * \param page
 */
void AlbumTable::setCurrentPage(qint64 albumId, int page)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE AlbumTable SET current_page = :page WHERE "
                  "id = :album_id");
    query.bindValue(":page", page);
    query.bindValue(":album_id", albumId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief AlbumTable::setCoverNickname Sets the cover style for the album.
 * \param albumId
 * \param coverNickname
 */
void AlbumTable::setCoverNickname(qint64 albumId, QString coverNickname)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE AlbumTable SET cover_nickname = :cover_nickname WHERE "
                  "id = :album_id");
    query.bindValue(":cover_nickname", coverNickname);
    query.bindValue(":album_id", albumId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief AlbumTable::setTitle Sets the title of the album.
 * \param albumId
 * \param title
 */
void AlbumTable::setTitle(qint64 albumId, QString title)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE AlbumTable SET title = :title WHERE "
                  "id = :album_id");
    query.bindValue(":title", title);
    query.bindValue(":album_id", albumId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief AlbumTable::setSubtitle Sets the subtitle of the album.
 * \param albumId
 * \param subtitle
 */
void AlbumTable::setSubtitle(qint64 albumId, QString subtitle)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE AlbumTable SET subtitle = :subtitle WHERE "
                  "id = :album_id");
    query.bindValue(":subtitle", subtitle);
    query.bindValue(":album_id", albumId);
    if (!query.exec())
        m_db->logSqlError(query);
}
