add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${gallery_src_SOURCE_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

add_executable(command-line-parser
    tst_commandlineparser.cpp
    ${gallery_src_SOURCE_DIR}/core/command-line-parser.cpp
    )

qt5_use_modules(command-line-parser Quick Test)
add_test(command-line-parser command-line-parser -xunitxml -o test_resource.xml)
set_tests_properties(command-line-parser PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
target_link_libraries(command-line-parser 
    ${GSTLIB_LDFLAGS}
    )
