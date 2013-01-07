include(../../coverage.pri)

CONFIG += testcase
TARGET = tst_gallerythumbnailimageprovider

QT += testlib quick

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

SOURCES += tst_gallerythumbnailimageprovider.cpp \
    ../../src/qml/gallery-thumbnail-image-provider.cpp

INCLUDEPATH += ../../src/qml

HEADERS += \
    gallery-application.h

