project(gallery_photo_src)

set(GALLERY_PHOTO_LIB gallery-photo)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/medialoader
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${EXIV2_INCLUDEDIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_photo_HDRS
    photo.h
    photo-caches.h
    photo-edit-state.h
    )

set(gallery_photo_SRCS
    photo.cpp
    photo-caches.cpp
    photo-edit-state.cpp
    )

add_library(${GALLERY_PHOTO_LIB}
    ${gallery_photo_SRCS}
    )

qt5_use_modules(${GALLERY_PHOTO_LIB} Widgets Core Quick)

target_link_libraries( ${GALLERY_PHOTO_LIB}
    gallery-core
    gallery-media
    gallery-medialoader
    gallery-database
    gallery-util
    ${EXIV2_LIBRARIES}
    ${GSTLIB_LDFLAGS}
    )
