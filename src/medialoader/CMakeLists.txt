project(gallery_medialoader_src)

set(GALLERY_MEDIALOADER_LIB gallery-medialoader)

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    ${gallery_media_src_SOURCE_DIR}
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${GSTLIB_INCLUDE_DIRS}
    )

set(gallery_medialoader_HDRS
    gallery-standard-image-provider.h
    gallery-thumbnail-image-provider.h
    photo-metadata.h
    preview-manager.h
    )

set(gallery_medialoader_SRCS
    gallery-standard-image-provider.cpp
    gallery-thumbnail-image-provider.cpp
    photo-metadata.cpp
    preview-manager.cpp
    )

add_library(${GALLERY_MEDIALOADER_LIB}
    ${gallery_medialoader_SRCS}
    )

qt5_use_modules(${GALLERY_MEDIALOADER_LIB} Core Quick)

target_link_libraries( ${GALLERY_MEDIALOADER_LIB}
    ${GSTLIB_LDFLAGS}
    )
