project(gallery_database_src)

set(GALLERY_DATABASE_LIB gallery-database)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_database_HDRS
    album-table.h
    database.h
    media-table.h
    photo-edit-table.h
    )

set(gallery_database_SRCS
    album-table.cpp
    database.cpp
    media-table.cpp
    photo-edit-table.cpp
    )

add_library(${GALLERY_DATABASE_LIB}
    ${gallery_database_MOC_SRCS}
    ${gallery_database_SRCS}
    )

qt5_use_modules(${GALLERY_DATABASE_LIB} Widgets Core Qml Quick Sql)

target_link_libraries( ${GALLERY_DATABASE_LIB}
    ${GSTLIB_LDFLAGS}
    )
