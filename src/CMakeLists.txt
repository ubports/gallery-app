project(gallery_src)

add_subdirectory(album)
add_subdirectory(core)
add_subdirectory(database)
add_subdirectory(event)
add_subdirectory(media)
add_subdirectory(photo)
add_subdirectory(qml)
add_subdirectory(util)

configure_file(
    config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
    )

set(GALLERY gallery-app)

include_directories(
    ${gallery_album_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_event_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${gallery_qml_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_HDRS
    gallery-application.h
    gallery-manager.h
    )

set(gallery_SRCS
    gallery-application.cpp
    gallery-manager.cpp
    main.cpp
    )

add_executable(${GALLERY}
    ${gallery_SRCS}
    )

qt5_use_modules(${GALLERY} Widgets Core Qml Quick Sql)

target_link_libraries( ${GALLERY}
    gallery-album
    gallery-core
    gallery-database
    gallery-event
    gallery-media
    gallery-photo
    gallery-qml
    gallery-util
    ${GSTLIB_LDFLAGS}
    )

install(TARGETS ${GALLERY} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
