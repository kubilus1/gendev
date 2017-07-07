GENDEV?=/opt/toolchains/gen/
MAKE?=make
GENBIN=$(GENDEV)/bin
GENGCC_BIN=$(GENDEV)/bin

SGDKDIR=SGDK


CC = $(GENGCC_BIN)/m68k-elf-gcc 
AR = $(GENGCC_BIN)/m68k-elf-ar 
RANLIB = $(GENGCC_BIN)/m68k-elf-ranlib 
OBJC = $(GENGCC_BIN)/m68k-elf-objcopy 
RM = rm -f 
ASMZ80 = $(GENBIN)/sjasm 
BINTOS = $(GENBIN)/bintos 

OPTION= -Dnologo_ 

CS=$(wildcard extra_libs/*.c)
SS=$(wildcard extra_libs/*.s)
S80S=$(wildcard extra_libs/*.s80)
RESOURCES=$(CS:.c=.o)
RESOURCES+=$(SS:.s=.o)
RESOURCES+=$(S80S:.s80=.o)

extrasmd.a_OBJS=$(RESOURCES)

INCS = -Iextra_incs -I$(SGDKDIR)/include -I$(SGDKDIR)/inc -I$(SGDKDIR)/res
FLAGS = $(OPTION) -m68000 -Wall -O1 -c -fomit-frame-pointer $(INCS) 
FLAGSZ80 = -isrc -iextra_incs -I$(SGDKDIR)/include

all: tools $(SGDKDIR) $(SGDKDIR)/src/libmd.a 

tools: $(SGDKDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
	gcc -o bintos bintos.c  && cp bintos $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/pcmtoraw/src && \
	gcc -o pcmtoraw pcmtoraw.c  && cp pcmtoraw $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c  && cp sizebnd $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/wavtoraw/src && \
	gcc -o wavtoraw wavtoraw.c -lm && cp wavtoraw $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/xgmtool && \
		make -f ../../../files/Makefile.xgmtool && cp out/xgmtool $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/rescomp && \
		patch -u -p0 < ../../../files/rescomp.diff && \
		make -f ../../../files/Makefile.rescomp && cp out/rescomp $(GENDEV)/bin/.
	cp $(SGDKDIR)/bin/lz4w.jar $(GENDEV)/bin/.

patch:
	cd $(SGDKDIR) && git diff * > ../files/sgdk.diff
	#svn diff $(SGDKDIR) > files/sgdk.diff

prep:
	cd $(STDKDIR) && patch -u -p0 < ../files/sgdk.diff

$(SGDKDIR):
	git clone https://github.com/Stephane-D/SGDK.git
	#git clone https://github.com/kubilus1/SGDK.git $(SGDKDIR)
	#svn co http://sgdk.googlecode.com/svn/trunk $(SGDKDIR)
	#patch -u -p0 < files/sgdk.diff

$(SGDKDIR)/libmd.a:
	cd $(SGDKDIR) && ln -sf ../files/Makefile.sgdk_lib .
	cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib 

install: tools $(SGDKDIR) $(SGDKDIR)/libmd.a
	echo "Install"
	cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib install
	install -b $(SGDKDIR)/md.ld $(GENDEV)/ldscripts/sgdk.ld
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
	-rm -rf $(SGDKDIR)
	#-cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib clean
	#cd $(SGDKDIR) && git checkout src
	#cd $(SGDKDIR) && git checkout tools
	#-svn revert -R $(SGDKDIR)/src
	#-svn revert -R $(SGDKDIR)/tools
	-rm extra_libs/*.o
	-rm extra_libs/*.a
	-rm libextrasmd.a

purge: clean
	echo "Purge"
	rm -rf $(SGDKDIR)
