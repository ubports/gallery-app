/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#include <QtTest>

#include <QDir>
#include <QImage>
#include <QColor>
#include <QStringList>

#include "media-monitor.h"

class tst_MediaMonitor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void tst_scanning_sub_folders();
    void cleanupTestCase();

private:
    QString m_tmpPath;
    QImage *m_sampleImage;
    MediaMonitor *m_monitor;
};

void tst_MediaMonitor::initTestCase()
{
    m_tmpPath = QString("/tmp/pics/");

    // Create a simple directories tree
    QDir *dir = new QDir(m_tmpPath);
    dir->mkpath("A/A");
    dir->mkpath("A/B");
    dir->mkpath("B/A");
    dir->mkpath("B/B");

    // Create sample image
    m_sampleImage = new QImage(400, 600, QImage::Format_RGB32);
    m_sampleImage->fill(QColor(Qt::red));
}

void tst_MediaMonitor::tst_scanning_sub_folders()
{
    // Create a Media Monitor
    m_monitor = new MediaMonitor();

    // Emitted every time a new photo/video found
    QSignalSpy filesFound(m_monitor, SIGNAL(mediaItemAdded(QString)));

    // Launch the monitoring process
    m_monitor->startMonitoring(QStringList(m_tmpPath));


    // Save sample image allover the tree
    m_sampleImage->save(m_tmpPath + "A/A/sample_AA.jpg", "JPG");
    m_sampleImage->save(m_tmpPath + "A/B/sample_AB.jpg", "JPG");
    m_sampleImage->save(m_tmpPath + "B/A/sample_BA.jpg", "JPG");
    m_sampleImage->save(m_tmpPath + "B/B/sample_BB.jpg", "JPG");

    m_sampleImage->save(m_tmpPath + "A/sample_A.jpg", "JPG");
    m_sampleImage->save(m_tmpPath + "B/sample_B.jpg", "JPG");

    m_sampleImage->save(m_tmpPath + "sample.jpg", "JPG");

    QTRY_COMPARE(filesFound.count(), 7);
}

void tst_MediaMonitor::cleanupTestCase()
{
    //Remove the previously created files
    QDir *dir = new QDir(m_tmpPath);
    dir->removeRecursively();
}

QTEST_MAIN(tst_MediaMonitor)

#include "tst_mediamonitor.moc"
