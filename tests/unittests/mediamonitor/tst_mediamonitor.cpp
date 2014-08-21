/* * Copyright (C) 2014 Canonical, Ltd.  *
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
#include <QTemporaryDir>
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
    QTemporaryDir *m_tmpDir;
    QImage *m_sampleImage;
    MediaMonitor *m_monitor;
};

void tst_MediaMonitor::initTestCase()
{
    m_tmpDir = new QTemporaryDir();

    // Create a simple directories tree
    QDir *dir = new QDir(m_tmpDir->path());
    dir->mkpath("A/A");
    dir->mkpath("A/B");
    dir->mkpath("B/A");
    dir->mkpath("B/B");

    // Create a hidden directory
    dir->mkpath(".H/A");
    dir->mkpath(".H/B");

    // Create symlink
    QFile::link(m_tmpDir->path() + "/.H", m_tmpDir->path() + "L");

    // Create sample image
    m_sampleImage = new QImage(400, 600, QImage::Format_RGB32);
    m_sampleImage->fill(QColor(Qt::red));
}

void tst_MediaMonitor::tst_scanning_sub_folders()
{
    // Create a Media Monitor
    m_monitor = new MediaMonitor();

    // Launch the monitoring process
    m_monitor->startMonitoring(QStringList(m_tmpDir->path()));

    // Save sample image allover the tree
    m_sampleImage->save(m_tmpDir->path() + "/A/A/sample_AA.jpg", "JPG");
    m_sampleImage->save(m_tmpDir->path() + "/A/B/sample_AB.jpg", "JPG");
    m_sampleImage->save(m_tmpDir->path() + "/B/A/sample_BA.jpg", "JPG");
    m_sampleImage->save(m_tmpDir->path() + "/B/B/sample_BB.jpg", "JPG");

    m_sampleImage->save(m_tmpDir->path() + "/.H/A/sample_HA.jpg", "JPG");
    m_sampleImage->save(m_tmpDir->path() + "/.H/B/sample_HB.jpg", "JPG");

    m_sampleImage->save(m_tmpDir->path() + "/A/sample_A.jpg", "JPG");
    m_sampleImage->save(m_tmpDir->path() + "/B/sample_B.jpg", "JPG");

    m_sampleImage->save(m_tmpDir->path() + "/.H/sample_H.jpg", "JPG");

    m_sampleImage->save(m_tmpDir->path() + "/sample.jpg", "JPG");

    QDir *dir = new QDir(m_tmpDir->path());
    dir->mkpath("ND");
    m_sampleImage->save(m_tmpDir->path() + "/ND/sample_ND.jpg", "JPG");

    QTRY_COMPARE_WITH_TIMEOUT(m_monitor->manifest().count(), 8, 10000);
}

void tst_MediaMonitor::cleanupTestCase()
{
    //Remove the previously created files
    QDir *dir = new QDir(m_tmpDir->path());
    dir->removeRecursively();
}

QTEST_MAIN(tst_MediaMonitor)

#include "tst_mediamonitor.moc"
