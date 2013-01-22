CONFIG += testcase
TARGET = tst_gallerythumbnailimageprovider

QT += testlib quick

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

include(../../coverage.pri)

SOURCES += tst_gallerythumbnailimageprovider.cpp \
    ../../src/qml/gallery-thumbnail-image-provider.cpp \
    gallery-manager.cpp \
    preview-manager.cpp

INCLUDEPATH += ../../src/qml \
               ../../src

HEADERS += ../../src/core/gallery-manager.h \
           ../../src/media/preview-manager.h

