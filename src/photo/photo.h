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
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

#ifndef GALLERY_PHOTO_H_
#define GALLERY_PHOTO_H_

#include <QObject>
#include <QDateTime>
#include <QFileInfo>
#include <QStack>

#include "media/media-source.h"
#include "photo/photo-metadata.h"
#include "photo/photo-edit-state.h"
#include "photo/photo-caches.h"

class Photo : public MediaSource {
  Q_OBJECT

  // A simple class for dealing with an undo-/redo-able stack of applied edits.
  class EditStack {
   public:
    EditStack() : base_(), undoable_(), redoable_() {
    }

    void set_base(const PhotoEditState& base) {
      base_ = base;
    }

    void push_edit(const PhotoEditState& edit_state) {
      clear_redos();
      undoable_.push(edit_state);
    }

    void clear_redos() {
      redoable_.clear();
    }

    const PhotoEditState& undo() {
      if (!undoable_.isEmpty())
        redoable_.push(undoable_.pop());
      return current();
    }

    const PhotoEditState& redo() {
      if (!redoable_.isEmpty())
        undoable_.push(redoable_.pop());
      return current();
    }

    const PhotoEditState& current() const {
      return (undoable_.isEmpty() ? base_ : undoable_.top());
    }

   private:
    PhotoEditState base_; // What to return when we have no undo-able edits.
    QStack<PhotoEditState> undoable_;
    QStack<PhotoEditState> redoable_;
  };

 public:
  static bool IsValid(const QFileInfo& file);
  
  explicit Photo(const QFileInfo& file);
  virtual ~Photo();
  
  virtual QImage Image(bool respect_orientation) const;
  virtual int width();
  virtual int height();
  virtual Orientation orientation() const;
  virtual QDateTime exposure_date_time() const;

  virtual QUrl gallery_path() const;
  virtual QUrl gallery_preview_path() const;

  // The "base state" is the PhotoEditState of the file when Gallery starts.  It's
  // the bottom of the undo stack.  The orientation comes from the file itself,
  // but the rest of the PhotoEditState params come from the DB.
  Orientation get_base_orientation() const;
  void set_base_edit_state(const PhotoEditState& base);

  Q_INVOKABLE void saveState();
  Q_INVOKABLE void revertToSavedState();
  Q_INVOKABLE void revertToOriginal();
  Q_INVOKABLE void undo();
  Q_INVOKABLE void redo();
  Q_INVOKABLE void rotateRight();
  Q_INVOKABLE void autoEnhance();
  // Edits the image to original size so you can recrop it.  Returns crop
  // coords in [0,1].  Should be followed by either cancelCropping() or crop().
  Q_INVOKABLE QVariant prepareForCropping();
  Q_INVOKABLE void cancelCropping();
  // You should call prepareForCropping() before calling this.  Specify all
  // coords in [0,1].
  Q_INVOKABLE void crop(QVariant vrect);

 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
 private:
  const PhotoEditState& current_state() const;
  // Returns the original image dimensions translated to the desired orientation.
  void get_original_dimensions(int* width, int* height, Orientation orientation);
  void make_undoable_edit(const PhotoEditState& state);
  void save(const PhotoEditState& state, Orientation old_orientation);
  void edit_file(const PhotoEditState& state);
  void create_cached_enhanced();
  void append_edit_revision(QUrl* url) const;

  mutable QDateTime* exposure_date_time_;
  int edit_revision_; // How many times the pixel data has been modified by us.
  EditStack edits_;
  PhotoEditState saved_state_; // A saved state separate from the undo stack.
  PhotoCaches caches_;

  // We cache this data to avoid an image read at various times.
  PhotoMetadata* original_metadata_;
  int original_width_;
  int original_height_;
  int width_;
  int height_;
};

#endif  // GALLERY_PHOTO_H_
