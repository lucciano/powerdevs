TEMPLATE = app
unix {
  TARGET = ../../bin/pdae
}
win32 {
  TARGET = ../../../tmp_win_installer/powerdevs/bin/pdae
}
DEPENDPATH += .
CONFIG += qt 
INCLUDEPATH += .
QMAKE_STRIP = strip -R .comment
QMAKE_POST_LINK=$(STRIP) $(TARGET)

# Input
HEADERS += configure_window.h highlighter.h pdae.h
SOURCES += configure_window.cpp highlighter.cpp main.cpp pdae.cpp
RC_FILE = pdae.rc
