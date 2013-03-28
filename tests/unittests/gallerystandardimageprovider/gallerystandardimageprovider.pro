CONFIG += testcase
TARGET = tst_gallerystandardimageprovider

QT += testlib quick gui

BASE_PATH = "../../.."
include($${BASE_PATH}/coverage.pri)

INCLUDEPATH += $${BASE_PATH}/src/qml \
               $${BASE_PATH}/src

SOURCES += \
    tst_gallerystandardimageprovidertest.cpp \
    $${BASE_PATH}/src/qml/gallery-standard-image-provider.cpp \
    gallery-manager.cpp \
    preview-manager.cpp \
    photometa-data.cpp

HEADERS += \
    $${BASE_PATH}/src/qml/gallery-standard-image-provider.h \
    $${BASE_PATH}/src/core/gallery-manager.h \
    $${BASE_PATH}/src/media/preview-manager.h
