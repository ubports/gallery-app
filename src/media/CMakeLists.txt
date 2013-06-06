project(gallery_media_src)

set(GALLERY_MEDIA_LIB gallery-media)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_event_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${gallery_qml_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_media_HDRS
    media-collection.h
    media-monitor.h
    media-source.h
    preview-manager.h
    )

set(gallery_media_SRCS
    media-collection.cpp
    media-monitor.cpp
    media-source.cpp
    preview-manager.cpp
    )

add_library(${GALLERY_MEDIA_LIB}
    ${gallery_media_SRCS}
    )

qt5_use_modules(${GALLERY_MEDIA_LIB} Core Quick)

target_link_libraries( ${GALLERY_MEDIA_LIB}
    gallery-core
    gallery-database
    gallery-event
    gallery-photo
    gallery-qml
    gallery-util
    ${GSTLIB_LDFLAGS}
    )
