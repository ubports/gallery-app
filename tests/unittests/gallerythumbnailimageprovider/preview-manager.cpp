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

#include "media/preview-manager.h"

const QString PreviewManager::PREVIEW_DIR = ".thumbs";
const char* PreviewManager::PREVIEW_FILE_EXT = "JPG";

PreviewManager::PreviewManager(const QString &thumbnailDirectory,
                               MediaCollection *mediaCollection, QObject *parent)
{
    Q_UNUSED(thumbnailDirectory);
    Q_UNUSED(mediaCollection)
}

bool PreviewManager::ensurePreview(QFileInfo file, bool regen)
{
    file = QFileInfo();
    regen = false;

    return regen;
}

void PreviewManager::onMediaAddedRemoved(const QSet<DataObject *> *, const QSet<DataObject *> *)
{
}

void PreviewManager::onMediaDestroying(const QSet<DataObject *> *)
{
}

void PreviewManager::updatePreview()
{
}

QString PreviewManager::thumbnailFileName(const QFileInfo& file) const
{
  return QString(file.absolutePath() + PREVIEW_DIR + "/" + file.completeBaseName() + "_th_s." + PREVIEW_FILE_EXT);
}
