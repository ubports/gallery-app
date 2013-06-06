project(gallery_qml_src)

set(GALLERY_QML_LIB gallery-qml)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_album_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_event_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_qml_HDRS
    qml-album-collection-model.h
    qml-event-collection-model.h
    qml-event-overview-model.h
    qml-media-collection-model.h
    qml-view-collection-model.h
    )

set(gallery_qml_SRCS
    qml-album-collection-model.cpp
    qml-event-collection-model.cpp
    qml-event-overview-model.cpp
    qml-media-collection-model.cpp
    qml-view-collection-model.cpp
    )

add_library(${GALLERY_QML_LIB}
    ${gallery_qml_SRCS}
    )

qt5_use_modules(${GALLERY_QML_LIB} Core Quick)

target_link_libraries( ${GALLERY_QML_LIB}
    gallery-album
    gallery-core
    gallery-event
    gallery-media
    gallery-photo
    gallery-qml
    gallery-util
    ${GSTLIB_LDFLAGS}
    )
