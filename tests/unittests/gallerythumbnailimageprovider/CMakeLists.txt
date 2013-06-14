add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_medialoader_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

QT_WRAP_CPP(gallerythumbnailimageprovider GALLERYTHUMBNAILIMAGEPROVIDIER_MOCS
    ${gallery_medialoader_src_SOURCE_DIR}/preview-manager.h
    )

add_executable(gallerythumbnailimageprovider
    tst_gallerythumbnailimageprovider.cpp
    ../stubs/preview-manager_stub.cpp
    ${gallery_src_SOURCE_DIR}/medialoader/gallery-thumbnail-image-provider.cpp
    ${GALLERYTHUMBNAILIMAGEPROVIDIER_MOCS}
    )

qt5_use_modules(gallerythumbnailimageprovider Quick Test)
add_test(gallerythumbnailimageprovider gallerythumbnailimageprovider
    -xunitxml -o test_gallerythumbnailimageprovider.xml
    )
set_tests_properties(gallerythumbnailimageprovider PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
target_link_libraries(gallerythumbnailimageprovider 
    ${GSTLIB_LDFLAGS}
    )
