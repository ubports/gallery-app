project(gallery_core_src)

set(GALLERY_CORE_LIB gallery-core)

include_directories(
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_core_HDRS
    container-source.h
    container-source-collection.h
    data-collection.h
    data-object.h
    data-source.h
    selectable-view-collection.h
    source-collection.h
    view-collection.h
    )

set(gallery_core_SRCS
    container-source.cpp
    container-source-collection.cpp
    data-collection.cpp
    data-object.cpp
    data-source.cpp
    selectable-view-collection.cpp
    source-collection.cpp
    view-collection.cpp
    )

add_library(${GALLERY_CORE_LIB}
    ${gallery_core_SRCS}
    )

qt5_use_modules(${GALLERY_CORE_LIB} Core Quick)

