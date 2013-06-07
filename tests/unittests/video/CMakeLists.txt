add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

configure_file(
    ${gallery_src_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_album_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_event_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${gallery_qml_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${gallery_video_src_SOURCE_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

add_executable(video
    tst_video.cpp
    ${gallery_src_SOURCE_DIR}/gallery-manager.cpp
    )

qt5_use_modules(video Widgets Core Qml Quick OpenGL Sql Test)
add_test(video video -xunitxml -o test_video.xml)
set_tests_properties(video PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )

target_link_libraries(video
    gallery-core
    gallery-database
    gallery-media
    gallery-util
    gallery-video
    ${EXIV2_LIBRARIES}
    ${GSTLIB_LDFLAGS}
    )
