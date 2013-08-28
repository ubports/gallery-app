/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <content-communicator.h>

using namespace com::ubuntu::content;

ContentCommunicator::ContentCommunicator(QObject *parent)
    : ImportExportHandler(parent),
      m_transfer(nullptr)
{
}

void ContentCommunicator::handle_import(content::Transfer *)
{
}

void ContentCommunicator::handle_export(content::Transfer *)
{
}

void ContentCommunicator::cancelTransfer()
{
}

void ContentCommunicator::returnPhotos(const QVector<QUrl> &urls)
{
    Q_UNUSED(urls);
}
