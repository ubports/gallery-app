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

#include "album/album-template.h"

/*!
 * \brief AlbumTemplate::AlbumTemplate
 * \param name
 */
AlbumTemplate::AlbumTemplate(const char* name)
  : name_(name)
{
}

/*!
 * \brief AlbumTemplate::page_count
 * \return
 */
int AlbumTemplate::page_count() const
{
  return pages_.count();
}

/*!
 * \brief AlbumTemplate::pages
 * \return
 */
const QList<AlbumTemplatePage*>& AlbumTemplate::pages() const
{
  return pages_;
}

/*!
 * \brief AlbumTemplate::AddPage
 * AlbumTemplate assumes ownership of the AlbumTemplatePage at this point
 * and will delete it when destroyed
 * \param page
 */
void AlbumTemplate::AddPage(AlbumTemplatePage* page)
{
  pages_.append(page);
}
