project(gallery_media_src)

set(GALLERY_MEDIA_LIB gallery-media)

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/medialoader
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_media_HDRS
    media-collection.h
    media-monitor.h
    media-source.h
    )

set(gallery_media_SRCS
    media-collection.cpp
    media-monitor.cpp
    media-source.cpp
    )

add_library(${GALLERY_MEDIA_LIB}
    ${gallery_media_SRCS}
    )

qt5_use_modules(${GALLERY_MEDIA_LIB} Core Quick)

