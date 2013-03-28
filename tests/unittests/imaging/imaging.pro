CONFIG += testcase
TARGET = tst_imaging

QT += testlib widgets

BASE_PATH = "../../.."
include($${BASE_PATH}/coverage.pri)

SOURCES += tst_imaging.cpp \
    $${BASE_PATH}/src/util/imaging.cpp

INCLUDEPATH += $${BASE_PATH}/src
