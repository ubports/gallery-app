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

#include <QFile>
#include <QSqlTableModel>
#include <QtSql>

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
    m_db(new QSqlDatabase())
{
    if (!QFile::exists(m_databaseDirectory)) {
        QDir dir;
        bool createOk = dir.mkpath(m_databaseDirectory);
        if (!createOk)
            qWarning() << "Unable to create DB directory" << m_databaseDirectory;
    }

    m_albumTable = new AlbumTable(this, this);
    m_mediaTable = new MediaTable(this, this);

    // Open the database.
    if (!openDB())
        restoreFromBackup();

    // Attempt a query to make sure the DB is valid.
    QSqlQuery test_query(*m_db);
    if (!test_query.exec("SELECT * FROM SQLITE_MASTER LIMIT 1")) {
        logSqlError(test_query);
        restoreFromBackup();
    }

    QSqlQuery query(*m_db);
    // Turn synchronous off.
    if (!query.exec("PRAGMA synchronous = OFF")) {
        logSqlError(query);
        return;
    }

    // Enable foreign keys.
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        logSqlError(query);
        return;
    }

    // Update if needed.
    upgradeSchema(schemaVersion());
}

/*!
 * \brief Database::~Database
 */
Database::~Database()
{
    delete m_albumTable;
    delete m_mediaTable;
    delete m_db;

    createBackup();
}

/*!
 * \brief Database::logSqlError Logs a SQL error
 * \param q
 */
void Database::logSqlError(QSqlQuery& q) const
{
    qDebug() << "SQLite error: " << q.lastError();
    qDebug() << "SQLite string: " << q.lastQuery();
}

/*!
 * \brief Database::openDB Open the SQLite database
 * \return
 */
bool Database::openDB()
{
    *m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db->setDatabaseName(getDBname());
    if (!m_db->open()) {
        qDebug() << "Error opening DB: " << m_db->lastError().text();
        return false;
    }

    return true;
}

/*!
 * \brief Database::schemaVersion Get schema version
 * \return
 */
int Database::schemaVersion() const
{
    QSqlQuery query(*m_db);
    if (!query.exec("PRAGMA user_version") || !query.next()) {
        logSqlError(query);
        return -1;
    }

    return query.value(0).toInt();
}

/*!
 * \brief Database::setSchemaVersion Set schema version
 * \param version
 */
void Database::setSchemaVersion(int version)
{
    // Must use string concats here since prepared statements
    // appear not to work with PRAGMAs.
    QSqlQuery query(*m_db);
    if (!query.exec("PRAGMA user_version = " + QString::number(version)))
        logSqlError(query);
}

/*!
 * \brief Database::upgradeSchema Upgrades the schema from current_version to
 * the latest & greatest
 * FIXME this should upgrade the whole DB (every row of every table)
 * \param current_version
 */
void Database::upgradeSchema(int current_version)
{
    int version = current_version + 1;
    for (;; version++) {
        // Check for the existence of an updated db file.
        // Filename format is n.sql, where n is the schema version number.
        QFile file(getSqlDir() + QDir::separator() + QString::number(version) + ".sql");
        if (!file.exists())
            return;

        if (!executeSqlFile(file))
            return;

        // Update version.
        setSchemaVersion(version);
    }
}

/*!
 * \brief Database::executeSqlFile Executes a text file containing SQL commands
 * \param file
 * \return
 */
bool Database::executeSqlFile(QFile& file)
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
        QSqlQuery query(*m_db);
        if (!query.exec(statement)) {
            qDebug() << "Error executing database file: " << file.fileName();
            logSqlError(query);
        }
    }

    return true;
}

/*!
 * \brief Database::getAlbumTable
 * \return
 */
AlbumTable* Database::getAlbumTable() const
{
    return m_albumTable;
}

/*!
 * \brief Database::getMediaTable
 * \return
 */
MediaTable* Database::getMediaTable() const
{
    return m_mediaTable;
}

/*!
 * \brief Database::getDB
 * \return
 */
QSqlDatabase* Database::getDB()
{
    return m_db;
}

/*!
 * \brief Database::getSqlDir Returns the directory where the .sql files live
 * \return
 */
const QString& Database::getSqlDir() const
{
    return m_sqlSchemaDirectory;
}

/*!
* \brief get_db_name
* \return the filename of the database
*/
QString Database::getDBname() const
{
    return m_databaseDirectory + "/gallery.sqlite";
}

/*!
* \brief get_db_backup_name
* \return the filename for the backup of the database
*/
QString Database::getDBBackupName() const
{
    return getDBname() + ".bak";
}

/*!
 * \brief Database::restore_from_backup Restores the database from the auto-backup, if possible
 */
void Database::restoreFromBackup()
{
    m_db->close();

    // Remove existing DB.
    QFile bad_db(getDBname());
    if (!bad_db.remove())
        qDebug() << "Could not remove old file.";

    // Copy the backup, if it exists.
    QFile file(getDBBackupName());
    if (file.exists()) {
        qDebug() << "Restoring database from backup.";
        file.copy(getDBname());
    }

    openDB();
}

/*!
 * \brief Database::create_backup Creates the auto-backup
 */
void Database::createBackup()
{
    QFile old_backup(getDBBackupName());
    if (!old_backup.remove())
        qDebug() << "Could not remove existing backup.";

    // Copy the database to the backup.
    QFile file(getDBname());
    file.copy(getDBBackupName());
}
