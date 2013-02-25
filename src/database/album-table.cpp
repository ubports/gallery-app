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
#include "album/album.h"
#include "album/album-default-template.h"
#include "core/gallery-manager.h"

/*!
 * \brief AlbumTable::AlbumTable
 * \param db
 * \param parent
 */
AlbumTable::AlbumTable(Database* db, QObject* parent) : QObject(parent), db_(db)
{
}

/*!
 * \brief AlbumTable::get_albums returns a set of all albums
 * Returns a set of all albums
 * \param album_list
 */
void AlbumTable::get_albums(QList<Album*>* album_list)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT id, title, subtitle, time_added, is_closed, current_page, "
                  "cover_nickname FROM AlbumTable ORDER BY time_added DESC");
    if (!query.exec())
        db_->log_sql_error(query);

    while (query.next()) {
        QDateTime timestamp;

        qint64 id = query.value(0).toLongLong();
        QString title = query.value(1).toString();
        QString subtitle = query.value(2).toString();
        timestamp.setMSecsSinceEpoch(query.value(3).toLongLong());
        bool is_closed = query.value(4).toBool();
        int current_page = query.value(5).toInt();
        QString cover_nickname = query.value(6).toString();

        Album* a = new Album(this, GalleryManager::instance()->album_default_template(), title, subtitle, id,
                             timestamp, is_closed, current_page, cover_nickname);
        album_list->append(a);
    }
}

/*!
 * \brief AlbumTable::add_album adds an album to the DB
 * \param album
 */
void AlbumTable::add_album(Album* album)
{
    if (album->get_id() != INVALID_ID)
        return; // Nothing to do here.

    QSqlQuery query(*db_->get_db());
    query.prepare("INSERT INTO AlbumTable (title, subtitle, time_added, is_closed, "
                  "current_page, cover_nickname) "
                  "VALUES (:title, :subtitle, :time_added, :is_closed, :page, "
                  ":cover_nickname)");
    query.bindValue(":title", album->title());
    query.bindValue(":subtitle", album->subtitle());
    query.bindValue(":time_added", album->creation_date_time().toMSecsSinceEpoch());
    query.bindValue(":is_closed", album->is_closed());
    query.bindValue(":page", album->current_page());
    query.bindValue(":cover_nickname", album->cover_nickname());
    if (!query.exec())
        db_->log_sql_error(query);

    album->set_id(query.lastInsertId().toLongLong());
}

/*!
 * \brief AlbumTable::remove_album removes an album from the DB.
 * \param album
 */
void AlbumTable::remove_album(Album* album)
{
    if (album->get_id() == INVALID_ID)
        return; // Nothing to remove.

    QSqlQuery query(*db_->get_db());
    query.prepare("DELETE FROM AlbumTable WHERE id = :id");
    query.bindValue(":id", album->get_id());
    if (!query.exec())
        db_->log_sql_error(query);

    album->set_id(INVALID_ID);
}

/*!
 * \brief AlbumTable::attach_to_album adds a photo to an album.
 * \param album_id
 * \param media_id
 */
void AlbumTable::attach_to_album(qint64 album_id, qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("INSERT INTO MediaAlbumTable (album_id, media_id) "
                  "VALUES (:album_id, :media_id)");
    query.bindValue(":album_id", album_id);
    query.bindValue(":media_id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief AlbumTable::detach_from_album removes a photo from an album.
 * \param album_id
 * \param media_id
 */
void AlbumTable::detach_from_album(qint64 album_id, qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("DELETE FROM MediaAlbumTable WHERE album_id = :album_id AND "
                  "media_id = :media_id");
    query.bindValue(":album_id", album_id);
    query.bindValue(":media_id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief AlbumTable::media_for_album returns a list of photos for an album.
 * \param album_id
 * \param list
 */
void AlbumTable::media_for_album(qint64 album_id, QList<qint64>* list) const
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT media_id FROM MediaAlbumTable WHERE "
                  "album_id = :album_id");
    query.bindValue(":album_id", album_id);
    if (!query.exec())
        db_->log_sql_error(query);

    while (query.next())
        list->append(query.value(0).toLongLong());
}

/*!
 * \brief AlbumTable::set_is_closed Sets whether or not an album is open
 * \param album_id
 * \param is_closed
 */
void AlbumTable::set_is_closed(qint64 album_id, bool is_closed)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE AlbumTable SET is_closed = :is_closed WHERE "
                  "id = :album_id");
    query.bindValue(":is_closed", is_closed);
    query.bindValue(":album_id", album_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief AlbumTable::set_current_page Sets the current page of the album.
 * \param album_id
 * \param page
 */
void AlbumTable::set_current_page(qint64 album_id, int page)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE AlbumTable SET current_page = :page WHERE "
                  "id = :album_id");
    query.bindValue(":page", page);
    query.bindValue(":album_id", album_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief AlbumTable::set_cover_nickname Sets the cover style for the album.
 * \param album_id
 * \param cover_nickname
 */
void AlbumTable::set_cover_nickname(qint64 album_id, QString cover_nickname)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE AlbumTable SET cover_nickname = :cover_nickname WHERE "
                  "id = :album_id");
    query.bindValue(":cover_nickname", cover_nickname);
    query.bindValue(":album_id", album_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief AlbumTable::set_title Sets the title of the album.
 * \param album_id
 * \param title
 */
void AlbumTable::set_title(qint64 album_id, QString title)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE AlbumTable SET title = :title WHERE "
                  "id = :album_id");
    query.bindValue(":title", title);
    query.bindValue(":album_id", album_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief AlbumTable::set_subtitle Sets the subtitle of the album.
 * \param album_id
 * \param subtitle
 */
void AlbumTable::set_subtitle(qint64 album_id, QString subtitle)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE AlbumTable SET subtitle = :subtitle WHERE "
                  "id = :album_id");
    query.bindValue(":subtitle", subtitle);
    query.bindValue(":album_id", album_id);
    if (!query.exec())
        db_->log_sql_error(query);
}
