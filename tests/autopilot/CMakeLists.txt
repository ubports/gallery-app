set(AUTOPILOT_DIR gallery_app)

execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
    OUTPUT_VARIABLE PYTHON_PACKAGE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

install(DIRECTORY ${AUTOPILOT_DIR}
    DESTINATION ${PYTHON_PACKAGE_DIR}
    )

include(${CMAKE_SOURCE_DIR}/cmake/autopilot.cmake)
declare_autopilot_test(GALLERY_APP=${gallery_src_BINARY_DIR}/gallery-app
    ${AUTOPILOT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    )

