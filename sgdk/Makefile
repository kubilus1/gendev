GENDEV?=/opt/toolchains/gen/
MAKE?=make

CC = m68k-elf-gcc 
AR = m68k-elf-ar 
RANLIB = m68k-elf-ranlib 
OBJC = m68k-elf-objcopy 
RM = rm -f 
ASMZ80 = sjasm 
BINTOS = bintos 

OPTION= -Dnologo_ 

CS=$(wildcard extra_libs/*.c)
SS=$(wildcard extra_libs/*.s)
S80S=$(wildcard extra_libs/*.s80)
RESOURCES=$(CS:.c=.o)
RESOURCES+=$(SS:.s=.o)
RESOURCES+=$(S80S:.s80=.o)

extrasmd.a_OBJS=$(RESOURCES)

INCS = -Iextra_incs -Isgdk-read-only/include -Isgdk-read-only/inc -Isgdk-read-only/res
FLAGS = $(OPTION) -m68000 -Wall -O1 -c -fomit-frame-pointer $(INCS) 
FLAGSZ80 = -isrc -iextra_incs -Isgdk-read-only/include

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
	cd sgdk-read-only/tools/xgmtool && \
		make -f ../../../files/Makefile.xgmtool && cp out/xgmtool $(GENDEV)/bin/.
	cd sgdk-read-only/tools/rescomp && \
		make -f ../../../files/Makefile.rescomp && cp out/rescomp $(GENDEV)/bin/.

patch: 
	svn diff sgdk-read-only > files/sgdk.diff

sgdk-read-only:
	svn co http://sgdk.googlecode.com/svn/trunk/@r206 sgdk-read-only
	patch -u -p0 < files/sgdk.diff

sgdk-read-only/libmd.a:
	cd sgdk-read-only && ln -sf ../files/Makefile.sgdk_lib .
	cd sgdk-read-only && $(MAKE) -f Makefile.sgdk_lib 

install: tools sgdk-read-only sgdk-read-only/libmd.a
	echo "Install"
	cd sgdk-read-only && $(MAKE) -f Makefile.sgdk_lib install
	install -b sgdk-read-only/md.ld $(GENDEV)/ldscripts/sgdk.ld
	#install -b files/sgdk.ld $(GENDEV)/ldscripts/sgdk.ld

%.a: $(extasmd.a_OBJS)
	$(RM) $@ 
	$(AR) cru $@ $($@_OBJS) 
	$(RANLIB) $@ 

%.o80: %.s80 
	$(ASMZ80) $(FLAGSZ80) $< $@ out.lst 

%.s: %.o80 
	$(BINTOS) $< 

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(FLAGS) -c $< -o $@

extras: $(extrasmd.a_OBJS) libextrasmd.a
	echo "Extras"
	#cp libextrasmd.a $(GENDEV)/m68k-elf/lib
	cp extra_libs/*.o $(GENDEV)/m68k-elf/lib
	cp extra_incs/*.h $(GENDEV)/m68k-elf/include

clean:
	echo "Clean"
	-cd sgdk-read-only && $(MAKE) -f Makefile.sgdk_lib clean
	#-svn revert -R sgdk-read-only/.
	-rm extra_libs/*.o
	-rm extra_libs/*.a
	-rm libextrasmd.a

purge: clean
	echo "Purge"
	rm -rf sgdk-read-only
