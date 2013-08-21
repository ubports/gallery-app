project(gallery_qml_src)

set(GALLERY_QML_LIB gallery-qml)

include_directories(
    ${gallery_src_SOURCE_DIR}
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/album
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/event
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
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

