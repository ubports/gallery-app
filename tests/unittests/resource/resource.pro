CONFIG += testcase
TARGET = tst_resource

QT += testlib

BASE_PATH = "../../.."
include($${BASE_PATH}/coverage.pri)

SOURCES += tst_resource.cpp \
    $${BASE_PATH}/src/util/resource.cpp

INCLUDEPATH += $${BASE_PATH}/src
