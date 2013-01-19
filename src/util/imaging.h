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

#ifndef GALLERY_UTIL_IMAGING_H_
#define GALLERY_UTIL_IMAGING_H_

#include <QImage>
#include <QColor>

inline int clampi(int i, int min, int max) {
  return (i < min) ? min : ((i > max) ? max : i);
}

inline float clampf(float x, float min, float max) {
  return (x < min) ? min : ((x > max) ? max : x);
}

class HSVTransformation {
 public:
  HSVTransformation() { }
  virtual ~HSVTransformation() { }

  virtual QColor transform_pixel(const QColor& pixel_color) const;
  virtual bool is_identity() const = 0;

 protected:
  int remap_table_[256];
};

class IntensityHistogram {
 public:
  IntensityHistogram(const QImage& basis_image);
  virtual ~IntensityHistogram() { }

  float get_cumulative_probability(int level);

 private:
  int counts_[256];
  float probabilities_[256];
  float cumulative_probabilities_[256];
};

class ToneExpansionTransformation : public virtual HSVTransformation {
  static const float DEFAULT_LOW_DISCARD_MASS;
  static const float DEFAULT_HIGH_DISCARD_MASS;

 public:
  ToneExpansionTransformation(IntensityHistogram h, float low_discard_mass =
    -1.0f, float high_discard_mass = -1.0f);
  virtual ~ToneExpansionTransformation() { }

  bool is_identity() const;

  float low_discard_mass() const;
  float high_discard_mass() const;

 private:
  void build_remap_table();

  int low_kink_;
  int high_kink_;
  float low_discard_mass_;
  float high_discard_mass_;
};

class HermiteGammaApproximationFunction {
 public:
  HermiteGammaApproximationFunction(float user_interval_upper);
  virtual ~HermiteGammaApproximationFunction() { }

  float evaluate(float x);

 private:
  float x_scale_;
  float nonzero_interval_upper_;
};

class ShadowDetailTransformation : public virtual HSVTransformation {
  static const float MAX_EFFECT_SHIFT;
  static const float MIN_TONAL_WIDTH;
  static const float MAX_TONAL_WIDTH;
  static const float TONAL_WIDTH;

 public:
  ShadowDetailTransformation(float intensity);

  bool is_identity() const;

 private:
  float intensity_;
};

class AutoEnhanceTransformation : public virtual HSVTransformation {
  static const int SHADOW_DETECT_MIN_INTENSITY;
  static const int SHADOW_DETECT_MAX_INTENSITY;
  static const int SHADOW_DETECT_INTENSITY_RANGE;
  static const int EMPIRICAL_DARK;
  static const float SHADOW_AGGRESSIVENESS_MUL;

 public:
  AutoEnhanceTransformation(const QImage& basis_image);
  virtual ~AutoEnhanceTransformation();

  QColor transform_pixel(const QColor& pixel_color) const;
  bool is_identity() const;

 private:
  ShadowDetailTransformation* shadow_transform_;
  ToneExpansionTransformation* tone_expansion_transform_;
};

#endif  // GALLERY_UTIL_IMAGING_H_

