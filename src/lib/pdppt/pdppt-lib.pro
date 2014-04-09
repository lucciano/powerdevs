TEMPLATE = lib
CONFIG+=staticlib
TARGET=pdppt

PDPPT=../../pdppt
# Input
HEADERS += $$PDPPT/codegenerator.h \
           $$PDPPT/modelatomic.h \
           $$PDPPT/modelchild.h \
           $$PDPPT/modelcoupled.h \
           $$PDPPT/modelparameter.h \
           $$PDPPT/parser.h
SOURCES += $$PDPPT/codegenerator.cpp $$PDPPT/parser.cpp

unix {
headers.path    = /usr/include/pdppt
headers.files   += $$HEADERS
lib.path = /usr/lib/
lib.files = ./libpdppt.a
}
INSTALLS  += headers lib
