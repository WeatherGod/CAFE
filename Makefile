SHELL = /bin/sh
.SUFFIXES: 


VERSION = -1.0
CAFEDIRS = AnalysisInfo Forecast SavedTrainings CorrelationCalcs scratch logs lib bin
MAKECAFES = SPAnalysis CAFEConfig CAFEUtils CAFE Utly PeakVal

.PHONY : all clean clean-depends $(MAKECAFES) tar tar.gz tar.bz2

all : $(CAFEDIRS)
	cd src; $(MAKE) all

$(MAKECAFES) :
	cd src; $(MAKE) $@

$(CAFEDIRS) :
	mkdir $@

clean :
	-rm -f "CAFE$(VERSION).tar"
	-rm -f "CAFE$(VERSION).tar.gz"
	-rm -f "CAFE$(VERSION).tar.bz2"
	cd src; $(MAKE) clean

clean-depends :
	cd src; $(MAKE) clean-depends

tar : CAFE$(VERSION).tar

tar.gz : CAFE$(VERSION).tar.gz

tar.bz2 : CAFE$(VERSION).tar.bz2

CAFE$(VERSION).tar.gz : CAFE$(VERSION).tar
	gzip -f $<

CAFE$(VERSION).tar.bz2 : CAFE$(VERSION).tar
	bzip2 -f $<

CAFE$(VERSION).tar :
	mkdir "CAFE$(VERSION)/"
	mkdir "CAFE$(VERSION)/src"
	mkdir "CAFE$(VERSION)/src/SPAnalysis"
	mkdir "CAFE$(VERSION)/src/Config"
	mkdir "CAFE$(VERSION)/src/Utils"
	mkdir "CAFE$(VERSION)/include"
	mkdir "CAFE$(VERSION)/include/SPAnalysis"
	mkdir "CAFE$(VERSION)/include/Config"
	mkdir "CAFE$(VERSION)/include/Utils"
	mkdir "CAFE$(VERSION)/mFiles"
	mkdir "CAFE$(VERSION)/docs"
	mkdir "CAFE$(VERSION)/scripts"
	mkdir "CAFE$(VERSION)/GradScripts"
	mkdir "CAFE$(VERSION)/bin"
	mkdir "CAFE$(VERSION)/lib"
	cp -f Makefile include.mk README TestTableConfig startup.m "CAFE$(VERSION)/"
	cp -f src/*.C src/Makefile "CAFE$(VERSION)/src/"
	cp -f src/SPAnalysis/*.C "CAFE$(VERSION)/src/SPAnalysis/"
	cp -f src/Config/*.C "CAFE$(VERSION)/src/Config/"
	cp -f src/Utils/*.C "CAFE$(VERSION)/src/Utils/"
	cp -f include/SPAnalysis/*.h "CAFE$(VERSION)/include/SPAnalysis/"
	cp -f include/Config/*.h "CAFE$(VERSION)/include/Config/"
	cp -f include/Utils/*.h "CAFE$(VERSION)/include/Utils/"
	cp -f mFiles/*.m mFiles/*.shp mFiles/*.shx mFiles/*.dbf "CAFE$(VERSION)/mFiles/"
	cp -f docs/* "CAFE$(VERSION)/docs/"
	cp -f scripts/* "CAFE$(VERSION)/scripts/"
	cp -f GradScripts/* "CAFE$(VERSION)/GradScripts/"
	-rm -f $@
	tar -cf $@ "CAFE$(VERSION)/"
	rm -rf "CAFE$(VERSION)/"

