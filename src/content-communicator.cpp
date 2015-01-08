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

// medialoader
#include "photo-metadata.h"

using namespace com::ubuntu::content;

/*!
 * \brief ContentCommunicator::ContentCommunicator
 * \param parent
 */
ContentCommunicator::ContentCommunicator(QObject *parent)
    : ImportExportHandler(parent),
      m_transfer(nullptr)
{
}

/*!
 * \brief ContentCommunicator::registerWithHub Register the handlers provided
 * by ContentCommunicator with the content hub
 */
void ContentCommunicator::registerWithHub()
{
    Hub *hub = Hub::Client::instance();
    hub->register_import_export_handler(this);
}

/*!
 * \brief \reimp
 */
void ContentCommunicator::handle_import(content::Transfer *transfer)
{
    QVector<Item> transferedItems = transfer->collect();
    foreach (const Item &hubItem, transferedItems) {
        QFileInfo fi(hubItem.url().toLocalFile());
        QString filename = fi.fileName();
        QString dir;
        QMimeDatabase mdb;
        QMimeType mt = mdb.mimeTypeForFile(hubItem.url().toLocalFile());
        QString suffix = fi.completeSuffix();
        QString filenameWithoutSuffix = filename.left(filename.size() - suffix.size());
        if(suffix.isEmpty()) {
            // If the filename doesn't have an extension add one from the
            // detected mimetype
            if(!mt.preferredSuffix().isEmpty()) {
                suffix = mt.preferredSuffix();
                filenameWithoutSuffix += ".";
            }
        }
        if(mt.name().startsWith("video/")) {
            dir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator();
        } else {
            dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator();
        }
        QString destination = QString("%1%2").arg(dir + filenameWithoutSuffix, suffix);
        // If we already have a file of this name reformat to "filename.x.png"
        // (where x is a number, incremented until we find an available filename)
        if(QFile::exists(destination)) {
            int append = 1;
            do {
                destination = QString("%1%2.%3").arg(dir + filenameWithoutSuffix, QString::number(append), suffix);
                append++;
            } while(QFile::exists(destination));
        }
        QFile::copy(hubItem.url().toLocalFile(), destination);

        if(!mt.name().startsWith("video/")) {
            // Set the TimeDigitized field of the Exif data to make imported photos
            // to show up on imported date Event
            QFileInfo destfi(destination);
            PhotoMetadata* metadata = PhotoMetadata::fromFile(destfi);
            if (metadata != NULL) {
                metadata->setDateTimeDigitized(QDateTime::currentDateTime());
                metadata->save();
            }
        }
    }
    // Allow content-hub to clean up temporary files in .cache/ once we've
    // moved them
    transfer->finalize();

    emit mediaImported();
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
    emit mediaRequested(transfer->contentType());
    emit selectionTypeChanged();
    emit singleContentPickModeChanged();
}

/*!
 * \brief \reimp
 */
void ContentCommunicator::handle_share(content::Transfer *)
{
    qDebug() << Q_FUNC_INFO << "gallery does not share content";
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
        return true;

    return m_transfer->selectionType() == Transfer::SelectionType::single;
}
