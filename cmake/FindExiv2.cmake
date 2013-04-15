find_path(EXIV2_INCLUDE_DIR NAMES exiv2/exif.hpp)
find_library(EXIV2_LIBRARY NAMES exiv2 libexiv2)
set(EXIV2_LIBRARIES "${EXIV2_LIBRARY}")
