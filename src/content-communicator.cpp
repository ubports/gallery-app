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

#include <QApplication>
#include <QDebug>
#include <QFileInfo>

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/transfer.h>

using namespace com::ubuntu::content;

/*!
 * \brief ContentCommunicator::ContentCommunicator
 * \param parent
 */
ContentCommunicator::ContentCommunicator(QObject *parent)
    : ImportExportHandler(parent),
      m_transfer(nullptr)
{
    Hub *hub = Hub::Client::instance();
    hub->register_import_export_handler(this);
}

/*!
 * \brief \reimp
 */
void ContentCommunicator::handle_import(content::Transfer *)
{
    qDebug() << Q_FUNC_INFO << "gallery does not import content";
}

/*!
 * \brief \reimp
 */
void ContentCommunicator::handle_export(content::Transfer *transfer)
{
    if (m_transfer != nullptr) {
        qWarning() << "gallery-app does only one content export at a time";
        transfer->abort();
        return;
    }

    m_transfer = transfer;
    emit photoRequested();
    emit selectionTypeChanged();
    emit singleContentPickModeChanged();

    QObject::connect(m_transfer, SIGNAL(stateChanged()), this, SLOT(quitApp()));
}

/*!
 * \brief ContentCommunicator::cancelTransfer aborts the current transfer
 */
void ContentCommunicator::cancelTransfer()
{
    if (!m_transfer) {
        qWarning() << "No ongoing transfer to cancel";
        return;
    }

    m_transfer->abort();
    m_transfer = nullptr;
}

/*!
 * \brief ContentCommunicator::returnPhoto returns the given photos
 * via content hub to the requester
 * \param urls
 */
void ContentCommunicator::returnPhotos(const QVector<QUrl> &urls)
{
    if (!m_transfer) {
        qWarning() << "No ongoing transfer to return a photo";
        return;
    }

    QVector<Item> items;
    items.reserve(urls.size());
    foreach (const QUrl &url, urls) {
        items.append(Item(url));
    }
    m_transfer->charge(items);
    m_transfer = nullptr;
}

/*!
 * \brief ContentCommunicator::selectionType return if the transfer requests
 * one single item only, or multiple
 * \return
 */
ContentCommunicator::SelectionType ContentCommunicator::selectionType() const
{
    if (!m_transfer)
        return SingleSelect;

    return static_cast<SelectionType>(m_transfer->selectionType());
}

/*!
 * \brief ContentCommunicator::singleContentPickMode
 * \return
 */
bool ContentCommunicator::singleContentPickMode() const
{
    if (!m_transfer)
        return SingleSelect;

    return m_transfer->selectionType() == Transfer::single;
}

/*!
 * \brief ContentCommunicator::quitApp
 * FIXME
 * As long as the content hub soes not close gallery -use this as fallback
 */
void ContentCommunicator::quitApp()
{
    if (!m_transfer)
        qApp->quit();
}
