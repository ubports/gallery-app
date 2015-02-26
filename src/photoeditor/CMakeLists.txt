project(gallery_photoeditor_src)

set(GALLERY_PHOTOEDITOR_LIB gallery-photoeditor)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${EXIV2_INCLUDEDIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_photoeditor_HDRS
    file-utils.h
    imaging.h
    orientation.h
    photo-caches.h
    photo-data.h
    photo-edit-command.h
    photo-edit-thread.h
    photo-image-provider.h
    photo-metadata.h
    )

set(gallery_photoeditor_SRCS
    file-utils.cpp
    imaging.cpp
    orientation.cpp
    photo-caches.cpp
    photo-data.cpp
    photo-edit-thread.cpp
    photo-image-provider.cpp
    photo-metadata.cpp
    )

add_library(${GALLERY_PHOTOEDITOR_LIB}
    ${gallery_photoeditor_SRCS}
    )

qt5_use_modules(${GALLERY_PHOTOEDITOR_LIB} Core Qml Quick Xml Widgets)

target_link_libraries( ${GALLERY_PHOTOEDITOR_LIB}
    ${EXIV2_LIBRARIES}
    )
