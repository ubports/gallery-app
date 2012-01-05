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
 * Lucas Beeler <lucas@yorba.org>
 */

#include <cstdio>

#include <QDate>

#include "photo-metadata.h"

namespace {
  const Orientation DEFAULT_ORIENTATION = TOP_LEFT_ORIGIN;
  const char* EXIF_ORIENTATION_KEY = "Exif.Image.Orientation";
  const char* EXPOSURE_TIME_KEYS[] = {
    "Exif.Photo.DateTimeOriginal",
    "Xmp.exif.DateTimeOriginal",
    "Xmp.xmp.CreateDate",
    "Exif.Photo.DateTimeDigitized",
    "Xmp.exif.DateTimeDigitized",
    "Exif.Image.DateTime"
  };
  const size_t NUM_EXPOSURE_TIME_KEYS = 6;
  const char* EXIF_DATE_FORMATS[] = {
    "%d:%d:%d %d:%d:%d",

    // for Minolta DiMAGE E223 (colon, instead of space, separates day from
    // hour in exif)
    "%d:%d:%d:%d:%d:%d",

    // for Samsung NV10 (which uses a period instead of colons for the date
    // and two spaces between date and time)
    "%d.%d.%d  %d:%d:%d"
  };
  const size_t NUM_EXIF_DATE_FORMATS = 3;
  
  const char* get_first_matched(const char* keys[], size_t n_keys,
    const QSet<QString>& in) {
    for (size_t i = 0; i < n_keys; i++)
      if (in.find(keys[i]) != in.end())
        return keys[i];
    
    return NULL;
  }
  
  bool is_xmp_key(const char* key) {
    return (std::strncmp("Xmp.", key, 4) == 0);
  }
  
  bool is_exif_key(const char* key) {
    return (std::strncmp("Exif.", key, 5) == 0);
  }
  
  // caller should test if 's' could be successfully parsed by invoking the
  // isValid() method on the returned QDateTime instance; if isValid() == false,
  // 's' couldn't be parsed
  QDateTime parse_xmp_date_string(const char* s) {
    return QDateTime::fromString(s, Qt::ISODate);
  }
  
  // caller should test if 's' could be successfully parsed by invoking the
  // isValid() method on the returned QDateTime instance; if isValid() == false,
  // 's' couldn't be parsed
  QDateTime parse_exif_date_string(const char* s) {
    int year, month, day, hour, minute, second;
    bool found = false;
    for (size_t i = 0; i < NUM_EXIF_DATE_FORMATS; i++) {
      if (std::scanf(s, EXIF_DATE_FORMATS[i], &year, &month, &day, &hour,
        &minute, &second) == 6) {
        found = true;
        break;
      }
    }
    
    // the no argument QDateTime constructor produces an invalid QDateTime,
    // which is what we want
    if (!found)
      return QDateTime();

    // no need to check year, month, day, hour, minute and second variables
    // for bogus values before using them -- if the values are bogus, the
    // resulting QDateTime will be invalid, which is exactly what we want
    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
  }
} // namespace

PhotoMetadata::PhotoMetadata(const char* filepath)
  : file_source_info_(filepath) {
  image_ = Exiv2::ImageFactory::open(filepath);
  image_->readMetadata();
}

PhotoMetadata* PhotoMetadata::FromFile(const char* filepath) {
  PhotoMetadata* result = NULL;
  try {
    result = new PhotoMetadata(filepath);

    if (!result->image_->good()) {
      delete result;
      return NULL;
    }
    
    Exiv2::ExifData& exif_data = result->image_->exifData();
    Exiv2::ExifData::const_iterator end = exif_data.end();
    for (Exiv2::ExifData::const_iterator i = exif_data.begin(); i != end; i++)
      result->keys_present_.insert(QString(i->key().c_str()));
    
    Exiv2::XmpData& xmp_data = result->image_->xmpData();
    Exiv2::XmpData::const_iterator end1 = xmp_data.end();
    for (Exiv2::XmpData::const_iterator i = xmp_data.begin(); i != end1; i++)
      result->keys_present_.insert(QString(i->key().c_str()));
    
    return result;
  } catch (Exiv2::AnyError& e) {
    delete result;
    return NULL;
  }
}

PhotoMetadata* PhotoMetadata::FromFile(const QFileInfo &file) {
  return PhotoMetadata::FromFile(file.absoluteFilePath().toStdString().c_str());
}

Orientation PhotoMetadata::orientation() const { 
  Exiv2::ExifData& exif_data = image_->exifData();
  
  if (exif_data.empty())
    return DEFAULT_ORIENTATION;
  
  if (keys_present_.find(EXIF_ORIENTATION_KEY) == keys_present_.end())
    return DEFAULT_ORIENTATION;
  
  long orientation_code = exif_data[EXIF_ORIENTATION_KEY].toLong();
  if (orientation_code < MIN_ORIENTATION || orientation_code > MAX_ORIENTATION)
    return DEFAULT_ORIENTATION;
  
  return static_cast<Orientation>(orientation_code);
}

QDateTime PhotoMetadata::exposure_time() const { 
  const char* matched = get_first_matched(EXPOSURE_TIME_KEYS,
    NUM_EXPOSURE_TIME_KEYS, keys_present_);
  
  QDateTime parsed;
  if (is_exif_key(matched)) {
    parsed = parse_xmp_date_string(
      image_->exifData()[matched].toString().c_str());
  } else if (is_xmp_key(matched)) {
    parsed = parse_exif_date_string(
      image_->exifData()[matched].toString().c_str());
  }
  
  return parsed;
}

OrientationCorrection PhotoMetadata::orientation_correction() const {
  return OrientationCorrection::FromOrientation(orientation());
}

QTransform PhotoMetadata::orientation_transform() const {
  OrientationCorrection correction = orientation_correction();

  QTransform result;
  result.scale(correction.horizontal_scale_factor_, 1.0);
  result.rotate(correction.rotation_angle_);

  return result;
}

OrientationCorrection OrientationCorrection::FromOrientation(Orientation o) {
  double rotation_angle = 0.0;
  double horizontal_scale_factor = 1.0;

  switch (o) {
    case TOP_RIGHT_ORIGIN:
      horizontal_scale_factor = -1.0;
    break;

    case BOTTOM_RIGHT_ORIGIN:
      rotation_angle = 180.0;
    break;

    case BOTTOM_LEFT_ORIGIN:
      horizontal_scale_factor = -1.0;
      rotation_angle = 180.0;
    break;

    case LEFT_TOP_ORIGIN:
      horizontal_scale_factor = -1.0;
      rotation_angle = -90.0;
    break;

    case RIGHT_TOP_ORIGIN:
      rotation_angle = 90.0;
    break;

    case RIGHT_BOTTOM_ORIGIN:
      horizontal_scale_factor = -1.0;
      rotation_angle = 90.0;
    break;

    case LEFT_BOTTOM_ORIGIN:
      rotation_angle = -90.0;
    break;

    default:
      ; // do nothing
    break;
  }

  return OrientationCorrection(rotation_angle, horizontal_scale_factor);
}

OrientationCorrection OrientationCorrection::Identity() {
  return OrientationCorrection(0.0, 1.0);
}