project(gallery_media_src)

set(GALLERY_MEDIA_LIB gallery-media)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/event
    ${gallery_src_SOURCE_DIR}/medialoader
    ${gallery_src_SOURCE_DIR}/photo
    ${gallery_src_SOURCE_DIR}/qml
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
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

target_link_libraries( ${GALLERY_MEDIA_LIB}
    gallery-core
    gallery-database
    gallery-event
    gallery-medialoader
    gallery-photo
    gallery-qml
    gallery-util
    ${GSTLIB_LDFLAGS}
    )
