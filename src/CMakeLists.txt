project(gallery_src)

add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(album)
add_subdirectory(database)
add_subdirectory(event)
add_subdirectory(media)
add_subdirectory(photoeditor)
add_subdirectory(medialoader)
add_subdirectory(photo)
add_subdirectory(qml)
add_subdirectory(video)

configure_file(
    config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
    )

include_directories(
    ${gallery_util_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_event_src_SOURCE_DIR}
    ${gallery_album_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${gallery_photoeditor_src_SOURCE_DIR}
    ${gallery_video_src_SOURCE_DIR}
    ${gallery_qml_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_HDRS
    content-communicator.h
    gallery-application.h
    gallery-manager.h
    media-object-factory.h
    )

set(gallery_SRCS
    content-communicator.cpp
    gallery-application.cpp
    gallery-manager.cpp
    main.cpp
    media-object-factory.cpp
    )

add_executable(${GALLERY}
    ${gallery_SRCS}
    )

qt5_use_modules(${GALLERY} Widgets Core Qml Quick Sql DBus Gui)

target_link_libraries( ${GALLERY}
    gallery-album
    gallery-core
    gallery-database
    gallery-event
    gallery-media
    gallery-medialoader
    gallery-photo
    gallery-photoeditor
    gallery-qml
    gallery-util
    gallery-video
    ${CONTENTHUB_LIBRARIES}
    )

install(TARGETS ${GALLERY} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
