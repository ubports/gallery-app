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
    Q_PROPERTY(QDate date READ date NOTIFY date_altered)
    Q_PROPERTY(QDateTime startDateTime READ start_date_time NOTIFY date_altered)
    Q_PROPERTY(QDateTime endDateTime READ end_date_time NOTIFY date_altered)

signals:
    void date_altered();

public:
    Event(QObject* parent = 0);
    explicit Event(QObject* parent, const QDate &date);

    static void RegisterType();

    const QDate& date() const;
    QDateTime start_date_time() const;
    QDateTime end_date_time() const;

protected:
    virtual void DestroySource(bool destroy_backing, bool as_orphan);

private:
    QDate date_;
};

QML_DECLARE_TYPE(Event)

#endif  // GALLERY_EVENT_H_
