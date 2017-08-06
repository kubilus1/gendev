GENDEV?=/opt/gendev/
SUDO?= sudo
MAKE?= make
TOPDIR?=$(CURDIR)/..
TCDIR=$(TOPDIR)/toolchain
BUILDDIR?=$(TOPDIR)/build
WORKDIR?=$(TCDIR)/work
FILESDIR?=$(TCDIR)/files

GCC_VERSION=6.3.0
BINUTILS_VERSION=2.24
NEWLIB_VERSION=1.19.0

LOG?=/dev/null

PATH := $(BUILDDIR)/bin:$(PATH)

toolchain_build: $(BUILDDIR) setup build-binutils build-gcc-1 build-newlib build-gcc-2 build-ldscripts
	echo "Toolchain done"

toolchain_params:
	@echo "TOPDIR $(TOPDIR)"
	@echo "TCDIR $(TCDIR)"
	@echo "WORKDIR $(WORKDIR)"
	@echo "BUILDDIR $(BUILDDIR)"
	@echo "FILESDIR $(FILESDIR)"

$(BUILDDIR):
	mkdir -p $@

install:
	mkdir -p $(GENDEV)
	cp -r $(BUILDDIR)/* $(GENDEV)/.

build-binutils: $(WORKDIR)/build-binutils
$(WORKDIR)/build-binutils:
	mkdir -p $@
	cd $@ && ../binutils-$(BINUTILS_VERSION)/configure --target=m68k-elf --prefix=$(BUILDDIR) --enable-install-libbfd --disable-werror
	make -C $@ all install DESTDIR= > $(LOG)

build-gcc-1: $(WORKDIR)/build-gcc
$(WORKDIR)/build-gcc:
	find $(TOPDIR) -name "m68k-elf-cc"
	cd $(WORKDIR)/gcc-$(GCC_VERSION) && ./contrib/download_prerequisites
	mkdir -p $@
	cd $@ && ../gcc-$(GCC_VERSION)/configure --target=m68k-elf --prefix=$(BUILDDIR) --without-headers --with-newlib --enable-languages=c --disable-libssp --disable-tls --with-cpu=m68000 --disable-werror --disable-nls --disable-multilib
	make -C $@ all install DESTDIR= > $(LOG)

build-newlib: $(WORKDIR)/build-newlib
$(WORKDIR)/build-newlib:
	find $(BUILDDIR) -name "m68k-elf-cc"
	find $(WORKDIR) -name "m68k-elf-cc"
	mkdir -p $@
	cd $@ && ../newlib-$(NEWLIB_VERSION)/configure --target=m68k-elf --prefix=$(BUILDDIR) --with-cpu=m68000 --disable-werror
	make -C $@ all install DESTDIR= $(LOG)

build-gcc-2:
	cd $(WORKDIR)/build-gcc &&  ../gcc-$(GCC_VERSION)/configure --target=m68k-elf --prefix=$(BUILDDIR) --with-newlib --disable-libssp --disable-tls --enable-threads=single --enable-languages=c --with-cpu=m68000 --disable-werror --disable-nls --disable-multilib
	make -C $(WORKDIR)/build-gcc all install DESTDIR= $(LOG)
	cp $(BUILDDIR)/lib/gcc/m68k-elf/$(GCC_VERSION)/libgcc.a $(BUILDDIR)/lib/.

build-ldscripts: $(BUILDDIR)/ldscripts
$(BUILDDIR)/ldscripts:
	mkdir -p $@
	cp ldscripts/*.ld $@/.

toolchain_clean:
	rm -rf $(WORKDIR)

#####################################################
#####################################################
#####################################################
TGZS=$(wildcard $(FILESDIR)/*.tar.gz)
BZ2S+=$(wildcard $(FILESDIR)/*.tar.bz2)

ARCH_DIRS=$(TGZS:.tar.gz=)
ARCH_DIRS+=$(BZ2S:.tar.bz2=)

PKGS=$(addprefix $(WORKDIR)/,$(notdir $(ARCH_DIRS)))

$(WORKDIR): 
	mkdir -p $(WORKDIR)

$(WORKDIR)/%: $(FILESDIR)/%
	cd $(WORKDIR) && \
		tar xf $<.*

$(FILESDIR)/%:
	@echo "Will extract $@"

setup: $(WORKDIR) $(PKGS)
	echo $< $@
	@echo "Setup done"
