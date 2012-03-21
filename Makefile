MODE = debug
RTAI = 
all: pdae pdif pdppt pdme
pdae:
	cd ./src/pdae && qmake CONFIG+=$(MODE) 
	make -C ./src/pdae 

pdppt:
	cd ./src/pdppt && qmake CONFIG+=$(MODE) 
	make -C ./src/pdppt 

pdif:
ifeq ($(RTAI),yes)
	cd ./src/pdif && qmake CONFIG+=$(MODE) QMAKE_CXXFLAGS+=-DRTAIOS 
else
	cd ./src/pdif && qmake CONFIG+=$(MODE) 
endif
	make -C ./src/pdif 


pdme:
	cd ./src/pdme_v2 && qmake CONFIG+=$(MODE) 
	make -C ./src/pdme_v2

clean:
	make -C ./src/pdae clean
	make -C ./src/pdif clean
	make -C ./src/pdppt clean
	make -C ./src/pdme_v2 clean
	rm bin/pdae bin/pdppt bin/pdif bin/pdme
