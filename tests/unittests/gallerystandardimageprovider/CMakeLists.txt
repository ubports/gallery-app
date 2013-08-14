add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    )

QT_WRAP_CPP(gallerystandardimageprovider GALLERYSTANDARDIMAGEPROVIDIER_MOCS
    ${gallery_medialoader_src_SOURCE_DIR}/preview-manager.h
    ${gallery_medialoader_src_SOURCE_DIR}/photo-metadata.h
    )

add_executable(gallerystandardimageprovider
    tst_gallerystandardimageprovidertest.cpp
    ${gallery_src_SOURCE_DIR}/medialoader/gallery-standard-image-provider.cpp
    ../stubs/orientation_stub.cpp
    ../stubs/preview-manager_stub.cpp
    ../stubs/photometa-data_stub.cpp
    ${GALLERYSTANDARDIMAGEPROVIDIER_MOCS}
    )

qt5_use_modules(gallerystandardimageprovider Quick Test)
add_test(gallerystandardimageprovider gallerystandardimageprovider
    -xunitxml -o test_gallerystandardimageprovider.xml
    )
set_tests_properties(gallerystandardimageprovider PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
target_link_libraries( gallerystandardimageprovider )
