GENDEV?=/opt/gendev/
GENBIN=$(GENDEV)/bin
GENGCC_BIN=$(GENDEV)/bin
SGDKDIR=$(CURDIR)/SGDK
SAMPLESDIR=$(SGDKDIR)/sample


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
	#svn diff $(SGDKDIR) > files/sgdk.diff

prep:
	cd $(SGDKDIR) && patch -u -p1 < ../files/sgdk.diff

$(SGDKDIR):
	git clone https://github.com/Stephane-D/SGDK.git
	cd $(SGDKDIR) && patch -u -p1 < ../files/sgdk.diff
	cp files/makefile.vars $(SGDKDIR)/.
	#git clone https://github.com/kubilus1/SGDK.git $(SGDKDIR)
	#svn co http://sgdk.googlecode.com/svn/trunk $(SGDKDIR)
	#patch -u -p0 < files/sgdk.diff

$(SGDKDIR)/libmd.a: $(SGDKDIR)
	cd $(SGDKDIR) && ln -sf ../files/Makefile.sgdk_lib .
	cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib 

install: tools $(SGDKDIR)/libmd.a
	echo "Install"
	cd $(SGDKDIR) && $(MAKE) -f Makefile.sgdk_lib install
	install -b $(SGDKDIR)/md.ld $(GENDEV)/ldscripts/sgdk.ld
	cp -r ./skeleton $(GENDEV)/.
	cp $(SGDKDIR)/libmd.a $(GENDEV)/lib/.
	#install -b files/sgdk.ld $(GENDEV)/ldscripts/sgdk.ld

SAMPLES=$(wildcard $(SAMPLESDIR)/*/out)
SAMPLEROMS=$(addsuffix /rom.bin,$(SAMPLES))

sample_clean:
	rm $(SAMPLEROMS)

samples: $(SAMPLEROMS)
	@echo "All samples built"

$(SAMPLEDIR)/%/out/rom.bin:
	echo "ROM $@"
	cd $@/../../ && GDK=$(SGDKDIR) make -f ../../makefile.gen

clean:
	echo "Clean"
	-rm -rf $(SGDKDIR)
