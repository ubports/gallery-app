add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_photo_src_SOURCE_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

QT_WRAP_CPP(gallerystandardimageprovider GALLERYSTANDARDIMAGEPROVIDIER_MOCS
    ${gallery_src_SOURCE_DIR}/medialoader/preview-manager.h
    )

add_executable(gallerystandardimageprovider
    tst_gallerystandardimageprovidertest.cpp
    preview-manager.cpp
    photometa-data.cpp
    ${gallery_src_SOURCE_DIR}/medialoader/gallery-standard-image-provider.cpp
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
target_link_libraries(gallerystandardimageprovider
    ${GSTLIB_LDFLAGS}
    )
