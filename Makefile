#
# HINT: If makeinfo is missing on Ubuntu, install texinfo package.
#

FILES=gcc-4.5.2.tar.bz2 gcc-g++-4.5.2.tar.bz2 gcc-objc-4.5.2.tar.bz2 \
	  mpfr-2.4.2.tar.bz2 mpc-0.8.2.tar.gz gmp-5.0.5.tar.bz2 \
	  binutils-2.21.1.tar.bz2 newlib-1.19.0.tar.gz makefiles-ldscripts-2.zip

all: setup build postbuild

setup: work $(FILES) work/gcc-4.5.2 work/gcc-4.5.2/mpfr work/gcc-4.5.2/mpc work/gcc-4.5.2/gmp work/binutils-2.21 work/newlib-1.19.0 work/makefile-gen

build: /opt/toolchains/gen
	echo "Build"
	cd work && \
	make -f makefile-gen

postbuild: /opt/toolchains/gen/ldscripts /opt/toolchains/gen/bin /opt/toolchains/gen/bin/bin2c \
   			/opt/toolchains/gen/bin/sjasm /opt/toolchains/gen/bin/zasm
	echo "Post build."
	echo "export GENDEV=/opt/toolchains/gen" > ~/.gendev
	echo "export PATH=\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.gendev
	echo "export GENDEV=/opt/toolchains/gen" > ~/.32xdev
	echo "export PATH=\$$GENDEV/sh-elf/bin:\$$GENDEV/m68k-elf/bin:\$$GENDEV/bin:\$$PATH" >> ~/.32xdev

clean:
	rm -rf work

purge: clean
	- rm *.rar *.bz2 *.gz *.tgz *.zip

work:
	mkdir work

gcc-4.5.2.tar.bz2:
	wget http://ftp.gnu.org/gnu/gcc/gcc-4.5.2/gcc-4.5.2.tar.bz2

gcc-g++-4.5.2.tar.bz2:
	wget http://ftp.gnu.org/gnu/gcc/gcc-4.5.2/gcc-g++-4.5.2.tar.bz2

gcc-objc-4.5.2.tar.bz2:
	wget http://ftp.gnu.org/gnu/gcc/gcc-4.5.2/gcc-objc-4.5.2.tar.bz2

mpfr-2.4.2.tar.bz2:
	wget http://www.mpfr.org/mpfr-2.4.2/mpfr-2.4.2.tar.bz2

mpc-0.8.2.tar.gz:
	wget http://www.multiprecision.org/mpc/download/mpc-0.8.2.tar.gz

gmp-5.0.5.tar.bz2:
	wget ftp://ftp.gmplib.org/pub/gmp-5.0.5/gmp-5.0.5.tar.bz2

binutils-2.21.1.tar.bz2:
	wget http://ftp.gnu.org/gnu/binutils/binutils-2.21.1.tar.bz2

newlib-1.19.0.tar.gz:
	wget ftp://sources.redhat.com/pub/newlib/newlib-1.19.0.tar.gz

makefiles-ldscripts-2.zip:
	wget http://www.fileden.com/files/2009/2/3/2304902/makefiles-ldscripts-2.zip

bin2c-1.0.zip:
	wget http://downloads.sourceforge.net/project/bin2c/bin2c-1.0.zip

sjasm39g6.zip:
	wget http://home.wanadoo.nl/smastijn/sjasm39g6.zip

zasm-3.0.21-source-linux-2011-06-19.zip:
	wget http://k1.dyndns.org/Develop/projects/zasm/distributions/zasm-3.0.21-source-linux-2011-06-19.zip

Hex2bin-1.0.10.tar.bz2:
	wget http://downloads.sourceforge.net/project/hex2bin/hex2bin/$@

VGMTools_src.rar:
	wget -O $@ http://www.smspower.org/forums/download.php?id=3201

work/makefile-gen:
	cd work && \
	unzip ../makefiles-ldscripts-2.zip

work/binutils-2.21:
	cd work && \
	tar xvjf ../binutils-2.21.1.tar.bz2 && \
	mv binutils-2.21.1 binutils-2.21

work/newlib-1.19.0:
	cd work && \
	tar xvzf ../newlib-1.19.0.tar.gz

work/gcc-4.5.2:
	cd work && \
	tar xvjf ../gcc-4.5.2.tar.bz2 && \
	tar xvjf ../gcc-g++-4.5.2.tar.bz2 && \
	tar xvjf ../gcc-objc-4.5.2.tar.bz2

work/gcc-4.5.2/mpfr: work/gcc-4.5.2
	cd work && \
	tar xvjf ../mpfr-2.4.2.tar.bz2 && \
	mv mpfr-2.4.2 gcc-4.5.2/mpfr

work/gcc-4.5.2/mpc: work/gcc-4.5.2
	cd work && \
	tar xvzf ../mpc-0.8.2.tar.gz && \
	mv mpc-0.8.2 gcc-4.5.2/mpc

work/gcc-4.5.2/gmp: work/gcc-4.5.2
	cd work && \
	tar xvjf ../gmp-5.0.5.tar.bz2 && \
	mv gmp-5.0.5 gcc-4.5.2/gmp

/opt/toolchains/gen:
	sudo mkdir -p $@
	sudo chmod 777 /opt/toolchains/gen

/opt/toolchains/gen/bin:
	mkdir $@

/opt/toolchains/gen/ldscripts: work/makefile-gen
	mkdir $@
	cp work/*.ld $@/.

/opt/toolchains/gen/bin/bin2c: bin2c-1.0.zip
	cd work && \
	unzip ../bin2c-1.0.zip && \
	cd bin2c && \
	gcc bin2c.c -o bin2c && \
	cp bin2c $@ 

/opt/toolchains/gen/bin/sjasm: sjasm39g6.zip
	- mkdir work/sjasm
	cd work/sjasm && \
	unzip ../../sjasm39g6.zip && \
	cp sjasm $@ && \
	chmod +x $@

/opt/toolchains/gen/bin/zasm: zasm-3.0.21-source-linux-2011-06-19.zip
	- mkdir work/zasm 
	cd work/zasm && \
	unzip ../../$< && \
	cd zasm-3.0.21-i386-ubuntu-linux-2011-06-19/source && \
	make && \
	cp zasm $@

/opt/toolchains/gen/bin/hex2bin: Hex2bin-1.0.10.tar.bz2
	cd work && \
	tar xvjf ../$< && \
	cp Hex2bin-1.0.10/hex2bin $@

/opt/toolchains/gen/bin/vgm_cmp: VGMTools_src.rar
	- mkdir work/vgmtools
	cd work/vgmtools && \
	unrar x ../../$< 
	cd work/vgmtools && \
	patch -u < ../../files/vgm_cmp.diff && \
	gcc  -c chip_cmp.c -o chip_cmp.o && \
	gcc -lz chip_cmp.o vgm_cmp.c -o vgm_cmp && \
	cp vgm_cmp $@
