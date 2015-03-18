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

    void logSqlError(QSqlQuery& q) const;
    QSqlDatabase* getDB();

    AlbumTable* getAlbumTable() const;
    MediaTable* getMediaTable() const;

private:
    bool openDB();

    int schemaVersion() const;
    void setSchemaVersion(int version);
    void upgradeSchema(int current_version);

    bool executeSqlFile(QFile& file);

    const QString &getSqlDir() const;

    QString getDBname() const;
    QString getDBBackupName() const;

    void restoreFromBackup();

    void createBackup();

    QString m_databaseDirectory;
    QString m_sqlSchemaDirectory;
    QSqlDatabase* m_db;
    AlbumTable* m_albumTable;
    MediaTable* m_mediaTable;
};

#endif // DATABASE_H
