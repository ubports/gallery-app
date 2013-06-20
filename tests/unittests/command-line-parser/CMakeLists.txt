add_definitions(-DTEST_SUITE)

if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

include_directories(
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

add_executable(command-line-parser
    tst_commandlineparser.cpp
    )

qt5_use_modules(command-line-parser Quick Test)

add_test(command-line-parser command-line-parser -xunitxml -o test_commandlineparser.xml)
set_tests_properties(command-line-parser PROPERTIES
    TIMEOUT ${CTEST_TESTING_TIMEOUT}
    ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )

target_link_libraries(command-line-parser 
    gallery-util
    )
