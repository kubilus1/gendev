#
# HINT: If makeinfo is missing on Ubuntu, install texinfo package.
#

#SHELL=/bin/sh

MGET?= wget
MAKE?= make

UNAMEO:=$(shell uname)

GCC_VERSION=4.8.2
MPFR_VERSION=2.4.2
MPC_VERSION=0.8.2
GMP_VERSION=5.0.5
BINUTILS_VERSION=2.24
NEWLIB_VERSION=1.19.0

FILES=gcc-$(GCC_VERSION).tar.bz2 \
	  mpfr-$(MPFR_VERSION).tar.bz2 mpc-$(MPC_VERSION).tar.gz gmp-$(GMP_VERSION).tar.bz2 \
	  binutils-$(BINUTILS_VERSION).tar.bz2 newlib-$(NEWLIB_VERSION).tar.gz

all: setup build postbuild

setup: work $(FILES) work/gcc-$(GCC_VERSION) work/gcc-$(GCC_VERSION)/mpfr work/gcc-$(GCC_VERSION)/mpc work/gcc-$(GCC_VERSION)/gmp work/binutils-$(BINUTILS_VERSION) work/newlib-$(NEWLIB_VERSION) work/makefile-gen

gendev.txz: /opt/toolchains/gen/ldscripts
	tar cJvf gendev.txz /opt/toolchains/gen

pkg/opt:
	mkdir -p pkg/opt/toolchains
	cp -r /opt/toolchains/gen pkg/opt/toolchains/.

gendev_1_all.deb: pkg/opt
	dpkg-deb -Zxz -z9 --build pkg .

build: /opt/toolchains/gen
	echo "Build"
	cd work && \
	patch -u < ../files/makefile-gen.diff || true && \
	MAKE=$(MAKE) $(MAKE) -f makefile-gen

postbuild: /opt/toolchains/gen/ldscripts tools
	echo "Post build."
	echo "export GENDEV=/opt/toolchains/gen" > ~/.gendev
	echo "export PATH=\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.gendev
	echo "export GENDEV=/opt/toolchains/gen" > ~/.32xdev
	echo "export PATH=\$$GENDEV/sh-elf/bin:\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.32xdev
	cp -r sgdk/skeleton /opt/toolchains/gen/.

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
	rm -rf work

purge: clean
	- rm *.rar *.bz2 *.gz *.tgz *.zip
	- rm gendev.tgz
	- rm gendev*.deb
	- rm -rf pkg/opt

work:
	mkdir work

#########################################################
#########################################################
#########################################################

gcc-$(GCC_VERSION).tar.bz2:
	$(MGET) http://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.bz2

gcc-g++-$(GCC_VERSION).tar.bz2:
	$(MGET) http://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-g++-$(GCC_VERSION).tar.bz2

gcc-objc-$(GCC_VERSION).tar.bz2:
	$(MGET) http://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-objc-$(GCC_VERSION).tar.bz2

mpfr-$(MPFR_VERSION).tar.bz2:
	$(MGET) http://www.mpfr.org/mpfr-$(MPFR_VERSION)/mpfr-$(MPFR_VERSION).tar.bz2

mpc-$(MPC_VERSION).tar.gz:
	$(MGET) http://www.multiprecision.org/mpc/download/mpc-$(MPC_VERSION).tar.gz

gmp-$(GMP_VERSION).tar.bz2:
	$(MGET) ftp://ftp.gmplib.org/pub/gmp-$(GMP_VERSION)/gmp-$(GMP_VERSION).tar.bz2

binutils-$(BINUTILS_VERSION).tar.bz2:
	$(MGET) http://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.bz2

newlib-$(NEWLIB_VERSION).tar.gz:
	$(MGET) ftp://sources.redhat.com/pub/newlib/newlib-$(NEWLIB_VERSION).tar.gz

bin2c-1.0.zip:
	$(MGET) http://downloads.sourceforge.net/project/bin2c/bin2c-1.0.zip

sjasm39g6.zip:
	$(MGET) http://home.wanadoo.nl/smastijn/sjasm39g6.zip

zasm-3.0.21-source-linux-2011-06-19.zip:
	$(MGET) http://k1.dyndns.org/Develop/projects/zasm/distributions/zasm-3.0.21-source-linux-2011-06-19.zip

Hex2bin-1.0.10.tar.bz2:
	$(MGET) http://downloads.sourceforge.net/project/hex2bin/hex2bin/$@

#genres_01.zip:
#	$(MGET) http://gendev.spritesmind.net/files/genres_01.zip

sixpack-13.zip:
	$(MGET) http://jiggawatt.org/badc0de/sixpack/sixpack-13.zip

VGMTools_src.rar:
	$(MGET) -O $@ http://www.smspower.org/forums/download.php?id=3201

#########################################################
#########################################################
#########################################################

work/makefile-gen:
	cd work && \
	unzip ../files/makefiles-ldscripts-2.zip

work/binutils-$(BINUTILS_VERSION):
	cd work && \
	tar xvjf ../binutils-$(BINUTILS_VERSION).tar.bz2

work/newlib-$(NEWLIB_VERSION):
	cd work && \
	tar xvzf ../newlib-$(NEWLIB_VERSION).tar.gz

work/gcc-$(GCC_VERSION):
	cd work && \
	tar xvjf ../gcc-$(GCC_VERSION).tar.bz2

work/gcc-$(GCC_VERSION)/mpfr: work/gcc-$(GCC_VERSION)
	cd work && \
	tar xvjf ../mpfr-$(MPFR_VERSION).tar.bz2 && \
	mv mpfr-$(MPFR_VERSION) gcc-$(GCC_VERSION)/mpfr

work/gcc-$(GCC_VERSION)/mpc: work/gcc-$(GCC_VERSION)
	cd work && \
	tar xvzf ../mpc-$(MPC_VERSION).tar.gz && \
	mv mpc-$(MPC_VERSION) gcc-$(GCC_VERSION)/mpc

work/gcc-$(GCC_VERSION)/gmp: work/gcc-$(GCC_VERSION)
	cd work && \
	tar xvjf ../gmp-$(GMP_VERSION).tar.bz2 && \
	mv gmp-$(GMP_VERSION) gcc-$(GCC_VERSION)/gmp

#########################################################
#########################################################
#########################################################

/opt/toolchains/gen:
	sudo mkdir -p $@
	sudo chmod 777 /opt/toolchains/gen

$(TOOLSDIR):
	mkdir $@

/opt/toolchains/gen/ldscripts: work/makefile-gen
	mkdir -p $@
	cp work/*.ld $@/.

$(TOOLSDIR)/bin2c: bin2c-1.0.zip
	- mkdir -p work 
	cd work && \
	unzip ../bin2c-1.0.zip && \
	cd bin2c && \
	gcc bin2c.c -o bin2c && \
	cp bin2c $@ 

$(TOOLSDIR)/sjasm: sjasm39g6.zip
	- mkdir -p work/sjasm
	cd work/sjasm && \
	unzip ../../sjasm39g6.zip && \
	cd sjasmsrc39g6 && \
	$(MAKE) && \
	cp sjasm $@ && \
	chmod +x $@

$(TOOLSDIR)/zasm: zasm-3.0.21-source-linux-2011-06-19.zip
	- mkdir -p work/zasm 
	cd work/zasm && \
	unzip ../../$< && \
	cd zasm-3.0.21-i386-ubuntu-linux-2011-06-19/source && \
	$(MAKE) && \
	cp zasm $@

$(TOOLSDIR)/hex2bin: Hex2bin-1.0.10.tar.bz2
	- mkdir -p work 
	cd work && \
	tar xvjf ../$< && \
	cp Hex2bin-1.0.10/hex2bin $@

sixpack $(TOOLSDIR)/sixpack: sixpack-13.zip
	- mkdir -p work/sixpack && \
	cd work/sixpack && \
	unzip ../../$< 
	cp work/sixpack/sixpack-12/bin/sixpack $@ 
	chmod +x $@	

#genres $(TOOLSDIR)/genres: genres_01.zip
#	- mkdir -p work/genres && \
#	cd work/genres && \
#	unzip ../../$< 
	

$(TOOLSDIR)/vgm_cmp: VGMTools_src.rar
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
