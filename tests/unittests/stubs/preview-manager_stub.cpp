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
#include "media-collection.h"

const int PreviewManager::PREVIEW_SIZE = 360;
const QString PreviewManager::PREVIEW_DIR = ".thumbs";
const char* PreviewManager::PREVIEW_FILE_EXT = "JPG";

PreviewManager::PreviewManager(const QString &thumbnailDirectory, QObject *parent)
{
    Q_UNUSED(thumbnailDirectory);
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

QString PreviewManager::previewFileName(const QFileInfo &file) const
{
    return QString(file.absolutePath() + QDir::separator() + PREVIEW_DIR +
                   QDir::separator() + file.completeBaseName() + "_th." + PREVIEW_FILE_EXT);
}

QString PreviewManager::thumbnailFileName(const QFileInfo& file) const
{
  return QString(file.absolutePath() + PREVIEW_DIR + QDir::separator() +
                 file.completeBaseName() + "_th_s." + PREVIEW_FILE_EXT);
}
