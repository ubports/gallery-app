isEmpty(PREFIX) {
	PREFIX = /usr/local
}

TEMPLATE = app
TARGET = ubuntu-gallery
CONFIG += qt link_pkgconfig
QMAKE_CXXFLAGS += -Werror -Wno-unused-parameter
DEFINES += "INSTALL_PREFIX=\\\"$$PREFIX\\\""
QT += gui qml quick opengl sql
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build
PKGCONFIG += exiv2
DEFINES += QT_USE_QSTRINGBUILDER

include(../coverage.pri)

# Input

SOURCES += \
  gallery-application.cpp \
  main.cpp \
  album/album.cpp \
  album/album-default-template.cpp \
  album/album-collection.cpp \
  album/album-page.cpp \
  album/album-template.cpp \
  album/album-template-page.cpp \
  core/command-line-parser.cpp \
  core/container-source.cpp \
  core/container-source-collection.cpp \
  core/data-collection.cpp \
  core/data-object.cpp \
  core/data-source.cpp \
  core/gallery-manager.cpp \
  core/selectable-view-collection.cpp \
  core/source-collection.cpp \
  core/view-collection.cpp \
  database/album-table.cpp \
  database/database.cpp \
  database/media-table.cpp \
  database/photo-edit-table.cpp \
  event/event.cpp \
  event/event-collection.cpp \
  media/media-collection.cpp \
  media/media-monitor.cpp \
  media/media-source.cpp \
  media/preview-manager.cpp \
  photo/photo.cpp \
  photo/photo-caches.cpp \
  photo/photo-edit-state.cpp \
  photo/photo-metadata.cpp \
  qml/gallery-standard-image-provider.cpp \
  qml/gallery-thumbnail-image-provider.cpp \
  qml/qml-album-collection-model.cpp \
  qml/qml-event-collection-model.cpp \
  qml/qml-event-overview-model.cpp \
  qml/qml-media-collection-model.cpp \
  qml/qml-view-collection-model.cpp \
  util/imaging.cpp \
  util/resource.cpp \
  util/sharefile.cpp

HEADERS += \
  gallery-application.h \
  album/album.h \
  album/album-collection.h \
  album/album-default-template.h \
  album/album-page.h \
  album/album-template.h \
  album/album-template-page.h \
  core/command-line-parser.h \
  core/container-source.h \
  core/container-source-collection.h \
  core/data-collection.h \
  core/data-object.h \
  core/data-source.h \
  core/gallery-manager.h \
  core/selectable-view-collection.h \
  core/source-collection.h \
  core/view-collection.h \
  database/album-table.h \
  database/database.h \
  database/media-table.h \
  database/photo-edit-table.h \
  event/event.h \
  event/event-collection.h \
  media/media-collection.h \
  media/media-monitor.h \
  media/media-source.h \
  media/preview-manager.h \
  photo/photo.h \
  photo/photo-caches.h \
  photo/photo-edit-state.h \
  photo/photo-metadata.h \
  qml/gallery-standard-image-provider.h \
  qml/gallery-thumbnail-image-provider.h \
  qml/qml-album-collection-model.h \
  qml/qml-event-collection-model.h \
  qml/qml-event-overview-model.h \
  qml/qml-media-collection-model.h \
  qml/qml-view-collection-model.h \
  util/collections.h \
  util/imaging.h \
  util/resource.h \
  util/sharefile.h \
  util/variants.h

# Install

install_binary.path = $$PREFIX/bin/
install_binary.files = gallery
INSTALLS = install_binary

