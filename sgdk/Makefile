GENDEV?=/opt/gendev/
GENBIN=$(GENDEV)/bin
GENGCC_BIN=$(GENDEV)/bin
BUILDDIR=$(CURDIR)/../build
SGDKDIR?=$(BUILDDIR)/sgdk
SAMPLESDIR=$(SGDKDIR)/sample

SGDKVER=v1.62
SGDKINSTALLDIR=$(GENDEV)/sgdk$(SGDKVER)

all: tools $(SGDKDIR) $(SGDKDIR)/lib/libmd.a

tools: $(SGDKDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
	gcc -o bintos bintos.c  && cp bintos $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c  && cp sizebnd $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/xgmtool && \
		$(MAKE) -f $(CURDIR)/mkfiles/Makefile.xgmtool && cp out/xgmtool $(GENDEV)/bin/.
	cp $(SGDKDIR)/bin/*.jar $(GENDEV)/bin/.

patch: $(SGDKDIR)
	cd $(SGDKDIR) && git diff . > $(CURDIR)/patches/sgdk_$(SGDKVER).diff

prep: $(SGDKDIR)
	cd $(SGDKDIR) && patch -u -p1 -l < $(CURDIR)/patches/sgdk_$(SGDKVER).diff

SGDK: $(SGDKDIR)
$(SGDKDIR):
	cd $(BUILDDIR) && git clone -b '$(SGDKVER)' --single-branch --depth 1 https://github.com/Stephane-D/SGDK.git sgdk
	cd $(SGDKDIR) && patch -u -p1 -l < $(CURDIR)/patches/sgdk_$(SGDKVER).diff


$(SGDKDIR)/mkfiles: $(SGDKDIR)
	mkdir -p $(SGDKDIR)/mkfiles
	cp -f $(CURDIR)/mkfiles/makefile.vars $@/.
	cp -f $(CURDIR)/mkfiles/Makefile.sgdk_lib $@/.
	cp -f $(CURDIR)/mkfiles/Makefile.rom $@/.

$(SGDKDIR)/lib/libmd.a: $(SGDKDIR) $(SGDKDIR)/mkfiles
	#cd $(SGDKDIR) && GENDEV=$(BUILDDIR) $(MAKE) -f makelib.gen
	cd $(SGDKDIR) && SGDKDIR=$(SGDKDIR) $(MAKE) -f $(CURDIR)/mkfiles/Makefile.sgdk_lib
	@echo "Done building sgdk library"	

$(SGDKINSTALLDIR):
	mkdir -p $@
	rm -f $(GENDEV)/sgdk
	cd $(GENDEV) && ln -sf sgdk$(SGDKVER) sgdk

install: tools $(SGDKDIR)/lib/libmd.a $(SGDKINSTALLDIR)
	echo "Install"
	mkdir -p $(GENDEV)/sgdk/src
	mkdir -p $(GENDEV)/sgdk/inc
	mkdir -p $(GENDEV)/sgdk/lib
	mkdir -p $(GENDEV)/sgdk/res
	mkdir -p $(GENDEV)/sgdk/mkfiles
	cp $(SGDKDIR)/md.ld $(GENDEV)/sgdk/.
	cp $(SGDKDIR)/inc/* $(GENDEV)/sgdk/inc/.
	cp -r $(SGDKDIR)/src/boot $(GENDEV)/sgdk/src/.
	cp -r $(SGDKDIR)/res/* $(GENDEV)/sgdk/res/.
	cp $(GENDEV)/lib/libgcc.a $(GENDEV)/sgdk/lib/.
	cp $(SGDKDIR)/lib/libmd.a $(GENDEV)/sgdk/lib/.
	cp $(SGDKDIR)/makefile.gen $(GENDEV)/sgdk/mkfiles/.
	cp mkfiles/makefile.vars $(GENDEV)/sgdk/mkfiles/.
	cp $(SGDKDIR)/tools/sizebnd/src/sizebnd $(GENDEV)/bin/

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
