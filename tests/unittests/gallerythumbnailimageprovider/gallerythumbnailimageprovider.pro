CONFIG += testcase
TARGET = tst_gallerythumbnailimageprovider

QT += testlib quick

BASE_PATH = "../../.."
include($${BASE_PATH}/coverage.pri)

SOURCES += tst_gallerythumbnailimageprovider.cpp \
    $${BASE_PATH}/src/qml/gallery-thumbnail-image-provider.cpp \
    gallery-manager.cpp \
    preview-manager.cpp

INCLUDEPATH += $${BASE_PATH}/src/qml \
               $${BASE_PATH}/src

HEADERS += $${BASE_PATH}/src/core/gallery-manager.h \
           $${BASE_PATH}/src/media/preview-manager.h

