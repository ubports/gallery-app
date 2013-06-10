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

#include "photo-metadata.h"

#include <cstdio>

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
    for (size_t i = 0; i < n_keys; i++) {
        if (in.contains(keys[i]))
            return keys[i];
    }
    
    return NULL;
}

bool is_xmp_key(const char* key) {
    return (key != NULL) ? (std::strncmp("Xmp.", key, 4) == 0) : false;
}

bool is_exif_key(const char* key) {
    return (key != NULL) ? (std::strncmp("Exif.", key, 5) == 0) : false;
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
    for (size_t i = 0; i < NUM_EXIF_DATE_FORMATS; i++) {
        int year, month, day, hour, minute, second;
        if (std::sscanf(s, EXIF_DATE_FORMATS[i], &year, &month, &day, &hour,
                        &minute, &second) == 6) {
            // no need to check year, month, day, hour, minute and second variables
            // for bogus values before using them -- if the values are bogus, the
            // resulting QDateTime will be invalid, which is exactly what we want
            return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
        }
    }
    
    // the no argument QDateTime constructor produces an invalid QDateTime,
    // which is what we want
    return QDateTime();
}
} // namespace

/*!
 * \brief PhotoMetadata::PhotoMetadata
 * \param filepath
 */
PhotoMetadata::PhotoMetadata(const char* filepath)
    : file_source_info_(filepath)
{
    image_ = Exiv2::ImageFactory::open(filepath);
    image_->readMetadata();
}

/*!
 * \brief PhotoMetadata::FromFile
 * \param filepath
 * \return
 */
PhotoMetadata* PhotoMetadata::FromFile(const char* filepath)
{
    PhotoMetadata* result = NULL;
    try {
        result = new PhotoMetadata(filepath);

        if (!result->image_->good()) {
            qDebug("Invalid image metadata in %s", filepath);
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
        qDebug("Error loading image metadata: %s", e.what());
        delete result;
        return NULL;
    }
}

/*!
 * \brief PhotoMetadata::FromFile
 * \param file
 * \return
 */
PhotoMetadata* PhotoMetadata::FromFile(const QFileInfo &file)
{
    return PhotoMetadata::FromFile(file.absoluteFilePath().toStdString().c_str());
}

/*!
 * \brief PhotoMetadata::orientation
 * \return
 */
Orientation PhotoMetadata::orientation() const
{
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

/*!
 * \brief PhotoMetadata::exposure_time
 * \return
 */
QDateTime PhotoMetadata::exposure_time() const
{
    const char* matched = get_first_matched(EXPOSURE_TIME_KEYS,
                                            NUM_EXPOSURE_TIME_KEYS, keys_present_);
    if (matched == NULL)
        return QDateTime();

    if (is_exif_key(matched))
        return parse_exif_date_string(image_->exifData()[matched].toString().c_str());

    if (is_xmp_key(matched))
        return parse_xmp_date_string(image_->xmpData()[matched].toString().c_str());

    // No valid/known tag for exposure date/time
    return QDateTime();
}

/*!
 * \brief PhotoMetadata::orientation_correction
 * \return
 */
OrientationCorrection PhotoMetadata::orientation_correction() const
{
    return OrientationCorrection::fromOrientation(orientation());
}

/*!
 * \brief PhotoMetadata::orientation_transform
 * \return
 */
QTransform PhotoMetadata::orientation_transform() const
{
    return orientation_correction().toTransform();
}

/*!
 * \brief PhotoMetadata::set_orientation
 * \param orientation
 */
void PhotoMetadata::set_orientation(Orientation orientation)
{
    Exiv2::ExifData& exif_data = image_->exifData();

    exif_data[EXIF_ORIENTATION_KEY] = orientation;

    if (!keys_present_.contains(EXIF_ORIENTATION_KEY))
        keys_present_.insert(EXIF_ORIENTATION_KEY);
}

/*!
 * \brief PhotoMetadata::save
 * \return
 */
bool PhotoMetadata::save() const
{
    try {
        image_->writeMetadata();
        return true;
    } catch (Exiv2::AnyError& e) {
        return false;
    }
}
