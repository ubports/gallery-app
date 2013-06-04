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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "database.h"
#include "album-table.h"
#include "media-table.h"
#include "photo-edit-table.h"

#include <QFile>
#include <QtSql>
#include <QSqlTableModel>

/*!
 * \brief Database::Database
 * \param databaseDir directory to load/store the database
 * \param schemaDirectory directory of the SQL schema for the database
 * \param parent
 */
Database::Database(const QString &databaseDir, const QString &schemaDirectory,
                   QObject* parent) :
    QObject(parent),
    m_databaseDirectory(databaseDir),
    m_sqlSchemaDirectory(schemaDirectory),
    db_(new QSqlDatabase())
{
    if (!QFile::exists(m_databaseDirectory)) {
        QDir dir;
        bool createOk = dir.mkpath(m_databaseDirectory);
        if (!createOk)
            qWarning() << "Unable to create DB directory" << m_databaseDirectory;
    }

    album_table_ = new AlbumTable(this, this);
    media_table_ = new MediaTable(this, this);
    photo_edit_table_ = new PhotoEditTable(this, this);

    // Open the database.
    if (!open_db())
        restore_from_backup();

    // Attempt a query to make sure the DB is valid.
    QSqlQuery test_query(*db_);
    if (!test_query.exec("SELECT * FROM SQLITE_MASTER LIMIT 1")) {
        log_sql_error(test_query);
        restore_from_backup();
    }

    QSqlQuery query(*db_);
    // Turn synchronous off.
    if (!query.exec("PRAGMA synchronous = OFF")) {
        log_sql_error(query);
        return;
    }

    // Enable foreign keys.
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        log_sql_error(query);
        return;
    }

    // Update if needed.
    upgrade_schema(get_schema_version());
}

/*!
 * \brief Database::~Database
 */
Database::~Database()
{
    delete album_table_;
    delete media_table_;
    delete photo_edit_table_;
    delete db_;

    create_backup();
}

/*!
 * \brief Database::log_sql_error Logs a SQL error
 * \param q
 */
void Database::log_sql_error(QSqlQuery& q) const
{
    qDebug() << "SQLite error: " << q.lastError();
    qDebug() << "SQLite string: " << q.lastQuery();
}

/*!
 * \brief Database::open_db Open the SQLite database
 * \return
 */
bool Database::open_db()
{
    *db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_->setDatabaseName(get_db_name());
    if (!db_->open()) {
        qDebug() << "Error opening DB: " << db_->lastError().text();
        return false;
    }

    return true;
}

/*!
 * \brief Database::get_schema_version Get schema version
 * \return
 */
int Database::get_schema_version() const
{
    QSqlQuery query(*db_);
    if (!query.exec("PRAGMA user_version") || !query.next()) {
        log_sql_error(query);
        return -1;
    }

    return query.value(0).toInt();
}

/*!
 * \brief Database::set_schema_version Set schema version
 * \param version
 */
void Database::set_schema_version(int version)
{
    // Must use string concats here since prepared statements
    // appear not to work with PRAGMAs.
    QSqlQuery query(*db_);
    if (!query.exec("PRAGMA user_version = " + QString::number(version)))
        log_sql_error(query);
}

/*!
 * \brief Database::upgrade_schema Upgrades the schema from current_version to the latest & greatest
 * \param current_version
 */
void Database::upgrade_schema(int current_version)
{
    int version = current_version + 1;
    for (;; version++) {
        // Check for the existence of an updated db file.
        // Filename format is n.sql, where n is the schema version number.
        QFile file(get_sql_dir() + "/" + QString::number(version) + ".sql");
        if (!file.exists())
            return;

        if (!execute_sql_file(file))
            return;

        // Update version.
        set_schema_version(version);
    }
}

/*!
 * \brief Database::execute_sql_file Executes a text file containing SQL commands
 * \param file
 * \return
 */
bool Database::execute_sql_file(QFile& file)
{
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file: " << file.fileName();
        return false;
    }

    // Read entire file into a string.
    QString sql;
    QTextStream in(&file);
    while (!in.atEnd())
        sql += in.readLine() + "\n";
    file.close();

    // Split string at semi-colons to break into multiple statements.
    // This is due to the SQLite driver's inability to handle multiple
    // statements in a single string.
    QStringList statements = sql.split(";", QString::SkipEmptyParts);
    foreach (QString statement, statements) {
        if (statement.trimmed() == "")
            continue;

        // Execute each statement.
        QSqlQuery query(*db_);
        if (!query.exec(statement)) {
            qDebug() << "Error executing database file: " << file.fileName();
            log_sql_error(query);
        }
    }

    return true;
}

/*!
 * \brief Database::get_album_table
 * \return
 */
AlbumTable* Database::get_album_table() const
{
    return album_table_;
}

/*!
 * \brief Database::get_media_table
 * \return
 */
MediaTable* Database::get_media_table() const
{
    return media_table_;
}

/*!
 * \brief Database::get_photo_edit_table
 * \return
 */
PhotoEditTable* Database::get_photo_edit_table() const
{
    return photo_edit_table_;
}

/*!
 * \brief Database::get_db
 * \return
 */
QSqlDatabase* Database::get_db()
{
    return db_;
}

/*!
 * \brief Database::get_sql_dir Returns the directory where the .sql files live
 * \return
 */
const QString& Database::get_sql_dir() const
{
    return m_sqlSchemaDirectory;
}

/*!
* \brief get_db_name
* \return the filename of the database
*/
QString Database::get_db_name() const
{
    return m_databaseDirectory + "/gallery.sqlite";
}

/*!
* \brief get_db_backup_name
* \return the filename for the backup of the database
*/
QString Database::get_db_backup_name() const
{
    return get_db_name() + ".bak";
}

/*!
 * \brief Database::restore_from_backup Restores the database from the auto-backup, if possible
 */
void Database::restore_from_backup()
{
    db_->close();

    // Remove existing DB.
    QFile bad_db(get_db_name());
    if (!bad_db.remove())
        qDebug() << "Could not remove old file.";

    // Copy the backup, if it exists.
    QFile file(get_db_backup_name());
    if (file.exists()) {
        qDebug() << "Restoring database from backup.";
        file.copy(get_db_name());
    }

    open_db();
}

/*!
 * \brief Database::create_backup Creates the auto-backup
 */
void Database::create_backup()
{
    QFile old_backup(get_db_backup_name());
    if (!old_backup.remove())
        qDebug() << "Could not remove existing backup.";

    // Copy the database to the backup.
    QFile file(get_db_name());
    file.copy(get_db_backup_name());
}
