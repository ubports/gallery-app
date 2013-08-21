project(gallery_event_src)

set(GALLERY_EVENT_LIB gallery-event)

include_directories(
    ${gallery_core_src_SOURCE_DIR}
    # because of circulate dependencies the sub project can't be used directly
    # FIXME resolve the circulate dependencies
    ${gallery_src_SOURCE_DIR}/media
    ${gallery_util_src_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

set(gallery_event_HDRS
    event.h
    event-collection.h
    )

set(gallery_event_SRCS
    event.cpp
    event-collection.cpp
    )

add_library(${GALLERY_EVENT_LIB}
    ${gallery_event_SRCS}
    )

qt5_use_modules(${GALLERY_EVENT_LIB} Widgets Core Qml)

