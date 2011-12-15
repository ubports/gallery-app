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

#ifndef GALLERY_QML_AGENT_H_
#define GALLERY_QML_AGENT_H_

#include <QObject>
#include <QDeclarativeView>
#include <QDeclarativeItem>

class QmlAgent : public QObject {
  Q_OBJECT
  
 public:
  static void SetContextProperty(QDeclarativeView* view, const char* name,
    QObject* object);
  static void SetContextProperty(QDeclarativeView* view, const char* name,
    const QVariant& variant);
  
  bool HasChild(const char* name, QDeclarativeItem* parent = NULL) const;
  
  // Asserts (fails) if the child is not found.  Use HasChild() to determine
  // if the child is available.
  QDeclarativeItem* FindChild(const char *name, QDeclarativeItem* parent = NULL) const;
  
 protected:
  explicit QmlAgent(QDeclarativeView* view);
  
 private:
  QDeclarativeItem* root_;
};

#endif  // GALLERY_QML_AGENT_H_
