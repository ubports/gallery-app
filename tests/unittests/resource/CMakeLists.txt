add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_util_src_SOURCE_DIR}
    )

add_executable(resource
    tst_resource.cpp
    )

qt5_use_modules(resource Quick Test)

add_test(resource resource -xunitxml -o test_resource.xml)
set_tests_properties(resource PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )

target_link_libraries(resource
    gallery-util
    )
