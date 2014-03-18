#-------------------------------------------------
#
# Project created by QtCreator 2012-10-01T09:48:35
#
#-------------------------------------------------

TARGET = ../../../bin/rtview
TEMPLATE = app
CONFIG += debug

include(./qtsingleapplication/src/qtsingleapplication.pri)
QMAKE_STRIP = strip -R .comment
QMAKE_POST_LINK=$(STRIP) $(TARGET)

SOURCES += main.cpp\
        rtview.cpp \
        lcd.cpp \
        knob.cpp

HEADERS  += rtview.h \
            lcd.h \
            knob.h

FORMS    += rtview.ui \
            lcd.ui \
            knob.ui
