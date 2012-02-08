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

#include "qml-stack.h"

QmlStack::QmlStack(QObject* parent)
  : QObject(parent) {
}

void QmlStack::RegisterType() {
  qmlRegisterType<QmlStack>("Gallery", 1, 0, "Stack");
}

void QmlStack::push(QVariant var) {
  bool empty = stack_.isEmpty();
  
  stack_.push(var);
  
  emit count_changed();
  if (empty)
    emit emptiness_changed();
}

QVariant QmlStack::pop() {
  if (stack_.isEmpty())
    return QVariant();
  
  QVariant var = stack_.pop();
  
  emit count_changed();
  if (stack_.isEmpty())
    emit emptiness_changed();
  
  return var;
}

QVariant QmlStack::peek(int index) const {
  return (index < stack_.count()) ? stack_.at(stack_.count() - 1 - index) : QVariant();
}

QVariant QmlStack::top() const {
  return (!stack_.isEmpty()) ? stack_.top() : QVariant();
}

int QmlStack::count() const {
  return stack_.count();
}

bool QmlStack::is_empty() const {
  return stack_.isEmpty();
}
