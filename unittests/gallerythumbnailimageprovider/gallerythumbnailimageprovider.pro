include(../../coverage.pri)

CONFIG += testcase
TARGET = tst_gallerythumbnailimageprovider

QT += testlib quick

SOURCES += tst_gallerythumbnailimageprovider.cpp \
    ../../src/qml/gallery-thumbnail-image-provider.cpp

INCLUDEPATH += ../../src/qml

HEADERS += \
    gallery-application.h

