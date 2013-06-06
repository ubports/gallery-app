/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Nicolas d'Offay <nicolas.doffay@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QDir>

#include "preview-manager.h"

const int PreviewManager::PREVIEW_WIDTH_MAX = 360;
const int PreviewManager::PREVIEW_HEIGHT_MAX = 360;

const QString PreviewManager::PREVIEW_DIR = ".thumbs";
const char* PreviewManager::PREVIEW_FILE_EXT = "JPG";

bool PreviewManager::ensure_preview_for_media(QFileInfo file, bool regen)
{
    file = QFileInfo();
    regen = false;

    return regen;
}

void PreviewManager::on_media_added_removed(const QSet<DataObject *> *, const QSet<DataObject *> *)
{

}

void PreviewManager::on_media_destroying(const QSet<DataObject *> *)
{

}

void PreviewManager::on_media_data_altered()
{

}

QFileInfo PreviewManager::PreviewFileFor(const QFileInfo &file)
{
    return QFileInfo(file.dir(), PREVIEW_DIR + "/" + file.completeBaseName() + "_th." + PREVIEW_FILE_EXT);
}
