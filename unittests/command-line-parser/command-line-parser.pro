include(../../coverage.pri)

CONFIG += testcase
TARGET = tst_commandlineparser

QT += testlib quick gui

INCLUDEPATH += ../../src/qml \
               ../../src


SOURCES += tst_commandlineparser.cpp \
    ../../src/core/command-line-parser.cpp

HEADERS += \
    ../../src/core/command-line-parser.h
