add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

add_executable(imaging
    tst_imaging.cpp
    )

qt5_use_modules(imaging Quick Widgets Test)

add_test(imaging imaging -xunitxml -o test_imaging.xml)
set_tests_properties(imaging PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )

target_link_libraries(imaging 
    gallery-util
    )
