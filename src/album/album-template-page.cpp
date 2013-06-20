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

#include "album-template-page.h"

/*!
 * \brief AlbumTemplatePage::AlbumTemplatePage
 * \param name
 * \param qmlRc
 * \param isLeft determines whether the page is meant to be displayed on the right or left of a spread.
 * \param frame_count
 * Final arguments are a list of PageOrientation enums that describe the page's frames (slots) from top to bottom, left to right.
 */
AlbumTemplatePage::AlbumTemplatePage(const char* name, const char* qmlRc,
                                     bool isLeft, int frameCount, ...)
    : m_name(name), m_isLeft(isLeft)
{
    Q_ASSERT(frameCount >= 0);

    va_list valist;
    va_start(valist, frameCount);
    for (int ctr = 0; ctr < frameCount; ctr++)
        m_layout.append(static_cast<PageOrientation>(va_arg(valist, int)));
    va_end(valist);

    m_qmlRc = QString(qmlRc);
}

/*!
 * \brief AlbumTemplatePage::name
 * \return
 */
const QString& AlbumTemplatePage::name() const
{
    return m_name;
}

/*!
 * \brief AlbumTemplatePage::qmlRc
 * \return
 */
const QString& AlbumTemplatePage::qmlRc() const
{
    return m_qmlRc;
}

/*!
 * \brief AlbumTemplatePage::isLeft
 * \return
 */
bool AlbumTemplatePage::isLeft() const
{
    return m_isLeft;
}

/*!
 * \brief AlbumTemplatePage::frameCount
 * \return
 */
int AlbumTemplatePage::frameCount() const
{
    return m_layout.count();
}

/*!
 * \brief AlbumTemplatePage::framesFor
 * \param orientation
 * \return
 */
int AlbumTemplatePage::framesFor(PageOrientation orientation) const
{
    int count = 0;
    PageOrientation o;
    foreach (o, m_layout) {
        if (o == orientation)
            count++;
    }

    return count;
}

/*!
 * \brief AlbumTemplatePage::layout
 * \return
 */
const QList<PageOrientation>& AlbumTemplatePage::layout() const
{
    return m_layout;
}
