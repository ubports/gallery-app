project(gallery_database_src)

set(GALLERY_DATABASE_LIB gallery-database)

include_directories(
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/album
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_src_SOURCE_DIR}/photo
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_database_HDRS
    album-table.h
    database.h
    media-table.h
    )

set(gallery_database_SRCS
    album-table.cpp
    database.cpp
    media-table.cpp
    )

add_library(${GALLERY_DATABASE_LIB}
    ${gallery_database_SRCS}
    )

qt5_use_modules(${GALLERY_DATABASE_LIB} Widgets Core Qml Quick Sql)

