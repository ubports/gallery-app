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

    void get_albums(QList<Album*>* album_set);

    void add_album(Album* album);
    void remove_album(Album* album);

    void attach_to_album(qint64 album_id, qint64 media_id);
    void detach_from_album(qint64 album_id, qint64 media_id);

    void media_for_album(qint64 album_id, QList<qint64>* list) const;

    void set_is_closed(qint64 album_id, bool is_closed);

    void set_current_page(qint64 album_id, int page);

    void set_cover_nickname(qint64 album_id, QString cover_nickname);
    void set_title(qint64 album_id, QString title);
    void set_subtitle(qint64 album_id, QString subtitle);

private:
    Database* db_;

    bool is_attached_to_album(qint64 album_id, qint64 media_id) const;
};

#endif // ALBUMTABLE_H
