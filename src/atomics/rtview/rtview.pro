#-------------------------------------------------
#
# Project created by QtCreator 2012-10-01T09:48:35
#
#-------------------------------------------------

TARGET = ../../../atomics/realtime/rtview
TEMPLATE = app
CONFIG += debug

include(./qtsingleapplication/src/qtsingleapplication.pri)

SOURCES += main.cpp\
        rtview.cpp

HEADERS  += rtview.h \
            lcd.h

FORMS    += rtview.ui \
            lcd.ui \
            knob.ui
