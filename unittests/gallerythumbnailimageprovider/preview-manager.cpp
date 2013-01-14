#include "media/preview-manager.h"

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
