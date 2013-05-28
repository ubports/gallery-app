project(gallery_qml_src)

set(GALLERY_QML_LIB gallery-qml)

include_directories(
    ../
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_qml_HDRS
    gallery-standard-image-provider.h
    gallery-thumbnail-image-provider.h
    qml-album-collection-model.h
    qml-event-collection-model.h
    qml-event-overview-model.h
    qml-media-collection-model.h
    qml-view-collection-model.h
    )

set(gallery_qml_SRCS
    gallery-standard-image-provider.cpp
    gallery-thumbnail-image-provider.cpp
    qml-album-collection-model.cpp
    qml-event-collection-model.cpp
    qml-event-overview-model.cpp
    qml-media-collection-model.cpp
    qml-view-collection-model.cpp
    )

add_library(${GALLERY_QML_LIB}
    ${gallery_qml_MOC_SRCS}
    ${gallery_qml_SRCS}
    )

qt5_use_modules(${GALLERY_QML_LIB} Core Quick)

target_link_libraries( ${GALLERY_QML_LIB}
    ${GSTLIB_LDFLAGS}
    )
