SUDO?= sudo
MGET?= wget
MAKE?= make
ORIG_USER:=$(shell whoami)

TOPDIR=$(CURDIR)/..
BUILDDIR?=$(TOPDIR)/build
WORKDIR?=$(CURDIR)/work

TOOLSDIR=$(BUILDDIR)/bin

UNAME:=$(shell uname)

TOOLS=$(TOOLSDIR)/bin2c
TOOLS+=$(TOOLSDIR)/sjasm
ifeq ($(UNAME), Linux)
TOOLS+=$(TOOLSDIR)/zasm
#TOOLS+=$(TOOLSDIR)/vgm_cmp
endif
TOOLS+=$(TOOLSDIR)/sixpack 
TOOLS+=$(TOOLSDIR)/appack

tools_build: $(WORKDIR) $(TOOLSDIR) $(TOOLS)
	-cp extras/scripts/*.* $(TOOLSDIR)/.
	echo "Done with tools."

tools_clean:
	-rm -rf $(WORKDIR)
	-rm $(TOOLSDIR)/bin2c
	-rm $(TOOLSDIR)/sjasm
	-rm $(TOOLSDIR)/zasm
	-rm $(TOOLSDIR)/hexbin
	-rm $(TOOLSDIR)/sixpack
	-rm $(TOOLSDIR)/vgmtools

$(WORKDIR):
	mkdir -p work

#########################################################
#########################################################
#########################################################

BIN2C_PKG=$(WORKDIR)/bin2c-1.0.zip
$(WORKDIR)/bin2c-1.0.zip:
	#cd $(WORKDIR) && $(MGET) http://downloads.sourceforge.net/project/bin2c/1.0/bin2c-1.0.zip
	cp files/`basename $@` $@

SJASM_PKG=$(WORKDIR)/sjasm39g6.zip
$(WORKDIR)/sjasm39g6.zip:
	#cd $(WORKDIR) && $(MGET) http://home.online.nl/smastijn/sjasm39g6.zip
	cp files/`basename $@` $@

ZASM_PKG=$(WORKDIR)/zasm-3.0.21-source-linux-2011-06-19.zip
$(WORKDIR)/zasm-3.0.21-source-linux-2011-06-19.zip:
	#cd $(WORKDIR) && $(MGET) http://k1.spdns.de/Develop/Projects/zasm/Distributions/old%20versions/zasm-3.0.21-source-linux-2011-06-19.zip 
	cp files/`basename $@` $@

HEXBIN_PKG=$(WORKDIR)/Hex2bin-1.0.10.tar.bz2
$(WORKDIR)/Hex2bin-1.0.10.tar.bz2:
	#cd $(WORKDIR) && $(MGET) http://downloads.sourceforge.net/project/hex2bin/hex2bin/$@
	cp files/`basename $@` $@

#$(WORKDIR)/genres_01.zip: 
#	cd $(WORKDIR) && $(MGET) http://gendev.spritesmind.net/files/genres_01.zip

SIXPACK_PKG=$(WORKDIR)/sixpack-13.zip
$(WORKDIR)/sixpack-13.zip:
	#cd $(WORKDIR) && $(MGET) http://jiggawatt.org/badc0de/sixpack/sixpack-13.zip
	cp files/`basename $@` $@

VGMTOOL_PKG=$(WORKDIR)/VGMTools_src.rar
$(WORKDIR)/VGMTools_src.rar:
	#$(MGET) -O $@ http://www.smspower.org/forums/download.php?id=3201
	cp files/`basename $@` $@

#########################################################
#########################################################
#########################################################

#########################################################
#########################################################
#########################################################

$(TOOLSDIR):
	[ -d $@ ] || mkdir $@

$(TOOLSDIR)/bin2c: $(BIN2C_PKG)
	cd $(WORKDIR) && \
	unzip bin2c-1.0.zip && \
	cd bin2c && \
	gcc bin2c.c -o bin2c && \
	cp bin2c $@ 

$(TOOLSDIR)/sjasm: $(SJASM_PKG)
	- mkdir -p $(WORKDIR)/sjasm
	cd $(WORKDIR)/sjasm && \
	unzip ../sjasm39g6.zip && \
	cd sjasmsrc39g6 && \
	$(MAKE) && \
	cp sjasm $@ && \
	chmod +x $@

$(TOOLSDIR)/zasm: $(ZASM_PKG)
	- mkdir -p $(WORKDIR)/zasm 
	cd $(WORKDIR)/zasm && \
	unzip ../zasm-3.0.21-source-linux-2011-06-19.zip && \
	cd zasm-3.0.21-i386-ubuntu-linux-2011-06-19/source && \
	$(MAKE) && \
	cp zasm $@

$(TOOLSDIR)/hex2bin: $(HEXBIN_PKG)
	cd $(WORKDIR) && \
	tar xjf $< && \
	cp Hex2bin-1.0.10/hex2bin $@

$(TOOLSDIR)/sixpack: $(SIXPACK_PKG)
	- mkdir -p $(WORKDIR)/sixpack && \
	cd $(WORKDIR)/sixpack && \
	unzip ../sixpack-13.zip 
	cp $(WORKDIR)/sixpack/sixpack-12/bin/sixpack $@ 
	chmod +x $@	

#genres $(TOOLSDIR)/genres: genres_01.zip
#	- mkdir -p $(WORKDIR)/genres && \
#	cd $(WORKDIR)/genres && \
#	unzip ../../$< 
	

$(TOOLSDIR)/vgm_cmp: $(VGMTOOL_PKG)
	- mkdir -p $(WORKDIR)/vgmtools
	cd $(WORKDIR)/vgmtools && \
	unrar x ../../$< 
	cd $(WORKDIR)/vgmtools && \
	patch -u < ../../files/vgm_cmp.diff && \
	gcc -c chip_cmp.c -o chip_cmp.o && \
	gcc chip_cmp.o vgm_cmp.c -lz -o vgm_cmp && \
	cp vgm_cmp $@

$(TOOLSDIR)/appack: 
	- mkdir -p $(WORKDIR)/applib 
	cp -r files/applib/* $(WORKDIR)/applib/. 
	cd $(WORKDIR)/applib/example && \
	$(MAKE) -f makefile.elf && \
	cp appack $@
