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

#include "album-template.h"

/*!
 * \brief AlbumTemplate::AlbumTemplate
 * \param name
 */
AlbumTemplate::AlbumTemplate(const char* name)
    : m_name(name)
{
}

/*!
 * \brief AlbumTemplate::pageCount
 * \return
 */
int AlbumTemplate::pageCount() const
{
    return m_pages.count();
}

/*!
 * \brief AlbumTemplate::pages
 * \return
 */
const QList<AlbumTemplatePage*>& AlbumTemplate::pages() const
{
    return m_pages;
}

/*!
 * \brief AlbumTemplate::addPage
 * AlbumTemplate assumes ownership of the AlbumTemplatePage at this point
 * and will delete it when destroyed
 * \param page
 */
void AlbumTemplate::addPage(AlbumTemplatePage* page)
{
    m_pages.append(page);
}
