MODE = debug
RTAI = 
all: pdae pdif pdppt pdme doc/PD_UserGuide.pdf
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

doc/PD_UserGuide.pdf: src/doc/commands.tex src/doc/pd_userguide.kilepr src/doc/chap_intro.tex src/doc/images src/doc/images/pdevsmodel.jpg src/doc/images/pdmodeling.jpg src/doc/images/pdsimulation.jpg src/doc/chap_get_start.tex src/doc/PD_UserGuide.tex
	cd ./src/doc && pdflatex PD_UserGuide.tex -output-directory ../../doc
	cp ./src/doc/PD_UserGuide.pdf ./doc


pdme:
	cd ./src/pdme_v2 && qmake CONFIG+=$(MODE) 
	make -C ./src/pdme_v2 -j4

clean:
	make -C ./src/pdae clean
	make -C ./src/pdif clean
	make -C ./src/pdppt clean
	make -C ./src/pdme_v2 clean
	rm bin/pdae bin/pdppt bin/pdif bin/pdme
