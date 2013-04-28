GENDEV?=/opt/toolchains/gen/
MAKE?=make

all: tools sgdk-read-only sgdk-read-only/src/libmd.a

tools: sgdk-read-only
	cd sgdk-read-only/tools/bintos/src && \
	gcc -o bintos bintos.c  && cp bintos $(GENDEV)/bin/.
	cd sgdk-read-only/tools/pcmtoraw/src && \
	gcc -o pcmtoraw pcmtoraw.c  && cp pcmtoraw $(GENDEV)/bin/.
	cd sgdk-read-only/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c  && cp sizebnd $(GENDEV)/bin/.
	cd sgdk-read-only/tools/wavtoraw/src && \
	gcc -o wavtoraw wavtoraw.c -lm && cp wavtoraw $(GENDEV)/bin/.

sgdk-read-only:
	svn co http://sgdk.googlecode.com/svn/trunk/ sgdk-read-only

sgdk-read-only/src/libmd.a:
	cp files/Makefile.sgdk_lib sgdk-read-only/.
	cd sgdk-read-only/src && $(MAKE) -f ../Makefile.sgdk_lib 

install: tools sgdk-read-only sgdk-read-only/src/libmd.a
	echo "Install"
	cd sgdk-read-only/src && $(MAKE) -f ../Makefile.sgdk_lib install
	install -b files/sgdk.ld $(GENDEV)/ldscripts/sgdk.ld

clean:
	echo "Clean"
	-cd sgdk-read-only/src && $(MAKE) -f ../Makefile.sgdk_lib clean
	-svn revert -R sgdk-read-only/.

purge: clean
	echo "Purge"
	rm -rf sgdk-read-only
