
# HINT: If makeinfo is missing on Ubuntu, install texinfo package.
#

#SHELL=/bin/sh

SUDO?= sudo
MGET?= wget
MAKE?= make
ORIG_USER:=$(shell whoami)

BUILDDIR?=$(CURDIR)/build
GENDEV?=/opt/gendev/
TOPDIR=$(CURDIR)

PATH := $(BUILDDIR)/bin:$(PATH)

build: toolchain_build tools_build sgdk_build
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

install:
	if [ -w /opt ]; then \
		mkdir -p $(GENDEV); \
	else \
		$(SUDO) mkdir -p $@; \
		$(SUDO) chown $(ORIG_USER):$(ORIG_USER) $@; \
	fi
	echo "export GENDEV=$(GENDEV)" > ~/.gendev
	echo "export PATH=\$$GENDEV/bin:\$$PATH" >> ~/.gendev
	#$(SUDO) chmod 777 $@
	cp -r $(BUILDDIR)/* $(GENDEV)/.

release: deb dist/gendev.txz
	echo "Release"

dist:
	mkdir -p dist

dist/gendev.txz: dist
	tar -C $(BUILDDIR)  -cJf dist/gendev.txz

pkg_build:
	mkdir -p pkg_build/opt/gendev
	cp -r $(BUILDDIR)/* pkg_build/opt/gendev/.
	cp -r pkg/* pkg_build/.

deb: dist dist/gendev_1_all.deb
dist/gendev_1_all.deb: pkg_build
	dpkg-deb -Zxz -z9 --build pkg_build .

sgdk_build: $(GENDEV)/m68k-elf/lib/libmd.a
$(GENDEV)/m68k-elf/lib/libmd.a:
	cd sgdk && GENDEV=$(BUILDDIR) make install 	
	cp -r sgdk/skeleton $(BUILDDIR)/.

sgdk_clean:
	- cd sgdk && make clean
	- rm $(GENDEV)/m68k-elf/lib/libmd.a

clean: tools_clean toolchain_clean sgdk_clean
	-rm -rf $(BUILDDIR)
	-rm -rf pkg_build
	-rm -rf dist
	#-rm -rf work/gcc-$(GCC_VERSION)
	#-rm -rf work/binutils-$(BINUTILS_VERSION)
	#-rm -rf work/build-*
	#-cd sgdk && make clean

#########################################################
#########################################################
#########################################################

#########################################################
#########################################################
#########################################################

