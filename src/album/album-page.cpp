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

// media
#include "media-collection.h"

// util
#include "collections.h"
#include "resource.h"

/*!
 * \brief AlbumPage::AlbumPage
 * \param parent
 */
AlbumPage::AlbumPage(QObject * parent)
    : ContainerSource(parent, "AlbumPage", MediaCollection::exposureDateTimeAscendingComparator),
      m_owner(NULL), m_pageNumber(-1), m_templatePage(NULL)
{
}

/*!
 * \brief AlbumPage::AlbumPage
 * \param owner
 */
AlbumPage::AlbumPage(Album* owner)
    : ContainerSource(owner, "AlbumPage", MediaCollection::exposureDateTimeAscendingComparator),
      m_owner(owner), m_pageNumber(-1), m_templatePage(NULL)
{
}

/*!
 * \brief AlbumPage::AlbumPage
 * \param owner
 * \param pageNumber
 * \param templatePage
 */
AlbumPage::AlbumPage(Album* owner, int pageNumber, AlbumTemplatePage* templatePage)
    : ContainerSource(owner, "AlbumPage", MediaCollection::exposureDateTimeAscendingComparator),
      m_owner(owner), m_pageNumber(pageNumber), m_templatePage(templatePage)
{
}

/*!
 * \brief AlbumPage::pageNumber
 * \return
 */
int AlbumPage::pageNumber() const
{
    return m_pageNumber;
}

/*!
 * \brief AlbumPage::templatePage
 * \return
 */
AlbumTemplatePage* AlbumPage::templatePage() const
{
    return m_templatePage;
}

/*!
 * \brief AlbumPage::qmlRc
 * \return
 */
QUrl AlbumPage::qmlRc() const
{
    return Resource::getRcUrl(m_templatePage->qmlRc());
}

/*!
 * \brief AlbumPage::qmlMediaSourceList
 * \return
 */
QQmlListProperty<MediaSource> AlbumPage::qmlMediaSourceList()
{
    return QQmlListProperty<MediaSource>(this, m_sourceList);
}

/*!
 * \brief AlbumPage::qmlOwner
 * \return
 */
QVariant AlbumPage::qmlOwner() const
{
    return QVariant::fromValue(m_owner);
}

/*!
 * \brief AlbumPage::destroySource \reimp
 * \param destroyBacking
 * \param asOrphan
 */
void AlbumPage::destroySource(bool destroyBacking, bool asOrphan)
{
}

/*!
 * \brief AlbumPage::notifyContainerContentsChanged
 * \param added
 * \param removed
 */
void AlbumPage::notifyContainerContentsChanged(const QSet<DataObject *> *added,
                                               const QSet<DataObject *> *removed)
{
    ContainerSource::notifyContainerContentsChanged(added, removed);

    // TODO: Can be done smarter using the added and removed; this will do for now
    m_sourceList = CastListToType<DataObject*, MediaSource*>(contained()->getAll());
    emit mediaSourceListChanged();
}
