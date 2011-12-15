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

#ifndef GALLERY_QML_PAGE_H_
#define GALLERY_QML_PAGE_H_

#include <QObject>
#include <QDeclarativeView>

class QmlPage : public QObject {
  Q_OBJECT
  
 public:
  QmlPage();
  
  virtual const char *qml_rc() const = 0;
  
  virtual void SwitchingTo(QDeclarativeView* view) = 0;
  virtual void SwitchingFrom(QDeclarativeView* view) = 0;
};

#endif  // GALLERY_QML_PAGE_H_
