/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 */

#include "album-page.h"
#include "album.h"
#include "media/media-collection.h"
#include "util/collections.h"
#include "util/resource.h"
#include "gallery-manager.h"

/*!
 * \brief AlbumPage::AlbumPage
 * \param parent
 */
AlbumPage::AlbumPage(QObject * parent)
    : ContainerSource(parent, "AlbumPage", MediaCollection::ExposureDateTimeAscendingComparator),
      owner_(NULL), page_number_(-1), template_page_(NULL)
{
}

/*!
 * \brief AlbumPage::AlbumPage
 * \param owner
 */
AlbumPage::AlbumPage(Album* owner)
    : ContainerSource(owner, "AlbumPage", MediaCollection::ExposureDateTimeAscendingComparator),
      owner_(owner), page_number_(-1), template_page_(NULL)
{
}

/*!
 * \brief AlbumPage::AlbumPage
 * \param owner
 * \param page_number
 * \param template_page
 */
AlbumPage::AlbumPage(Album* owner, int page_number, AlbumTemplatePage* template_page)
    : ContainerSource(owner, "AlbumPage", MediaCollection::ExposureDateTimeAscendingComparator),
      owner_(owner), page_number_(page_number), template_page_(template_page)
{
}

/*!
 * \brief AlbumPage::RegisterType
 */
void AlbumPage::RegisterType()
{
    qmlRegisterType<AlbumPage>("Gallery", 1, 0, "AlbumPage");
}

/*!
 * \brief AlbumPage::page_number
 * \return
 */
int AlbumPage::page_number() const
{
    return page_number_;
}

/*!
 * \brief AlbumPage::template_page
 * \return
 */
AlbumTemplatePage* AlbumPage::template_page() const
{
    return template_page_;
}

/*!
 * \brief AlbumPage::qml_rc
 * \return
 */
QUrl AlbumPage::qml_rc() const
{
    return Resource::get_rc_url(template_page_->qml_rc());
}

/*!
 * \brief AlbumPage::qml_media_source_list
 * \return
 */
QQmlListProperty<MediaSource> AlbumPage::qml_media_source_list()
{
    return QQmlListProperty<MediaSource>(this, source_list_);
}

/*!
 * \brief AlbumPage::qml_owner
 * \return
 */
QVariant AlbumPage::qml_owner() const
{
    return QVariant::fromValue(owner_);
}

/*!
 * \brief AlbumPage::DestroySource \reimp
 * \param destroy_backing
 * \param as_orphan
 */
void AlbumPage::DestroySource(bool destroy_backing, bool as_orphan)
{
}

/*!
 * \brief AlbumPage::notify_container_contents_altered
 * \param added
 * \param removed
 */
void AlbumPage::notify_container_contents_altered(const QSet<DataObject *> *added,
                                                  const QSet<DataObject *> *removed)
{
    ContainerSource::notify_container_contents_altered(added, removed);

    // TODO: Can be done smarter using the added and removed; this will do for now
    source_list_ = CastListToType<DataObject*, MediaSource*>(contained()->GetAll());
    emit media_source_list_changed();
}
