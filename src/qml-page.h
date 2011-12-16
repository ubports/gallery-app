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
#include <QDeclarativeItem>
#include <QVariant>

class QmlPage : public QObject {
  Q_OBJECT
  
 public:
  virtual const char *qml_rc() const = 0;
  
  virtual void SwitchingTo() = 0;
  virtual void SwitchingFrom() = 0;
  
  QDeclarativeView* view() const;
  
  void Connect(const char* item_name, const char* signal, const QObject* receiver,
    const char* method, Qt::ConnectionType type = Qt::AutoConnection,
    QDeclarativeItem* parent = NULL) const;
  
  void SetContextProperty(const char* name, QObject* object) const;
  void SetContextProperty(const char* name, const QVariant& variant) const;
  
  bool HasChild(const char* name, QDeclarativeItem* parent = NULL) const;
  
  // Asserts (fails) if the child is not found.  Use HasChild() to determine
  // if the child is available.
  QDeclarativeItem* FindChild(const char *name, QDeclarativeItem* parent = NULL) const;
  
 protected:
  QmlPage(QDeclarativeView* view);
  
 private:
  QDeclarativeView* view_;
  QDeclarativeItem* root_;
};

#endif  // GALLERY_QML_PAGE_H_
