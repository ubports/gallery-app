CONFIG += testcase
TARGET = tst_commandlineparser

QT += testlib quick gui

BASE_PATH = "../../.."
include($${BASE_PATH}/coverage.pri)

INCLUDEPATH += $${BASE_PATH}/src/qml \
               $${BASE_PATH}/src

SOURCES += tst_commandlineparser.cpp \
    $${BASE_PATH}/src/core/command-line-parser.cpp

HEADERS += \
    $${BASE_PATH}/src/core/command-line-parser.h
