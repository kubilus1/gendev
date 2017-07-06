
# HINT: If makeinfo is missing on Ubuntu, install texinfo package.
#

#SHELL=/bin/sh

SUDO?= sudo
MGET?= wget
MAKE?= make
ORIG_USER:=$(shell whoami)

BUILDDIR?=$(CURDIR)/build
GENDEV?=/opt/toolchains/gen/
TOPDIR=$(CURDIR)

UNAME:=$(shell uname)

PATH := $(GENDEV)/bin:$(PATH)

all: toolchain_build
	echo "Done"

$(BUILDDIR):
	mkdir -p $@

toolchain_build:
	cd toolchain && make toolchain_build

toolchain_clean:
	cd toolchain && make toolchain_clean

all_2: setup toolchain_build $(GENDEV)/ldscripts tools sgdk_build
	echo "export GENDEV=$(GENDEV)" > ~/.gendev
	echo "export PATH=\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.gendev
	cp -r sgdk/skeleton $(GENDEV)/.

32x: setup toolchain_build_full $(GENDEV)/ldscripts tools
	echo "export GENDEV=$(GENDEV)" > ~/.32xdev
	echo "export PATH=\$$GENDEV/sh-elf/bin:\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.32xdev

release: setup toolchain_build $(GENDEV)/ldscripts tools sgdk_build deb gendev.txz
	echo "Release"

gendev.txz: $(GENDEV)/ldscripts pkg/opt
	tar -C pkg  -cJf gendev.txz opt

pkg/opt:
	mkdir -p pkg/opt/toolchains
	cp -r $(GENDEV) pkg/opt/toolchains/.

gendev_1_all.deb: pkg/opt
	dpkg-deb -Zxz -z9 --build pkg .

deb: gendev_1_all.deb

sgdk_build: $(GENDEV)/m68k-elf/lib/libmd.a
$(GENDEV)/m68k-elf/lib/libmd.a:
	cd sgdk && GENDEV=$(GENDEV) make install 	

sgdk_clean:
	- cd sgdk && make clean
	- rm $(GENDEV)/m68k-elf/lib/libmd.a

TOOLSDIR=$(GENDEV)/bin

TOOLS=$(TOOLSDIR)/bin2c
TOOLS+=$(TOOLSDIR)/sjasm
ifeq ($(UNAME), Linux)
TOOLS+=$(TOOLSDIR)/zasm
#TOOLS+=$(TOOLSDIR)/vgm_cmp
endif
TOOLS+=$(TOOLSDIR)/sixpack 
TOOLS+=$(TOOLSDIR)/appack
#TOOLS+=/opt/toolchains

tools: $(TOOLSDIR) $(TOOLS)
	-cp extras/scripts/*.* $(TOOLSDIR)/.
	echo "Done with tools."

clean: clean_tools
	-rm -rf work/gcc-$(GCC_VERSION)
	-rm -rf work/binutils-$(BINUTILS_VERSION)
	-rm -rf work/build-*
	-cd sgdk && make clean

clean_tools:
	-rm -rf work/bin2c
	-rm -rf work/sjasm
	-rm -rf work/zasm
	-rm -rf work/hexbin
	-rm -rf work/sixpack
	-rm -rf work/vgmtools

purge: clean
	- rm -rf work
	- rm gendev.tgz
	- rm gendev*.deb
	- rm -rf pkg/opt

#########################################################
#########################################################
#########################################################

BIN2C_PKG=work/bin2c-1.0.zip
work/bin2c-1.0.zip:
	#cd work && $(MGET) http://downloads.sourceforge.net/project/bin2c/1.0/bin2c-1.0.zip
	cp files/`basename $@` $@

SJASM_PKG=work/sjasm39g6.zip
work/sjasm39g6.zip:
	#cd work && $(MGET) http://home.online.nl/smastijn/sjasm39g6.zip
	cp files/`basename $@` $@

ZASM_PKG=work/zasm-3.0.21-source-linux-2011-06-19.zip
work/zasm-3.0.21-source-linux-2011-06-19.zip:
	#cd work && $(MGET) http://k1.spdns.de/Develop/Projects/zasm/Distributions/old%20versions/zasm-3.0.21-source-linux-2011-06-19.zip 
	cp files/`basename $@` $@

HEXBIN_PKG=work/Hex2bin-1.0.10.tar.bz2
work/Hex2bin-1.0.10.tar.bz2:
	#cd work && $(MGET) http://downloads.sourceforge.net/project/hex2bin/hex2bin/$@
	cp files/`basename $@` $@

#work/genres_01.zip: 
#	cd work && $(MGET) http://gendev.spritesmind.net/files/genres_01.zip

SIXPACK_PKG=work/sixpack-13.zip
work/sixpack-13.zip:
	#cd work && $(MGET) http://jiggawatt.org/badc0de/sixpack/sixpack-13.zip
	cp files/`basename $@` $@

VGMTOOL_PKG=work/VGMTools_src.rar
work/VGMTools_src.rar:
	#$(MGET) -O $@ http://www.smspower.org/forums/download.php?id=3201
	cp files/`basename $@` $@

#########################################################
#########################################################
#########################################################

#########################################################
#########################################################
#########################################################

$(GENDEV):
	if [ -w /opt ]; then \
		mkdir -p $@; \
	else \
		$(SUDO) mkdir -p $@; \
		$(SUDO) chown $(ORIG_USER):$(ORIG_USER) $@; \
	fi
	#$(SUDO) chmod 777 $@

$(TOOLSDIR):
	[ -d $@ ] || mkdir $@

$(TOOLSDIR)/bin2c: $(BIN2C_PKG)
	cd work && \
	unzip bin2c-1.0.zip && \
	cd bin2c && \
	gcc bin2c.c -o bin2c && \
	cp bin2c $@ 

$(TOOLSDIR)/sjasm: $(SJASM_PKG)
	- mkdir -p work/sjasm
	cd work/sjasm && \
	unzip ../sjasm39g6.zip && \
	cd sjasmsrc39g6 && \
	$(MAKE) && \
	cp sjasm $@ && \
	chmod +x $@

$(TOOLSDIR)/zasm: $(ZASM_PKG)
	- mkdir -p work/zasm 
	cd work/zasm && \
	unzip ../zasm-3.0.21-source-linux-2011-06-19.zip && \
	cd zasm-3.0.21-i386-ubuntu-linux-2011-06-19/source && \
	$(MAKE) && \
	cp zasm $@

$(TOOLSDIR)/hex2bin: $(HEXBIN_PKG)
	cd work && \
	tar xjf $< && \
	cp Hex2bin-1.0.10/hex2bin $@

$(TOOLSDIR)/sixpack: $(SIXPACK_PKG)
	- mkdir -p work/sixpack && \
	cd work/sixpack && \
	unzip ../sixpack-13.zip 
	cp work/sixpack/sixpack-12/bin/sixpack $@ 
	chmod +x $@	

#genres $(TOOLSDIR)/genres: genres_01.zip
#	- mkdir -p work/genres && \
#	cd work/genres && \
#	unzip ../../$< 
	

$(TOOLSDIR)/vgm_cmp: $(VGMTOOL_PKG)
	- mkdir -p work/vgmtools
	cd work/vgmtools && \
	unrar x ../../$< 
	cd work/vgmtools && \
	patch -u < ../../files/vgm_cmp.diff && \
	gcc -c chip_cmp.c -o chip_cmp.o && \
	gcc chip_cmp.o vgm_cmp.c -lz -o vgm_cmp && \
	cp vgm_cmp $@

$(TOOLSDIR)/appack: 
	- mkdir -p work/applib 
	cp -r files/applib/* work/applib/. 
	cd work/applib/example && \
	make -f makefile.elf && \
	cp appack $@
