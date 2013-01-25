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

/*!
 * \brief PhotoCaches::PhotoCaches
 * \param file
 */
PhotoCaches::PhotoCaches(const QFileInfo& file) : file_(file),
    original_file_(file.dir(),
                   QString("%1/%2").arg(ORIGINAL_DIR).arg(file.fileName())),
    enhanced_file_(file.dir(),
                   QString("%1/%2").arg(ENHANCED_DIR).arg(file.fileName()))
{
  // We always want our file checks to hit the disk.
  file_.setCaching(false);
  original_file_.setCaching(false);
  enhanced_file_.setCaching(false);
}

/*!
 * \brief PhotoCaches::has_cached_original
 * \return
 */
bool PhotoCaches::has_cached_original() const
{
  return original_file_.exists();
}

/*!
 * \brief PhotoCaches::has_cached_enhanced
 * \return
 */
bool PhotoCaches::has_cached_enhanced() const
{
  return enhanced_file_.exists();
}

/*!
 * \brief PhotoCaches::original_file
 * \return
 */
const QFileInfo& PhotoCaches::original_file() const
{
  return original_file_;
}

/*!
 * \brief PhotoCaches::enhanced_file
 * \return
 */
const QFileInfo& PhotoCaches::enhanced_file() const
{
  return enhanced_file_;
}

/*!
 * \brief PhotoCaches::pristine_file
 * Returns original_file() if it exists; otherwise, returns the file passed
 * to the constructor.
 * \return
 */
const QFileInfo& PhotoCaches::pristine_file() const
{
  return (has_cached_original() ? original_file_ : file_);
}

/*!
 * \brief PhotoCaches::cache_original
 * Moves the pristine file into .original so we don't mess it up.  Note that
 * this potentially removes the main file, so it must be followed by a copy
 * from original (or elsewhere) back to the file.
 * \return
 */
bool PhotoCaches::cache_original()
{
  if (has_cached_original()) {
    return true;
  }

  file_.dir().mkdir(ORIGINAL_DIR);

  return rename(file_, original_file_);
}

/*!
 * \brief PhotoCaches::restore_original
 * Moves the file out of .original, overwriting the main file.  Note that
 * this removes the .original file.
 * \return
 */
bool PhotoCaches::restore_original()
{
  if (!has_cached_original()) {
    return true;
  }

  remove(file_);
  return rename(original_file_, file_);
}

/*!
 * \brief PhotoCaches::cache_enhanced_from_original
 * Copies the file in .original to .enhanced so it can then be enhanced.
 * \return
 */
bool PhotoCaches::cache_enhanced_from_original()
{
  file_.dir().mkdir(ENHANCED_DIR);

  // If called subsequently, the previously cached version is replaced.
  remove(enhanced_file_);
  return copy(pristine_file(), enhanced_file_);
}

/*!
 * \brief PhotoCaches::overwrite_from_cache
 * Tries to overwrite the file from one of its cached versions.
 * \param prefer_enhanced
 * \return
 */
bool PhotoCaches::overwrite_from_cache(bool prefer_enhanced)
{
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

/*!
 * \brief PhotoCaches::discard_cached_original
 */
void PhotoCaches::discard_cached_original()
{
  remove(original_file_);
}

/*!
 * \brief PhotoCaches::discard_cached_enhanced
 */
void PhotoCaches::discard_cached_enhanced()
{
  remove(enhanced_file_);
}

/*!
 * \brief PhotoCaches::discard_all
 */
void PhotoCaches::discard_all()
{
  discard_cached_original();
  discard_cached_enhanced();
}
