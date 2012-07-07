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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "photo/photo-caches.h"

#include <QDir>

const QString PhotoCaches::ORIGINAL_DIR = ".original";
const QString PhotoCaches::ENHANCED_DIR = ".enhanced";

PhotoCaches::PhotoCaches(const QFileInfo& file) : file_(file),
    original_file_(file.dir(),
                   QString("%1/%2").arg(ORIGINAL_DIR).arg(file.fileName())),
    enhanced_file_(file.dir(),
                   QString("%1/%2").arg(ENHANCED_DIR).arg(file.fileName())) {
  // We always want our file checks to hit the disk.
  file_.setCaching(false);
  original_file_.setCaching(false);
  enhanced_file_.setCaching(false);
}

bool PhotoCaches::has_cached_original() const {
  return original_file_.exists();
}

bool PhotoCaches::has_cached_enhanced() const {
  return enhanced_file_.exists();
}

const QFileInfo& PhotoCaches::original_file() const {
  return original_file_;
}

const QFileInfo& PhotoCaches::enhanced_file() const {
  return enhanced_file_;
}

bool PhotoCaches::cache_original() {
  if (has_cached_original()) {
    return true;
  }

  file_.dir().mkdir(ORIGINAL_DIR);

  return rename(file_, original_file_);
}

bool PhotoCaches::restore_original() {
  if (!has_cached_original()) {
    return true;
  }

  remove(file_);
  return rename(original_file_, file_);
}

bool PhotoCaches::cache_enhanced_from_original() {
  file_.dir().mkdir(ENHANCED_DIR);

  QFileInfo source = (has_cached_original() ? original_file_ : file_);

  // If called subsequently, the previously cached version is replaced.
  remove(enhanced_file_);
  return copy(source, enhanced_file_);
}

bool PhotoCaches::overwrite_from_cache(bool prefer_enhanced) {
  if (prefer_enhanced && has_cached_enhanced()) {
    remove(file_);
    return copy(enhanced_file_, file_);
  } else if (has_cached_original()) {
    remove(file_);
    return copy(original_file_, file_);
  } else {
    return true;
  }
}

void PhotoCaches::discard_cached_original() {
  remove(original_file_);
}

void PhotoCaches::discard_cached_enhanced() {
  remove(enhanced_file_);
}

void PhotoCaches::discard_all() {
  discard_cached_original();
  discard_cached_enhanced();
}
