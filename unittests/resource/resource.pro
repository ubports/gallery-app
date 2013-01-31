CONFIG += testcase
TARGET = tst_resource

QT += testlib

include(../../coverage.pri)

SOURCES += tst_resource.cpp \
    ../../src/util/resource.cpp

INCLUDEPATH += ../../src
