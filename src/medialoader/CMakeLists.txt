project(gallery_medialoader_src)

set(GALLERY_MEDIALOADER_LIB gallery-medialoader)

include_directories(
    ${gallery_src_SOURCE_DIR}
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_src_SOURCE_DIR}/database
    ${gallery_src_SOURCE_DIR}/event
    ${gallery_src_SOURCE_DIR}/photo
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_medialoader_HDRS
    gallery-standard-image-provider.h
    gallery-thumbnail-image-provider.h
    preview-manager.h
    )

set(gallery_medialoader_SRCS
    gallery-standard-image-provider.cpp
    gallery-thumbnail-image-provider.cpp
    preview-manager.cpp
    )

add_library(${GALLERY_MEDIALOADER_LIB}
    ${gallery_medialoader_SRCS}
    )

qt5_use_modules(${GALLERY_MEDIALOADER_LIB} Core Quick)

target_link_libraries( ${GALLERY_MEDIALOADER_LIB}
    gallery-core
    gallery-database
    gallery-event
    gallery-photo
    gallery-media
    gallery-util
    ${GSTLIB_LDFLAGS}
    )
