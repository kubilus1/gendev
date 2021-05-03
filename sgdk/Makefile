GENDEV?=/opt/gendev/
GENBIN=$(GENDEV)/bin
GENGCC_BIN=$(GENDEV)/bin



BUILDDIR?=$(CURDIR)/build
SGDKDIR?=$(BUILDDIR)/sgdk



SAMPLESDIR=$(SGDKDIR)/sample

SGDKVER=v1.62


SGDKINSTALLDIR=$(GENDEV)/sgdk$(SGDKVER)




all: $(SGDKDIR) $(SGDKDIR)/lib/libmd.a tools

tools: $(SGDKDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
	gcc -o bintos bintos.c
	cd $(SGDKDIR)/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c
	cd $(SGDKDIR)/tools/xgmtool && \
		$(MAKE) -f $(CURDIR)/mkfiles/Makefile.xgmtool

toolsinstall: $(SGDKINSTALLDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
		cp bintos $(SGDKINSTALLDIR)/bin/.
	cd $(SGDKDIR)/tools/sizebnd/src && \
		cp sizebnd $(SGDKINSTALLDIR)/bin/.
	cd $(SGDKDIR)/tools/xgmtool && \
		cp out/xgmtool $(SGDKINSTALLDIR)/bin/.
	cp $(SGDKDIR)/bin/*.jar $(SGDKINSTALLDIR)/bin/.

patch: $(SGDKDIR)
	cd $(SGDKDIR) && git diff . > $(CURDIR)/patches/sgdk_$(SGDKVER).diff

prep: $(SGDKDIR)
	cd $(SGDKDIR) && patch -u -p1 -l < $(CURDIR)/patches/sgdk_$(SGDKVER).diff

$(BUILDDIR):
	mkdir -p $@

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

$(SGDKDIR)/lib/libmd.a: $(SGDKDIR) $(SGDKDIR)/mkfiles
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

install: toolsinstall $(SGDKDIR)/lib/libmd.a $(SGDKINSTALLDIR)
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

SAMPLES=$(wildcard $(SAMPLESDIR)/*/out)
SAMPLEROMS=$(addsuffix /rom.bin,$(SAMPLES))
CLEANSAMPLES=$(SAMPLES)

.PHONY: $(CLEANSAMPLES)
sample_clean: $(CLEANSAMPLES)
	@echo "Done cleaning samples"

$(CLEANSAMPLES):
	cd $@/.. && SGDKDIR=$(SGDKDIR) $(MAKE) -f $(CURDIR)/mkfiles/Makefile.rom clean

samples: $(SAMPLEROMS)
	@echo "All samples built"

$(SAMPLESDIR)/%/out/rom.bin:
	echo "ROM $@"
	cd $@/../../ && SGDKDIR=$(SGDKDIR) $(MAKE) -f $(CURDIR)/mkfiles/Makefile.rom

clean:
	echo "Clean"
	-rm -rf $(SGDKDIR)
	-rm -rf $(BUILDDIR)
