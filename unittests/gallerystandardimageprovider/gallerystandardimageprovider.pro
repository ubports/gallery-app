CONFIG += testcase
TARGET = tst_gallerystandardimageprovider

QT += testlib quick gui

include(../../coverage.pri)

INCLUDEPATH += ../../src/qml \
               ../../src

SOURCES += \
    tst_gallerystandardimageprovidertest.cpp \
    ../../src/qml/gallery-standard-image-provider.cpp \
    gallery-manager.cpp \
    preview-manager.cpp \
    photometa-data.cpp

HEADERS += \
    ../../src/qml/gallery-standard-image-provider.h \
    ../../src/core/gallery-manager.h \
    ../../src/media/preview-manager.h
