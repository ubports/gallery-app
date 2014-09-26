project(gallery_util_src)

set(GALLERY_UTIL_LIB gallery-util)

include_directories(
    ${gallery_src_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_util_HDRS
    collections.h
    command-line-parser.h
    imaging.h
    orientation.h
    resource.h
    variants.h
    urlhandler.h
    )

set(gallery_util_SRCS
    command-line-parser.cpp
    imaging.cpp
    orientation.cpp
    resource.cpp
    urlhandler.cpp
    )

add_library(${GALLERY_UTIL_LIB}
    ${gallery_util_SRCS}
    )

qt5_use_modules(${GALLERY_UTIL_LIB} Widgets Core)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    set(GL_LIBRARIES ${GLESv2_LIBRARIES})
else()
    set(GL_LIBRARIES ${OPENGL_LIBRARIES})
endif()

target_link_libraries( ${GALLERY_UTIL_LIB}
    ${GL_LIBRARIES}
    )
