project(gallery_medialoader_src)

set(GALLERY_MEDIALOADER_LIB gallery-medialoader)

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    ${MEDIAINFO_INCLUDE_DIR}
    )

set(gallery_medialoader_HDRS
    video-metadata.h
    )

set(gallery_medialoader_SRCS
    video-metadata.cpp
    )

string(REPLACE ";" " " OTHER_CFLAGS "${MEDIAINFO_CFLAGS_OTHER}")
set(CMAKE_CXX_FLAGS ${OTHER_CFLAGS})

add_library(${GALLERY_MEDIALOADER_LIB}
    ${gallery_medialoader_SRCS}
    )

qt5_use_modules(${GALLERY_MEDIALOADER_LIB} Core Quick)

if(CLICK_MODE)
    set(EXIV2_LIBRARIES
        ${CMAKE_EXE_LINK_STATIC_CXX_FLAGS}
        ${EXIV2_STATIC_LIBRARIES}
        -ldl
        ${CMAKE_EXE_LINK_DYNAMIC_CXX_FLAGS})
    install(FILES exiv2.copyright DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(CLICK_MODE)

target_link_libraries( ${GALLERY_MEDIALOADER_LIB}
    ${GSTLIB_LDFLAGS}
    ${MEDIAINFO_LIBRARIES}
    gallery-util
    )
