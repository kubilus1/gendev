#
# HINT: If makeinfo is missing on Ubuntu, install texinfo package.
#

#SHELL=/bin/sh

MGET?= wget
MAKE?= make
ORIG_USER:=$(shell whoami)

UNAME:=$(shell uname)

GCC_VERSION=4.8.2
MPFR_VERSION=2.4.2
MPC_VERSION=0.8.2
GMP_VERSION=5.0.5
BINUTILS_VERSION=2.24
NEWLIB_VERSION=1.19.0

all: setup toolchain_build /opt/toolchains/gen/ldscripts tools sgdk_build 
	echo "export GENDEV=/opt/toolchains/gen" > ~/.gendev
	echo "export PATH=\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.gendev
	cp -r sgdk/skeleton /opt/toolchains/gen/.

32x: setup toolchain_build_full /opt/toolchains/gen/ldscripts tools
	echo "export GENDEV=/opt/toolchains/gen" > ~/.32xdev
	echo "export PATH=\$$GENDEV/sh-elf/bin:\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.32xdev

setup: \
	work \
	work/gcc-$(GCC_VERSION) \
	work/gcc-$(GCC_VERSION)/mpfr \
	work/gcc-$(GCC_VERSION)/mpc \
	work/gcc-$(GCC_VERSION)/gmp \
	work/binutils-$(BINUTILS_VERSION) \
	work/newlib-$(NEWLIB_VERSION)

gendev.txz: /opt/toolchains/gen/ldscripts
	tar cJvf gendev.txz /opt/toolchains/gen

pkg/opt:
	mkdir -p pkg/opt/toolchains
	cp -r /opt/toolchains/gen pkg/opt/toolchains/.

gendev_1_all.deb: pkg/opt
	dpkg-deb -Zxz -z9 --build pkg .

toolchain_build: work /opt/toolchains/gen
	echo "Build"
	cd work && \
		MAKE=$(MAKE) $(MAKE) -f ../gen_gcc/makefile-gen build-m68k

toolchain_build_full: work /opt/toolchains/gen
	echo "Build"
	cd work && \
		MAKE=$(MAKE) $(MAKE) -f ../gen_gcc/makefile-gen

sgdk_build:
	cd sgdk && make install 	

TOOLSDIR=/opt/toolchains/gen/bin

TOOLS=$(TOOLSDIR)/bin2c
TOOLS+=$(TOOLSDIR)/sjasm
ifeq ($(UNAME), Linux)
TOOLS+=$(TOOLSDIR)/zasm
TOOLS+=$(TOOLSDIR)/vgm_cmp
endif
TOOLS+=$(TOOLSDIR)/sixpack 
TOOLS+=$(TOOLSDIR)/appack
TOOLS+=/opt/toolchains

tools: $(TOOLSDIR) $(TOOLS)
	-cp extras/scripts/* $(TOOLSDIR)/.
	echo "Done with tools."

clean:
	-rm -rf work/gcc-$(GCC_VERSION)
	-rm -rf work/binutils-$(BINUTILS_VERSION)
	-rm -rf work/bin2c
	-rm -rf work/sjasm
	-rm -rf work/zasm
	-rm -rf work/hexbin
	-rm -rf work/sixpack
	-rm -rf work/vgmtools
	-rm -rf work/build-*
	-cd sgdk && make clean

purge: clean
	- rm -rf work
	- rm gendev.tgz
	- rm gendev*.deb
	- rm -rf pkg/opt

work:
	[ -d work ] || mkdir work

#########################################################
#########################################################
#########################################################

GCC_PKG=work/gcc-$(GCC_VERSION).tar.bz2
work/gcc-$(GCC_VERSION).tar.bz2:
	cd work && $(MGET) http://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.bz2
	
#g++: work/gcc-g++-$(GCC_VERSION).tar.bz2
#work/gcc-g++-$(GCC_VERSION).tar.bz2:
#	cd work && $(MGET) http://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-g++-$(GCC_VERSION).tar.bz2

#work/gcc-objc-$(GCC_VERSION).tar.bz2:
#	cd work && $(MGET) http://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-objc-$(GCC_VERSION).tar.bz2

MPFR_PKG=work/mpfr-$(MPFR_VERSION).tar.bz2
work/mpfr-$(MPFR_VERSION).tar.bz2: 
	cd work && $(MGET) http://www.mpfr.org/mpfr-$(MPFR_VERSION)/mpfr-$(MPFR_VERSION).tar.bz2

MPC_PKG=work/mpc-$(MPC_VERSION).tar.gz
work/mpc-$(MPC_VERSION).tar.gz: 
	cd work && $(MGET) http://www.multiprecision.org/mpc/download/mpc-$(MPC_VERSION).tar.gz

GMP_PKG=work/gmp-$(GMP_VERSION).tar.bz2
work/gmp-$(GMP_VERSION).tar.bz2:
	cd work && $(MGET) ftp://ftp.gmplib.org/pub/gmp-$(GMP_VERSION)/gmp-$(GMP_VERSION).tar.bz2

BINUTILS_PKG=work/binutils-$(BINUTILS_VERSION).tar.bz2
work/binutils-$(BINUTILS_VERSION).tar.bz2:
	cd work && $(MGET) http://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.bz2

NEWLIB_PKG=work/newlib-$(NEWLIB_VERSION).tar.gz
work/newlib-$(NEWLIB_VERSION).tar.gz:
	cd work && $(MGET) ftp://sources.redhat.com/pub/newlib/newlib-$(NEWLIB_VERSION).tar.gz

BIN2C_PKG=work/bin2c-1.0.zip
work/bin2c-1.0.zip:
	cd work && $(MGET) http://downloads.sourceforge.net/project/bin2c/1.0/bin2c-1.0.zip

SJASM_PKG=work/sjasm39g6.zip
work/sjasm39g6.zip:
	cd work && $(MGET) http://home.online.nl/smastijn/sjasm39g6.zip

ZASM_PKG=work/zasm-3.0.21-source-linux-2011-06-19.zip
work/zasm-3.0.21-source-linux-2011-06-19.zip:
	cd work && $(MGET) http://k1.dyndns.org/Develop/projects/zasm/distributions/zasm-3.0.21-source-linux-2011-06-19.zip

HEXBIN_PKG=work/Hex2bin-1.0.10.tar.bz2
work/Hex2bin-1.0.10.tar.bz2:
	cd work && $(MGET) http://downloads.sourceforge.net/project/hex2bin/hex2bin/$@

#work/genres_01.zip: 
#	cd work && $(MGET) http://gendev.spritesmind.net/files/genres_01.zip

SIXPACK_PKG=work/sixpack-13.zip
work/sixpack-13.zip:
	cd work && $(MGET) http://jiggawatt.org/badc0de/sixpack/sixpack-13.zip

VGMTOOL_PKG=work/VGMTools_src.rar
work/VGMTools_src.rar:
	cd work && $(MGET) -O $@ http://www.smspower.org/forums/download.php?id=3201

#########################################################
#########################################################
#########################################################

work/binutils-$(BINUTILS_VERSION): $(BINUTILS_PKG)
	cd work && \
	tar xvjf binutils-$(BINUTILS_VERSION).tar.bz2

work/newlib-$(NEWLIB_VERSION): $(NEWLIB_PKG)
	cd work && \
	tar xvzf newlib-$(NEWLIB_VERSION).tar.gz

work/gcc-$(GCC_VERSION): $(GCC)
	cd work && \
	tar xvjf gcc-$(GCC_VERSION).tar.bz2

work/gcc-$(GCC_VERSION)/mpfr: work/gcc-$(GCC_VERSION) $(MPFR_PKG)
	cd work && \
	tar xvjf mpfr-$(MPFR_VERSION).tar.bz2 && \
	mv mpfr-$(MPFR_VERSION) gcc-$(GCC_VERSION)/mpfr

work/gcc-$(GCC_VERSION)/mpc: work/gcc-$(GCC_VERSION) $(MPC_PKG)
	cd work && \
	tar xvzf mpc-$(MPC_VERSION).tar.gz && \
	mv mpc-$(MPC_VERSION) gcc-$(GCC_VERSION)/mpc

work/gcc-$(GCC_VERSION)/gmp: work/gcc-$(GCC_VERSION) $(GMP_PKG)
	cd work && \
	tar xvjf gmp-$(GMP_VERSION).tar.bz2 && \
	mv gmp-$(GMP_VERSION) gcc-$(GCC_VERSION)/gmp

#########################################################
#########################################################
#########################################################

/opt/toolchains/gen:
	if [ -w /opt ]; then \
		mkdir -p $@; \
	else \
		sudo mkdir -p $@; \
		sudo chown $(ORIG_USER):$(ORIG_USER) $@; \
	fi
	#sudo chmod 777 $@

$(TOOLSDIR):
	mkdir $@

/opt/toolchains/gen/ldscripts:
	mkdir -p $@
	cp gen_gcc/*.ld $@/.

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

$(TOOLSDIR)/zasm: $(SJASM_PKG)
	- mkdir -p work/zasm 
	cd work/zasm && \
	unzip ../$< && \
	cd zasm-3.0.21-i386-ubuntu-linux-2011-06-19/source && \
	$(MAKE) && \
	cp zasm $@

$(TOOLSDIR)/hex2bin: $(HEXBIN_PKG)
	cd work && \
	tar xvjf $< && \
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
	unrar x ../$< 
	cd work/vgmtools && \
	patch -u < ../files/vgm_cmp.diff && \
	gcc -c chip_cmp.c -o chip_cmp.o && \
	gcc chip_cmp.o vgm_cmp.c -lz -o vgm_cmp && \
	cp vgm_cmp $@

$(TOOLSDIR)/appack: 
	- mkdir -p work/applib 
	cp -r files/applib/* work/applib/. 
	cd work/applib/example && \
	make -f makefile.elf && \
	cp appack $@
