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
    ${gallery_src_SOURCE_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

add_executable(resource
    tst_resource.cpp
    ${gallery_src_SOURCE_DIR}/util/resource.cpp
    )

qt5_use_modules(resource Quick Test)
add_test(resource resource -xunitxml -o test_resource.xml)
set_tests_properties(resource PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
target_link_libraries(resource 
    ${GLESv2_LIBRARIES}
    ${GSTLIB_LDFLAGS}
    )
