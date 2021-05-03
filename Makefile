#
# HINT: If makeinfo is missing on Ubuntu, install texinfo package.
#

SUDO?= sudo
MGET?= wget
MAKE?= make

ORIG_USER:=$(shell whoami)

BUILDDIR?=$(CURDIR)/build
GENDEV?=/opt/gendev/
TOPDIR=$(CURDIR)

VER?=dev

PATH := $(BUILDDIR)/bin:$(PATH)

build: toolchain_build tools_build sgdk_build
	echo "Done"

docker_build:
	docker build -t gendev .

$(BUILDDIR):
	mkdir -p $@

stepbystep:
	make clean
	make toolchain_clean
	cd toolchain && make setup
	cd toolchain && make build-binutils
	cd toolchain && make build-gcc-1
	cd toolchain && make build-newlib
	cd toolchain && make build-gcc-2
	cd toolchain && make build-ldscripts

toolchain_build:
	cd toolchain && $(MAKE) toolchain_build

toolchain_clean:
	cd toolchain && $(MAKE) toolchain_clean

tools_build:
	cd tools && $(MAKE) tools_build

tools_clean:
	cd tools && $(MAKE) tools_clean

$(GENDEV):
	if [ -w /opt ]; then \
		mkdir -p $(GENDEV); \
	else \
		$(SUDO) mkdir -p $@; \
		$(SUDO) chown $(ORIG_USER):$(ORIG_USER) $@; \
	fi

install: $(GENDEV)
	echo "export GENDEV=$(GENDEV)" > ~/.gendev
	echo "export PATH=\$$GENDEV/bin:\$$PATH" >> ~/.gendev
	#$(SUDO) chmod 777 $@
	cp -r $(BUILDDIR)/* $(GENDEV)/.

release: deb txz
	echo "Release"

dist:
	mkdir -p dist

txz: dist/gendev_$(VER).txz
dist/gendev_$(VER).txz: dist pkg_build
	tar -C pkg_build  -cJf dist/gendev_$(VER).txz opt

pkg_build:
	mkdir -p pkg_build/opt/gendev
	cp -r $(BUILDDIR)/* pkg_build/opt/gendev/.
	cp -r pkg/* pkg_build/.

deb: dist dist/gendev_$(VER)_all.deb
dist/gendev_$(VER)_all.deb: pkg_build
	sed -i 's/##VER##/$(VER)/g' pkg_build/DEBIAN/control
	cd dist && dpkg-deb -Zxz -z9 --build $(TOPDIR)/pkg_build .

sgdk_build:
	cd sgdk && GENDEV=$(BUILDDIR) $(MAKE)

sgdk_install: $(GENDEV)
	cd sgdk && $(MAKE) install

sgdk_clean:
	- cd sgdk && $(MAKE) clean

sgdk_samples:
	cd sgdk && $(MAKE) sample_clean samples

clean: tools_clean toolchain_clean sgdk_clean
	-rm -rf $(BUILDDIR)
	-rm -rf pkg_build
	-rm -rf dist

#########################################################
#########################################################
#########################################################

#########################################################
#########################################################
#########################################################
