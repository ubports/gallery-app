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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "media-monitor.h"

#include <QStringList>
#include <iostream>

MediaMonitor::MediaMonitor(const QDir& target_directory)
  : target_directory_(target_directory),
    watcher_(QStringList(target_directory.path())),
    manifest_(get_manifest(target_directory)) {
  QObject::connect(&watcher_, SIGNAL(directoryChanged(const QString&)), this,
    SLOT(on_directory_event(const QString&)));
}

MediaMonitor::~MediaMonitor() {
}

void MediaMonitor::on_directory_event(const QString& event_source) {
  QStringList new_manifest = get_manifest(target_directory_);
  
  QStringList difference;
  
  // don't assume a new file has been added to the ~/Pictures directory unless
  // subtract_manifest() returns true. only when subtract_manifest() is true
  // are we assured that 'new_manifest' is a true supersequence of 'manifest_'
  // (and hence contains entries that 'manifest_' does not, but not vice-versa).
  // See the comment accompanying the definition of subtract_manifest() for
  // more information.
  if (subtract_manifest(new_manifest, manifest_, difference)) {
    for (int i = 0; i < difference.size(); i++)
      notify_media_item_added(event_source + "/" + difference.at(i));
  }

  manifest_ = new_manifest;
}

QStringList MediaMonitor::get_manifest(const QDir& dir) {
  return dir.entryList(QDir::Files, QDir::Time);
}

// Attempt to subtract m2 from m1. Return true if subtraction was possible and
// populate empty list 'difference' with subtraction results. Return false if
// subtraction wasn't possible.
//
// NOTE: out-parameter 'difference' should only be used when this function
// returns true. This function has strict pre-conditions that must be met and
// it will return false and leave 'difference' in an undefined state when these
// pre-conditions aren't met. The reason for this is as follows. Lots of things
// can happen to file manifests, but sequence subtraction is possible only
// when m2 is a true subsequence of m1. To flesh this out a bit,
// substraction is possible only when, for all files f in m2, f is
// also in m1. Likewise for two files f1 and f2 in m1 and m2, if f1 precedes
// f2 in m1 then f1 also precedes f2 in m2, and conversely.
bool MediaMonitor::subtract_manifest(const QStringList& m1,
  const QStringList& m2, QStringList& difference) {
  if (m1.size() < m2.size())
    return false;
  
  if (difference.size() != 0)
    return false;
    
  int m1_iter = 0;
  int m2_iter = 0;
  while (m1_iter < m1.size()) {
    if (m2_iter < m2.size() && m1.at(m1_iter) == m2.at(m2_iter)) {
      // most common case: m1 and m2 have the same entry at the current position
      // of their respective iterators
      m1_iter++;
      m2_iter++;
    } else {
      // accumulate and advance m1_iter only; m1 is allowed to have entries that
      // m2 does not
      difference.append(m1.at(m1_iter));
      m1_iter++;
    }
  }
  
  // if both iterators haven't reached the end of their respective manifests,
  // then list m2 must have an entry that list m1 does not. This means that
  // list m2 is not a subsequence of list m1 and hence subtraction is
  // impossible.
  return (m1_iter == m1.size() && m2_iter == m2.size());
}

void MediaMonitor::notify_media_item_added(const QString& item_path) {
  QFileInfo item_info(item_path);
  
  if (item_info.size() == 0)
    return;
  
  emit media_item_added(item_info);
}
