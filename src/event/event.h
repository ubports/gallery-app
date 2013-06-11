/*
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef GALLERY_EVENT_H_
#define GALLERY_EVENT_H_

// core
#include "container-source.h"

// media
#include "media-source.h"

#include <QDate>
#include <QDateTime>
#include <QObject>

/*!
 * \brief The Event class
 */
class Event : public ContainerSource
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date NOTIFY dateChanged)
    Q_PROPERTY(QDateTime startDateTime READ startDateTime NOTIFY dateChanged)
    Q_PROPERTY(QDateTime endDateTime READ endDateTime NOTIFY dateChanged)

signals:
    void dateChanged();

public:
    Event(QObject* parent = 0);
    explicit Event(QObject* parent, const QDate &date);

    const QDate& date() const;
    QDateTime startDateTime() const;
    QDateTime endDateTime() const;

protected:
    virtual void destroySource(bool destroyBacking, bool asOrphan);

private:
    QDate m_date;
};

QML_DECLARE_TYPE(Event)

#endif  // GALLERY_EVENT_H_
