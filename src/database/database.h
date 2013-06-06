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

#ifndef DATABASE_H
#define DATABASE_H

#include <QFile>
#include <QObject>
#include <QString>

class AlbumTable;
class MediaTable;
class PhotoEditTable;

class QSqlDatabase;
class QSqlQuery;

const qint64 INVALID_ID = -1;

/*!
 * \brief The Database class
 */
class Database : public QObject
{
    Q_OBJECT

public:
    Database(const QString& databaseDir, const QString &schemaDirectory, QObject* parent = 0);

    ~Database();

    void log_sql_error(QSqlQuery& q) const;
    QSqlDatabase* get_db();

    AlbumTable* get_album_table() const;
    MediaTable* get_media_table() const;
    PhotoEditTable* get_photo_edit_table() const;

private:
    bool open_db();

    int get_schema_version() const;
    void set_schema_version(int version);
    void upgrade_schema(int current_version);

    bool execute_sql_file(QFile& file);

    const QString &get_sql_dir() const;

    QString get_db_name() const;
    QString get_db_backup_name() const;

    void restore_from_backup();

    void create_backup();

    QString m_databaseDirectory;
    QString m_sqlSchemaDirectory;
    QSqlDatabase* db_;
    AlbumTable* album_table_;
    MediaTable* media_table_;
    PhotoEditTable* photo_edit_table_;
};

#endif // DATABASE_H
