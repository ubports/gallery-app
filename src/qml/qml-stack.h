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

#ifndef GALLERY_QML_STACK_H_
#define GALLERY_QML_STACK_H_

#include <QObject>
#include <QStack>
#include <QVariant>
#include <QtDeclarative>

class QmlStack : public QObject {
  Q_OBJECT
  Q_PROPERTY(int count READ count NOTIFY count_changed)
  Q_PROPERTY(bool isEmpty READ is_empty NOTIFY emptiness_changed)
  Q_PROPERTY(QVariant top READ top NOTIFY count_changed)
  
 signals:
  void count_changed();
  void emptiness_changed();
  
 public:
  QmlStack(QObject* parent = NULL);
  
  static void RegisterType();
  
  Q_INVOKABLE void push(QVariant var);
  Q_INVOKABLE QVariant pop();
  // Returns the item on the stack at the offset index, where the most recently
  // pushed item is index 0 and the item at the bottom of the stack is at
  // count - 1.  Thus, peek(0) will return the same item as top().
  Q_INVOKABLE QVariant peek(int index) const;
  
  bool is_empty() const;
  int count() const;
  QVariant top() const;
  
 private:
  QStack<QVariant> stack_;
};

QML_DECLARE_TYPE(QmlStack)

#endif  // GALLERY_QML_STACK_H_
