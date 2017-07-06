
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

all: toolchain_build tools_build
	echo "Done"

$(BUILDDIR):
	mkdir -p $@

toolchain_build:
	cd toolchain && make toolchain_build

toolchain_clean:
	cd toolchain && make toolchain_clean

tools_build: 
	cd tools && make tools_build

tools_clean: 
	cd tools && make tools_clean

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

clean: tools_clean toolchain_clean sgdk_clean
	-rm -rf build
	#-rm -rf work/gcc-$(GCC_VERSION)
	#-rm -rf work/binutils-$(BINUTILS_VERSION)
	#-rm -rf work/build-*
	#-cd sgdk && make clean

purge: clean
	- rm -rf work
	- rm gendev.tgz
	- rm gendev*.deb
	- rm -rf pkg/opt

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

