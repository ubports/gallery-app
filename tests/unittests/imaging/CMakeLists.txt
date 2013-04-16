add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_src_SOURCE_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

add_executable(imaging
    tst_imaging.cpp
    ${gallery_src_SOURCE_DIR}/util/imaging.cpp
    )

qt5_use_modules(imaging Quick Widgets Test)
add_test(imaging imaging -xunitxml -o test_resource.xml)
set_tests_properties(imaging PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
target_link_libraries(imaging 
    ${GSTLIB_LDFLAGS}
    )
