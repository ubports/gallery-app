add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_database_src_SOURCE_DIR}
    ${gallery_album_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

add_executable(mediamonitor tst_mediamonitor.cpp)

qt5_use_modules(mediamonitor Quick Test)

add_test(mediamonitor mediamonitor -xunitxml -o test_mediamonitor.xml)

set_tests_properties(mediamonitor PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )

target_link_libraries(mediamonitor
    gallery-util
    gallery-media
    gallery-core
    gallery-medialoader
    gallery-database
    gallery-album
    )
