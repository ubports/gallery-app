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

#include <QApplication>

#include "util/imaging.h"

/*!
 * \brief HSVTransformation::transform_pixel
 * \param pixel_color
 * \return
 */
QColor HSVTransformation::transform_pixel(const QColor &pixel_color) const
{
  QColor result;

  int h, s, v;
  pixel_color.getHsv(&h, &s, &v);

  v = remap_table_[v];

  result.setHsv(h, s, v);

  return result;
}

/*!
 * \brief IntensityHistogram::IntensityHistogram
 * \param basis_image
 */
IntensityHistogram::IntensityHistogram(const QImage& basis_image)
{
  for (int i = 0; i < 256; i++)
    counts_[i] = 0;

  int width = basis_image.width();
  int height = basis_image.height();

  for (int j = 0; j < height; j++) {
    QApplication::processEvents();
    
    for (int i = 0; i < width; i++) {
      QColor c = QColor(basis_image.pixel(i, j));
      int intensity = c.value();
      counts_[intensity]++;
    }
  }

  float pixel_count = (float)(width * height);
  float accumulator = 0.0f;
  for (int i = 0; i < 256; i++) {
    probabilities_[i] = ((float) counts_[i]) / pixel_count;
    accumulator += probabilities_[i];
    cumulative_probabilities_[i] = accumulator;
  }
}

/*!
 * \brief IntensityHistogram::get_cumulative_probability
 * \param level
 * \return
 */
float IntensityHistogram::get_cumulative_probability(int level)
{
  return cumulative_probabilities_[level];
}


const float ToneExpansionTransformation::DEFAULT_LOW_DISCARD_MASS = 0.02f;
const float ToneExpansionTransformation::DEFAULT_HIGH_DISCARD_MASS = 0.98f;
/*!
 * \brief ToneExpansionTransformation::ToneExpansionTransformation
 * \param h
 * \param low_discard_mass
 * \param high_discard_mass
 */
ToneExpansionTransformation::ToneExpansionTransformation(IntensityHistogram h,
  float low_discard_mass, float high_discard_mass) {
  if (low_discard_mass == -1.0f)
    low_discard_mass = DEFAULT_LOW_DISCARD_MASS;
  if (high_discard_mass == -1.0f)
    high_discard_mass = DEFAULT_HIGH_DISCARD_MASS;

  low_discard_mass_ = low_discard_mass;
  high_discard_mass_ = high_discard_mass;

  low_kink_ = 0;
  high_kink_ = 255;

  while (h.get_cumulative_probability(low_kink_) < low_discard_mass)
    low_kink_++;

  while (h.get_cumulative_probability(high_kink_) > high_discard_mass)
    high_kink_--;

  low_kink_ = clampi(low_kink_, 0, 255);
  high_kink_ = clampi(high_kink_, 0, 255);

  build_remap_table();
}

/*!
 * \brief ToneExpansionTransformation::is_identity
 * \return
 */
bool ToneExpansionTransformation::is_identity() const
{
  return ((low_kink_ == 0) && (high_kink_ == 255));
}

/*!
 * \brief ToneExpansionTransformation::build_remap_table
 */
void ToneExpansionTransformation::build_remap_table()
{
  float low_kink_f = ((float) low_kink_) / 255.0f;
  float high_kink_f = ((float) high_kink_) / 255.0f;

  float slope = 1.0f / (high_kink_f - low_kink_f);
  float intercept = -(low_kink_f / (high_kink_f - low_kink_f));

  int i = 0;
  for ( ; i <= low_kink_; i++)
      remap_table_[i] = 0;

  for ( ; i < high_kink_; i++)
      remap_table_[i] = (int) ((255.0f * (slope * (((float) i) / 255.0f) +
        intercept)) + 0.5);

  for ( ; i < 256; i++)
      remap_table_[i] = 255;
}

/*!
 * \brief ToneExpansionTransformation::low_discard_mass
 * \return
 */
float ToneExpansionTransformation::low_discard_mass() const
{
  return low_discard_mass_;
}

/*!
 * \brief ToneExpansionTransformation::high_discard_mass
 * \return
 */
float ToneExpansionTransformation::high_discard_mass() const
{
  return high_discard_mass_;
}


/*!
 * \brief HermiteGammaApproximationFunction::HermiteGammaApproximationFunction
 * \param user_interval_upper
 */
HermiteGammaApproximationFunction::HermiteGammaApproximationFunction(
  float user_interval_upper)
{
  nonzero_interval_upper_ = clampf(user_interval_upper, 0.1f, 1.0f);
  x_scale_ = 1.0f / nonzero_interval_upper_;
}

/*!
 * \brief HermiteGammaApproximationFunction::evaluate
 * \param x
 * \return
 */
float HermiteGammaApproximationFunction::evaluate(float x)
{
  if (x < 0.0f)
    return 0.0f;
  else if (x > nonzero_interval_upper_)
    return 0.0f;
  else {
    float indep_var = x_scale_ * x;

    float dep_var =  6.0f * ((indep_var * indep_var * indep_var) -
      (2.0f * (indep_var * indep_var)) + (indep_var));

    return clampf(dep_var, 0.0f, 1.0f);
  }
}


const float ShadowDetailTransformation::MAX_EFFECT_SHIFT = 0.5f;
const float ShadowDetailTransformation::MIN_TONAL_WIDTH = 0.1f;
const float ShadowDetailTransformation::MAX_TONAL_WIDTH = 1.0f;
const float ShadowDetailTransformation::TONAL_WIDTH = 1.0f;
/*!
 * \brief ShadowDetailTransformation::ShadowDetailTransformation
 * \param intensity
 */
ShadowDetailTransformation::ShadowDetailTransformation(float intensity)
{
  intensity_ = intensity;
  float effect_shift = MAX_EFFECT_SHIFT * intensity;

  HermiteGammaApproximationFunction func =
    HermiteGammaApproximationFunction(TONAL_WIDTH);

  for (int i = 0; i < 256; i++) {
    float x = ((float) i) / 255.0f;
    float weight = func.evaluate(x);

    int remapped = (int) ((255.0f * (weight * (x + effect_shift)) + ((1.0f -
      weight) * x)) + 0.5f);
    remap_table_[i] = clampi(remapped, i, 255);
  }
}

/*!
 * \brief ShadowDetailTransformation::is_identity
 * \return
 */
bool ShadowDetailTransformation::is_identity() const
{
  return (intensity_ == 0.0f);
}


const int AutoEnhanceTransformation::SHADOW_DETECT_MIN_INTENSITY = 2;
const int AutoEnhanceTransformation::SHADOW_DETECT_MAX_INTENSITY = 90;
const int AutoEnhanceTransformation::SHADOW_DETECT_INTENSITY_RANGE =
  AutoEnhanceTransformation::SHADOW_DETECT_MAX_INTENSITY -
  AutoEnhanceTransformation::SHADOW_DETECT_MIN_INTENSITY;
const int AutoEnhanceTransformation::EMPIRICAL_DARK = 40;
const float AutoEnhanceTransformation::SHADOW_AGGRESSIVENESS_MUL = 0.45f;
/*!
 * \brief AutoEnhanceTransformation::AutoEnhanceTransformation
 * \param basis
 */
AutoEnhanceTransformation::AutoEnhanceTransformation(const QImage& basis)
  : shadow_transform_(0), tone_expansion_transform_(0)
{
  IntensityHistogram histogram = IntensityHistogram(basis);

  /* compute the percentage of pixels in the image that fall into the
     shadow range -- this measures "of the pixels in the image, how many of
     them are in shadow?" */
  float pct_in_range = 100.0f *
    (histogram.get_cumulative_probability(SHADOW_DETECT_MAX_INTENSITY) -
    histogram.get_cumulative_probability(SHADOW_DETECT_MIN_INTENSITY));

  /* compute the mean intensity of the pixels that are in the shadow range --
     this measures "of those pixels that are in shadow, just how dark are
     they?" */
  float sh_prob_mu =
    (histogram.get_cumulative_probability(SHADOW_DETECT_MIN_INTENSITY) +
    histogram.get_cumulative_probability(SHADOW_DETECT_MAX_INTENSITY)) * 0.5f;
  int sh_intensity_mu = SHADOW_DETECT_MIN_INTENSITY;
  for ( ; sh_intensity_mu <= SHADOW_DETECT_MAX_INTENSITY; sh_intensity_mu++) {
    if (histogram.get_cumulative_probability(sh_intensity_mu) >= sh_prob_mu)
      break;
  }

  /* if more than 30 percent of the pixels in the image are in the shadow
     detection range, or if the mean intensity within the shadow range is less
     than an empirically determined threshold below which pixels appear very
     dark, regardless of the percent of pixels in it, then perform shadow
     detail enhancement. Otherwise, skip shadow detail enhancement and perform
     contrast expansion only */
  if ((pct_in_range > 30.0f) || ((pct_in_range > 10.0f) &&
    (sh_intensity_mu < EMPIRICAL_DARK))) {
    float shadow_trans_effect_size = ((((float) SHADOW_DETECT_MAX_INTENSITY) -
      ((float) sh_intensity_mu)) /
      ((float) SHADOW_DETECT_INTENSITY_RANGE));
    shadow_trans_effect_size *= SHADOW_AGGRESSIVENESS_MUL;

    shadow_transform_
      = new ShadowDetailTransformation(shadow_trans_effect_size);

    QImage shadow_corrected_image = QImage(basis);
    // Can't write into indexed images, due to a limitation in Qt.
    if (shadow_corrected_image.format() == QImage::Format_Indexed8)
      shadow_corrected_image = shadow_corrected_image.convertToFormat(
          QImage::Format_RGB32);

    for (int j = 0; j < shadow_corrected_image.height(); j++) {
      QApplication::processEvents();
      
      for (int i = 0; i < shadow_corrected_image.width(); i++) {
        QColor px = shadow_transform_->transform_pixel(
          QColor(shadow_corrected_image.pixel(i, j)));
        shadow_corrected_image.setPixel(i, j, px.rgb());
      }
    }

    tone_expansion_transform_ = new ToneExpansionTransformation(
      IntensityHistogram(shadow_corrected_image), 0.005f, 0.995f);

  } else {
    tone_expansion_transform_ = new ToneExpansionTransformation(
      IntensityHistogram(basis));
  }
}

/*!
 * \brief AutoEnhanceTransformation::~AutoEnhanceTransformation
 */
AutoEnhanceTransformation::~AutoEnhanceTransformation()
{
  if (shadow_transform_)
    delete shadow_transform_;
  delete tone_expansion_transform_;
}

/*!
 * \brief AutoEnhanceTransformation::transform_pixel
 * \param pixel_color
 * \return
 */
QColor AutoEnhanceTransformation::transform_pixel(
  const QColor& pixel_color) const
{
  QColor px = pixel_color;
  
  if (shadow_transform_)
    px = shadow_transform_->transform_pixel(px);

  px = tone_expansion_transform_->transform_pixel(px);

  /* if tone expansion occurs, boost saturation to compensate for boosted
     dynamic range */
  if (!tone_expansion_transform_->is_identity()) {
    int h, s, v;
    px.getHsv(&h, &s, &v);

    float compensation_multiplier =
        (tone_expansion_transform_->low_discard_mass() < 0.01f) ? 1.02f : 1.10f;

    s = (int) (((float) s) * compensation_multiplier);
    s = clampi(s, 0, 255);

    px.setHsv(h, s, v);
  }

  return px;
}

bool AutoEnhanceTransformation::is_identity() const {
  return false;
}

