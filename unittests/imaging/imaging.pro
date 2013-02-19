CONFIG += testcase
TARGET = tst_imaging

QT += testlib widgets

include(../../coverage.pri)

SOURCES += tst_imaging.cpp \
    ../../src/util/imaging.cpp

INCLUDEPATH += ../../src
