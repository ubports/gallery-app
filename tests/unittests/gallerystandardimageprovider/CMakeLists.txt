add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_src_SOURCE_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

add_executable(gallerystandardimageprovider
    tst_gallerystandardimageprovidertest.cpp
    gallery-manager.cpp
    preview-manager.cpp
    photometa-data.cpp
    ${gallery_src_SOURCE_DIR}/qml/gallery-standard-image-provider.cpp
    )

qt5_use_modules(gallerystandardimageprovider Quick Test)
add_test(gallerystandardimageprovider gallerystandardimageprovider
    -xunitxml -o test_resource.xml
    )
set_tests_properties(gallerystandardimageprovider PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
target_link_libraries(gallerystandardimageprovider 
    ${GSTLIB_LDFLAGS}
    )
