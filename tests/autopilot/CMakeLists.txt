execute_process(COMMAND python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
    OUTPUT_VARIABLE PYTHON_PACKAGE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

if(INSTALL_TESTS)
install(DIRECTORY ${AUTOPILOT_DIR}
    DESTINATION ${PYTHON_PACKAGE_DIR}
    )
endif(INSTALL_TESTS)

include(${CMAKE_SOURCE_DIR}/cmake/autopilot.cmake)
declare_autopilot_test(GALLERY_APP=${gallery_src_BINARY_DIR}/gallery-app
    ${AUTOPILOT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    )

