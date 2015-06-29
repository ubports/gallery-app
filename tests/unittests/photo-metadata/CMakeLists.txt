add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_album_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_event_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${gallery_photoeditor_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${gallery_video_src_SOURCE_DIR}
    ${EXIV2_INCLUDEDIR}
    ${CMAKE_BINARY_DIR}
    )

QT5_WRAP_CPP(PHOTOMETADATA_MOCS
    ${gallery_src_SOURCE_DIR}/gallery-manager.h
    ${gallery_database_src_SOURCE_DIR}/album-table.h
    ${gallery_database_src_SOURCE_DIR}/database.h
    ${gallery_database_src_SOURCE_DIR}/media-table.h
    ${gallery_medialoader_src_SOURCE_DIR}/video-metadata.h
    ${gallery_photo_src_SOURCE_DIR}/photo.h
    )

add_definitions(-DSAMPLE_IMAGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/images")
add_executable(photo-metadata
    tst_photo-metadata.cpp
    ${gallery_photoeditor_src_SOURCE_DIR}/photo-metadata.cpp
    ../stubs/album-table_stub.cpp
    ../stubs/database_stub.cpp
    ../stubs/media-table_stub.cpp
    ../stubs/photo_stub.cpp
    ../stubs/video_stub.cpp
    ../stubs/video-metadata_stub.cpp
    ../stubs/gallery-manager_stub.cpp
    ${PHOTOMETADATA_MOCS}
    )

qt5_use_modules(photo-metadata Widgets Core Quick Qml Test)
add_test(photo-metadata photo-metadata -xunitxml -o test_video.xml)
set_tests_properties(photo-metadata PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal;TZ=Pacific/Auckland"
    )

target_link_libraries(photo-metadata
    gallery-album
    gallery-core
    gallery-media
    gallery-util
    gallery-video
    ${EXIV2_LIBRARIES}
    )
