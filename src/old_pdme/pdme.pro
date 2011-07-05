TEMPLATES = app
QT += svg

CONFIG += debug

GRAPHICS_DIR=graphics
DATA_DIR=data

#Graphics sources
SOURCES +=  $$GRAPHICS_DIR/gpxatomic.cpp \
						$$GRAPHICS_DIR/gpxblock.cpp \
						$$GRAPHICS_DIR/gpxconnection.cpp \
						$$GRAPHICS_DIR/gpxconnectionpoint.cpp \
						$$GRAPHICS_DIR/gpxcoupled.cpp \
						$$GRAPHICS_DIR/gpxeditionscene.cpp \
						$$GRAPHICS_DIR/gpxeditionwindow.cpp \
						$$GRAPHICS_DIR/gpxedge.cpp  \
						$$GRAPHICS_DIR/gpxinport.cpp \
						$$GRAPHICS_DIR/gpxoutport.cpp \
			 			$$GRAPHICS_DIR/gpxtextbox.cpp \
						$$GRAPHICS_DIR/libraryscene.cpp \
						$$GRAPHICS_DIR/librarywindow.cpp 

#Data sources
SOURCES += 	$$DATA_DIR/modelcoupled.cpp \
						$$DATA_DIR/parser.cpp 

#General sources
SOURCES += 	main.cpp \
						powerdialogs.cpp \
						powergui.cpp 

#Graphics headers
HEADERS += 	$$GRAPHICS_DIR/gpxatomic.h \
						$$GRAPHICS_DIR/gpxblock.h \
						$$GRAPHICS_DIR/gpxconnection.h \
						$$GRAPHICS_DIR/gpxconnectionpoint.h \
						$$GRAPHICS_DIR/gpxcoupled.h \
						$$GRAPHICS_DIR/gpxedge.h \
						$$GRAPHICS_DIR/gpxeditionscene.h \
						$$GRAPHICS_DIR/gpxeditionwindow.h \
						$$GRAPHICS_DIR/gpxinport.h \
						$$GRAPHICS_DIR/gpxoutport.h \
						$$GRAPHICS_DIR/gpxtextbox.h \
						$$GRAPHICS_DIR/libraryscene.h \
						$$GRAPHICS_DIR/librarywindow.h 
#Data headers
HEADERS += 	$$DATA_DIR/modelatomic.h \
						$$DATA_DIR/modelcoupled.h \
						$$DATA_DIR/modelparameter.h \
						$$DATA_DIR/parser.h 

#General headers
HEADERS += 	powergui.h \
						powerdialogs.h

RESOURCES = pdme.qrc

INCLUDEPATH += $$GRAPHICS_DIR $$DATA_DIR
OBJECTS_DIR = obj
MOC_DIR = moc

TARGET=../../bin/pdme
