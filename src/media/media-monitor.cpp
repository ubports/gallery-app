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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "media-monitor.h"

#include <QSet>
#include <QString>

/*!
 * \brief MediaMonitor::MediaMonitor
 * \param target_directory
 */
MediaMonitor::MediaMonitor(const QDir& target_directory)
    : target_directory_(target_directory),
      watcher_(QStringList(target_directory.path())),
      manifest_(get_manifest(target_directory)),
      file_activity_timer_(this)
{
    QObject::connect(&watcher_, SIGNAL(directoryChanged(const QString&)), this,
                     SLOT(on_directory_event(const QString&)));

    file_activity_timer_.setSingleShot(true);
    QObject::connect(&file_activity_timer_, SIGNAL(timeout()), this,
                     SLOT(on_file_activity_ceased()));
}

/*!
 * \brief MediaMonitor::~MediaMonitor
 */
MediaMonitor::~MediaMonitor()
{
}

/*!
 * \brief MediaMonitor::on_directory_event
 * \param event_source
 */
void MediaMonitor::on_directory_event(const QString& event_source)
{
    file_activity_timer_.start(100);
}

/*!
 * \brief MediaMonitor::on_file_activity_ceased
 */
void MediaMonitor::on_file_activity_ceased()
{
    QStringList new_manifest = get_manifest(target_directory_);

    QStringList difference = subtract_manifest(new_manifest, manifest_);
    for (int i = 0; i < difference.size(); i++)
        notify_media_item_added(target_directory_.absolutePath() + "/" +
                                difference.at(i));

    manifest_ = new_manifest;
}

/*!
 * \brief MediaMonitor::get_manifest
 * \param dir
 * \return
 */
QStringList MediaMonitor::get_manifest(const QDir& dir)
{
    return dir.entryList(QDir::Files, QDir::Time);
}

/*!
 * \brief MediaMonitor::subtract_manifest
 * \param m1
 * \param m2
 * \return
 */
QStringList MediaMonitor::subtract_manifest(const QStringList& m1,
                                            const QStringList& m2)
{
    QSet<QString> result = QSet<QString>::fromList(m1);

    result.subtract(QSet<QString>::fromList(m2));

    return QStringList(result.toList());
}

/*!
 * \brief MediaMonitor::notify_media_item_added
 * \param item_path
 */
void MediaMonitor::notify_media_item_added(const QString& item_path)
{
    QFileInfo item_info(item_path);

    emit media_item_added(item_info);
}
