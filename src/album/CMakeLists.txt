project(gallery_album_src)

set(GALLERY_ALBUM_LIB gallery-album)

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
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
    )
