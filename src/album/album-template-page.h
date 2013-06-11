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
 * Charles Lindsay <chaz@yorba.org>
 */

#ifndef GALLERY_ALBUM_TEMPLATE_PAGE_H_
#define GALLERY_ALBUM_TEMPLATE_PAGE_H_

#include <QObject>
#include <QList>
#include <QString>

enum PageOrientation {
    PORTRAIT,
    LANDSCAPE,
    SQUARE
};

/*!
 * \brief The AlbumTemplatePage class
 */
class AlbumTemplatePage : public QObject
{
    Q_OBJECT

public:
    AlbumTemplatePage(const char* name, const char* qmlRc, bool isLeft,
                      int frameCount, ...);

    const QString& name() const;
    const QString& qmlRc() const;
    bool isLeft() const;

    int frameCount() const;
    int framesFor(PageOrientation orientation) const;
    const QList<PageOrientation>& layout() const;

private:
    QString m_name;
    QString m_qmlRc;
    bool m_isLeft;
    QList<PageOrientation> m_layout;
};

#endif  // GALLERY_ALBUM_TEMPLATE_PAGE_H_
