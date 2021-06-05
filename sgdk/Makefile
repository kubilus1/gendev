
# Locatin of gendev install
GENDEV?=/opt/gendev/
GENBIN=$(GENDEV)/bin

# Location of build and source
BUILDDIR?=$(CURDIR)/build
SGDKDIR=$(BUILDDIR)/sgdk
SAMPLESDIR=$(SGDKDIR)/sample

# Version of SGDK to build
SGDKVER=v1.62

# SGDK install location
SGDKINSTALLDIR=$(GENDEV)/sgdk$(SGDKVER)

all: $(SGDKDIR) $(SGDKDIR)/lib/libmd.a tools

# Various tools needed to use SGDK
tools: $(SGDKDIR) $(SGDKINSTALLDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
	gcc -o bintos bintos.c
	cd $(SGDKDIR)/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c
	cd $(SGDKDIR)/tools/xgmtool && \
		$(MAKE) -f $(CURDIR)/mkfiles/Makefile.xgmtool
	cd $(SGDKDIR)/tools/bintos/src && \
		cp bintos $(SGDKINSTALLDIR)/bin/.
	cd $(SGDKDIR)/tools/sizebnd/src && \
		cp sizebnd $(SGDKINSTALLDIR)/bin/.
	cd $(SGDKDIR)/tools/xgmtool && \
		cp out/xgmtool $(SGDKINSTALLDIR)/bin/.
	cp $(SGDKDIR)/bin/*.jar $(SGDKINSTALLDIR)/bin/.


# Create a new patchfile based on the current SGDK checkout
patch: $(SGDKDIR)
	cd $(SGDKDIR) && git diff . > $(CURDIR)/patches/sgdk_$(SGDKVER).diff

# Update an SGDK checkout with the current versions patchfile
prep: $(SGDKDIR)
	cd $(SGDKDIR) && patch -u -p1 -l < $(CURDIR)/patches/sgdk_$(SGDKVER).diff

$(BUILDDIR):
	mkdir -p $@

# Checkout and prep SGDK for building
SGDK: $(SGDKDIR)
$(SGDKDIR): $(BUILDDIR)
	cd $(BUILDDIR) && git clone -b '$(SGDKVER)' --single-branch --depth 1 https://github.com/Stephane-D/SGDK.git sgdk
	cd $(SGDKDIR) && patch -u -p1 -l < $(CURDIR)/patches/sgdk_$(SGDKVER).diff
	rm -f $(SGDKDIR)/lib/*.a
	rm -f $(SGDKDIR)/bin/*.exe

$(SGDKDIR)/mkfiles: $(SGDKDIR)
	mkdir -p $(SGDKDIR)/mkfiles
	cp -f $(CURDIR)/mkfiles/makefile.vars $@/.
	cp -f $(CURDIR)/mkfiles/Makefile.sgdk_lib $@/.
	cp -f $(CURDIR)/mkfiles/Makefile.rom $@/.

$(SGDKDIR)/lib/libmd.a: $(SGDKDIR) $(SGDKDIR)/mkfiles tools
	cd $(SGDKDIR) && SGDKDIR=$(SGDKDIR) $(MAKE) -f $(CURDIR)/mkfiles/Makefile.sgdk_lib
	cp $(GENDEV)/lib/libgcc.a $(SGDKDIR)/lib/libgcc.a
	mv $(SGDKDIR)/libmd.a $(SGDKDIR)/lib/libmd.a
	@echo "Done building sgdk library"	

$(SGDKINSTALLDIR):
	mkdir -p $@
	rm -f $(GENDEV)/sgdk
	cd $(GENDEV) && ln -sf sgdk$(SGDKVER) sgdk
	mkdir -p $@/src
	mkdir -p $@/bin
	mkdir -p $@/inc
	mkdir -p $@/res
	mkdir -p $@/lib
	mkdir -p $@/mkfiles

install: $(SGDKDIR)/lib/libmd.a $(SGDKINSTALLDIR)
	echo "Install"
	cp $(SGDKDIR)/md.ld $(SGDKINSTALLDIR)/.
	cp $(SGDKDIR)/inc/* $(SGDKINSTALLDIR)/inc/.
	cp -r $(SGDKDIR)/src/boot $(SGDKINSTALLDIR)/src/.
	cp -r $(SGDKDIR)/res/* $(SGDKINSTALLDIR)/res/.
	cp $(GENDEV)/lib/libgcc.a $(SGDKINSTALLDIR)/lib/.
	cp $(SGDKDIR)/lib/libmd.a $(SGDKINSTALLDIR)/lib/.
	cp $(SGDKDIR)/makefile.gen $(SGDKINSTALLDIR)/mkfiles/.
	cp $(SGDKDIR)/mkfiles/* $(SGDKINSTALLDIR)/mkfiles/.
	cp $(SGDKDIR)/tools/sizebnd/src/sizebnd $(SGDKINSTALLDIR)/bin/
	cd $(SGDKINSTALLDIR)/bin && ln -sf ../../bin/m68k-elf-gcc gcc
	cd $(SGDKINSTALLDIR)/bin && ln -sf ../../bin/m68k-elf-nm nm
	cd $(SGDKINSTALLDIR)/bin && ln -sf ../../bin/m68k-elf-ld ld
	cd $(SGDKINSTALLDIR)/bin && ln -sf ../../bin/m68k-elf-objcopy objcopy
	cd $(SGDKINSTALLDIR)/bin && ln -sf ../../bin/sjasm sjasm


SAMPLES=$(wildcard $(SAMPLESDIR)/*/out)
SAMPLEROMS=$(addsuffix /rom.bin,$(SAMPLES))
CLEANSAMPLES=$(SAMPLES)

.PHONY: $(CLEANSAMPLES)
sample_clean: $(CLEANSAMPLES)
	@echo "Done cleaning samples"

$(CLEANSAMPLES):
	cd $@/.. && $(MAKE) -f $(CURDIR)/mkfiles/Makefile.rom clean

samples: $(SAMPLEROMS)
	@echo "All samples built"

$(SAMPLESDIR)/%/out/rom.bin:
	echo "ROM $@"
	cd $@/../../ && GENDEV=$(GENDEV) $(MAKE) -f $(SGDKINSTALLDIR)/mkfiles/Makefile.rom

clean:
	echo "Clean"
	-rm -rf $(SGDKDIR)
	-rm -rf $(BUILDDIR)
