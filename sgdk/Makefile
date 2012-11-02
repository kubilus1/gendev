GENDEV!=/opt/toolchains/gen/

all: sgdk-read-only sgdk-read-only/src/libmd.a tools

tools: sgdk-read-only
	cd sgdk-read-only/tools/bintos/src && gcc -o bintos bintos.c  && cp bintos $(GENDEV)/bin/.
	cd sgdk-read-only/tools/pcmtoraw/src && gcc -o pcmtoraw pcmtoraw.c  && cp pcmtoraw $(GENDEV)/bin/.
	cd sgdk-read-only/tools/sizebnd/src && gcc -o sizebnd sizebnd.c  && cp sizebnd $(GENDEV)/bin/.

sgdk-read-only:
	svn co http://sgdk.googlecode.com/svn/trunk/ sgdk-read-only

sgdk-read-only/src/libmd.a:
	cp files/Makefile.sgdk_lib sgdk-read-only/.
	cd sgdk-read-only/src && patch -u < ../../files/z80_drvrs.diff
	cd sgdk-read-only/src && make -f ../Makefile.sgdk_lib 
	cp sgdk-read-only/src/boot/* skeleton/.


install: sgdk-read-only/src/libmd.a
	echo "Install"
	cd sgdk-read-only/src && make -f ../Makefile.sgdk_lib install

clean:
	echo "Clean"
	cd sgdk-read-only/src && make -f ../Makefile.sgdk_lib clean
	svn revert sgdk-read-only/src/*

purge: clean
	echo "Purge"
	rm -rf sgdk-read-only
