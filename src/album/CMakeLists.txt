project(gallery_album_src)

set(GALLERY_ALBUM_LIB gallery-album)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_src_SOURCE_DIR}/photo
    ${gallery_src_SOURCE_DIR}/qml
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_album_HDRS
    album.h
    album-collection.h
    album-default-template.h
    album-page.h
    album-template.h
    album-template-page.h
    )

set(gallery_album_SRCS
    album.cpp
    album-default-template.cpp
    album-collection.cpp
    album-page.cpp
    album-template.cpp
    album-template-page.cpp
    )

add_library(${GALLERY_ALBUM_LIB}
    ${gallery_album_SRCS}
    )

qt5_use_modules(${GALLERY_ALBUM_LIB} Widgets Core Qml)

target_link_libraries( ${GALLERY_ALBUM_LIB}
    gallery-core
    gallery-database
    gallery-media
    gallery-photo
    gallery-qml
    gallery-util
    ${GSTLIB_LDFLAGS}
    )
