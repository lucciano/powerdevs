MODE = debug
RTAI = 
all: pdae pdif pdppt pdme
pdae:
	cd ./src/pdae && qmake CONFIG+=$(MODE) 
	make -C ./src/pdae -j 4

pdppt:
	cd ./src/pdppt && qmake CONFIG+=$(MODE) 
	make -C ./src/pdppt -j 4

pdif:
ifeq ($(RTAI),yes)
	cd ./src/pdif && qmake CONFIG+=$(MODE) QMAKE_CXXFLAGS+=-DRTAIOS 
else
	cd ./src/pdif && qmake CONFIG+=$(MODE) 
endif
	make -C ./src/pdif -j 4


pdme:
	cd ./src/pdme_v2 && qmake CONFIG+=$(MODE) 
	make -C ./src/pdme_v2 -j4

clean:
	make -C ./src/pdae clean
	make -C ./src/pdif clean
	make -C ./src/pdppt clean
	make -C ./src/pdme_v2 clean
	rm bin/pdae bin/pdppt bin/pdif bin/pdme
