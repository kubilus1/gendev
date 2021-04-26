GENDEV?=/opt/gendev/
GENBIN=$(GENDEV)/bin
GENGCC_BIN=$(GENDEV)/bin
SGDKDIR=$(CURDIR)/SGDK
SAMPLESDIR=$(SGDKDIR)/sample

SGDKVER=v1.62
SGDKINSTALLDIR=$(GENDEV)/sgdk$(SGDKVER)

all: tools $(SGDKDIR) $(SGDKDIR)/libmd.a

tools: $(SGDKDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
	gcc -o bintos bintos.c  && cp bintos $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c  && cp sizebnd $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/xgmtool && \
		$(MAKE) -f ../../../files/Makefile.xgmtool && cp out/xgmtool $(GENDEV)/bin/.
	cp $(SGDKDIR)/bin/*.jar $(GENDEV)/bin/.

patch: $(SGDKDIR)
	cd $(SGDKDIR) && git diff . > ../files/sgdk_$(SGDKVER).diff

prep: $(SGDKDIR)
	cd $(SGDKDIR) && patch -u -p1 -l < ../files/sgdk_$(SGDKVER).diff

SGDK: $(SGDKDIR)
$(SGDKDIR):
	git clone -b '$(SGDKVER)' --single-branch --depth 1 https://github.com/Stephane-D/SGDK.git

$(SGDKDIR)/lib/libmd.a: prep
	#cp -rf ../tools/files/applib/lib $(SGDKDIR)/tools/appack
	#cd $(SGDKDIR) && ln -sf ../files/Makefile.sgdk_lib .
	#cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib
	cd $(SGDKDIR) && $(MAKE) -f makelib.gen
	

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
	cp files/makefile.vars $(GENDEV)/sgdk/mkfiles/.
	cp $(SGDKDIR)/tools/sizebnd/src/sizebnd $(GENDEV)/bin/

SAMPLES=$(wildcard $(SAMPLESDIR)/*/out)
SAMPLEROMS=$(addsuffix /rom.bin,$(SAMPLES))
CLEANSAMPLES=$(SAMPLES)

.PHONY: $(CLEANSAMPLES)
sample_clean: $(CLEANSAMPLES)
	@echo "Done cleaning samples"

$(CLEANSAMPLES):
	cd $@/.. && $(MAKE) -f $(SGDKDIR)/makefile.gen clean

samples: $(SAMPLEROMS)
	@echo "All samples built"

$(SAMPLEDIR)/%/out/rom.bin:
	echo "ROM $@"
	cd $@/../../ && $(MAKE) -f $(GENDEV)/sgdk/mkfiles/makefile.gen

clean:
	echo "Clean"
	-rm -rf $(SGDKDIR)
