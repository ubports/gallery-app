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

#include "qml-page.h"

#include <QDeclarativeContext>

QmlPage::QmlPage(QDeclarativeView* view, const char* root_name)
  : view_(view), page_root_name_(root_name) {
}

QDeclarativeView* QmlPage::view() const {
  return view_;
}

const QString& QmlPage::page_root_name() const {
  return page_root_name_;
}

QDeclarativeItem* QmlPage::root_object() const {
  QDeclarativeItem* root = qobject_cast<QDeclarativeItem*>(view_->rootObject());
  Q_ASSERT(root != NULL);
  
  return root;
}

void QmlPage::Entered() {
}

void QmlPage::ReturnedTo() {
}

void QmlPage::Leaving() {
}

void QmlPage::Connect(const char* item_name, const char* signal,
  const QObject* receiver, const char* method, Qt::ConnectionType type,
  QDeclarativeItem* parent) const {
  QObject::connect(FindChild(item_name, parent), signal, receiver, method, type);
}

void QmlPage::SetProperty(const char* item_name, const char* name,
  const QVariant& variant, QDeclarativeItem* parent) const {
  FindChild(item_name, parent)->setProperty(name, variant);
}

void QmlPage::ClearProperty(const char* item_name, const char* name,
  QDeclarativeItem* parent) const {
  SetProperty(item_name, name, QVariant(), parent);
}

void QmlPage::SetContextProperty(const char *name, QObject* object) const {
  view_->rootContext()->setContextProperty(name, object);
}

void QmlPage::SetContextProperty(const char *name, const QVariant &variant) const {
  view_->rootContext()->setContextProperty(name, variant);
}

bool QmlPage::HasChild(const char* name, QDeclarativeItem* parent) const {
  if (parent == NULL)
    parent = root_object();
  
  return parent->findChild<QDeclarativeItem*>(name) != NULL;
}

QDeclarativeItem* QmlPage::FindChild(const char *name, QDeclarativeItem* parent) const {
  if (parent == NULL)
    parent = root_object();
  
  QDeclarativeItem* child = parent->findChild<QDeclarativeItem*>(name);
  Q_ASSERT(child != NULL);
  
  return child;
}
