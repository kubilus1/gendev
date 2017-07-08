GENDEV?=/opt/gendev/
GENBIN=$(GENDEV)/bin
GENGCC_BIN=$(GENDEV)/bin
SGDKDIR=$(CURDIR)/SGDK
SAMPLESDIR=$(SGDKDIR)/sample

SGDKVER=1_3
SGDKINSTALLDIR=$(GENDEV)/sgdk$(SGDKVER)


all: tools $(SGDKDIR) $(SGDKDIR)/libmd.a 

tools: $(SGDKDIR)
	cd $(SGDKDIR)/tools/bintos/src && \
	gcc -o bintos bintos.c  && cp bintos $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/pcmtoraw/src && \
	gcc -o pcmtoraw pcmtoraw.c  && cp pcmtoraw $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/sizebnd/src && \
	gcc -o sizebnd sizebnd.c  && cp sizebnd $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/wavtoraw/src && \
	gcc -o wavtoraw wavtoraw.c -lm && cp wavtoraw $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/xgmtool && \
		make -f ../../../files/Makefile.xgmtool && cp out/xgmtool $(GENDEV)/bin/.
	cd $(SGDKDIR)/tools/rescomp && \
		make -f ../../../files/Makefile.rescomp && cp out/rescomp $(GENDEV)/bin/.
		#patch -u -p0 < ../../../files/rescomp.diff && \
		#
	cp $(SGDKDIR)/bin/lz4w.jar $(GENDEV)/bin/.

patch:
	cd $(SGDKDIR) && git diff * > ../files/sgdk.diff

prep:
	cd $(SGDKDIR) && patch -u -p1 < ../files/sgdk.diff

$(SGDKDIR):
	git clone https://github.com/Stephane-D/SGDK.git
	cd $(SGDKDIR) && patch -u -p1 < ../files/sgdk.diff
	#cp files/makefile.vars $(SGDKDIR)/.
	#git clone https://github.com/kubilus1/SGDK.git $(SGDKDIR)
	#patch -u -p0 < files/sgdk.diff

$(SGDKDIR)/libmd.a: $(SGDKDIR)
	cd $(SGDKDIR) && ln -sf ../files/Makefile.sgdk_lib .
	cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib 

$(SGDKINSTALLDIR):
	mkdir -p $@
	ln -sf $(SGDKINSTALLDIR) $(GENDEV)/sgdk

install: tools $(SGDKDIR)/libmd.a $(SGDKINSTALLDIR)
	echo "Install"
	mkdir -p $(GENDEV)/sgdk/ldscripts
	mkdir -p $(GENDEV)/sgdk/src
	mkdir -p $(GENDEV)/sgdk/inc
	mkdir -p $(GENDEV)/sgdk/lib
	mkdir -p $(GENDEV)/sgdk/res
	mkdir -p $(GENDEV)/sgdk/mkfiles
	#cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib install
	cp $(SGDKDIR)/md.ld $(GENDEV)/sgdk/ldscripts/.
	#cp -r ./skeleton $(GENDEV)/.
	cp $(SGDKDIR)/inc/* $(GENDEV)/sgdk/inc/.
	cp -r $(SGDKDIR)/src/boot $(GENDEV)/sgdk/src/.
	cp -r $(SGDKDIR)/res/* $(GENDEV)/sgdk/res/.
	cp $(GENDEV)/lib/libgcc.a $(GENDEV)/sgdk/lib/.
	cp $(SGDKDIR)/libmd.a $(GENDEV)/sgdk/lib/.
	cp $(SGDKDIR)/makefile.gen $(GENDEV)/sgdk/mkfiles/.
	cp files/makefile.vars $(GENDEV)/sgdk/mkfiles/.

SAMPLES=$(wildcard $(SAMPLESDIR)/*/out)
SAMPLEROMS=$(addsuffix /rom.bin,$(SAMPLES))
CLEANSAMPLES=$(SAMPLES)

.PHONY: $(CLEANSAMPLES)
sample_clean: $(CLEANSAMPLES)
	@echo "Done cleaning samples"

$(CLEANSAMPLES):
	cd $@/.. && make -f $(GENDEV)/sgdk/mkfiles/makefile.gen clean

samples: $(SAMPLEROMS)
	@echo "All samples built"

$(SAMPLEDIR)/%/out/rom.bin:
	echo "ROM $@"
	cd $@/../../ && make -f $(GENDEV)/sgdk/mkfiles/makefile.gen

clean:
	echo "Clean"
	-rm -rf $(SGDKDIR)
