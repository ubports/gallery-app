project(gallery_medialoader_src)

set(GALLERY_MEDIALOADER_LIB gallery-medialoader)

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${EXIV2_INCLUDEDIR}
    ${GSTLIB_INCLUDE_DIRS}
    ${MEDIAINFO_INCLUDE_DIR}
    )

set(gallery_medialoader_HDRS
    gallery-standard-image-provider.h
    gallery-thumbnail-image-provider.h
    photo-metadata.h
    preview-manager.h
    video-metadata.h
    )

set(gallery_medialoader_SRCS
    gallery-standard-image-provider.cpp
    gallery-thumbnail-image-provider.cpp
    photo-metadata.cpp
    preview-manager.cpp
    video-metadata.cpp
    )

set(CMAKE_CXX_FLAGS ${MEDIAINFO_CFLAGS_OTHER})

add_library(${GALLERY_MEDIALOADER_LIB}
    ${gallery_medialoader_SRCS}
    )

qt5_use_modules(${GALLERY_MEDIALOADER_LIB} Core Quick)

target_link_libraries( ${GALLERY_MEDIALOADER_LIB}
    ${EXIV2_LIBRARIES}
    ${GSTLIB_LDFLAGS}
    ${MEDIAINFO_LIBRARIES}
    )
