project(gallery_photo_src)

set(GALLERY_PHOTO_LIB gallery-photo)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_photoeditor_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_photo_HDRS
    photo.h
    )

set(gallery_photo_SRCS
    photo.cpp
    )

add_library(${GALLERY_PHOTO_LIB}
    ${gallery_photo_SRCS}
    )

qt5_use_modules(${GALLERY_PHOTO_LIB} Widgets Core Quick)

