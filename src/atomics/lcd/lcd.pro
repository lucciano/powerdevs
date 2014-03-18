#-------------------------------------------------
#
# Project created by QtCreator 2011-11-08T13:16:16
#
#-------------------------------------------------

TARGET = ../../../bin/lcd
TEMPLATE = app
QMAKE_POST_LINK=$(STRIP) $(TARGET)
QMAKE_STRIP = strip -R .comment


SOURCES += main.cpp\
        lcd.cpp

HEADERS  += lcd.h

FORMS    += lcd.ui
