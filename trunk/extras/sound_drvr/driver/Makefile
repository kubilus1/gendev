SDCC=sdcc
ASMZ80=sdasz80

#SDCCFLAGS=--opt-code-speed --cyclomatic -mz80 --lospre-unsafe-read --peep-file optimiza.tion --max-allocs-per-node 10000
SDCCFLAGS=--cyclomatic -mz80 --opt-code-speed --max-allocs-per-node 50000 --std-c99 --nojtbound --peep-file optimiza.tion -V
#SDCCFLAGS=-mz80 --std-c99
#--callee-saves fill_buf,fill_pcm_buf,getc,getoprnd,bank_switch,play_n_pause
#SDCC=../sdcc/bin/sdcc
#ASMZ80=../sdcc/bin/as-z80

all: vgm_drv.h

asm: vgm_drv.asm
	$(ASMZ80) -plosgffwz  "vgm_drv.o" "vgm_drv.asm"
	cp vgm_drv.o vgm_drv.rel
	cp crt0.o crt0.rel
	$(SDCC) --verbose --xstack --fomit-frame-pointer $(SDCCFLAGS) --no-std-crt0 --code-loc 0x0101 --data-loc 0xfff -V -mz80 crt0.rel vgm_drv.rel
	python timez80.py vgm_drv.lst > vgm_drv_timed.lst
	mv crt0.ihx vgm_drv.ihx
	hex2bin vgm_drv.ihx
	bin2c -o vgm_drv.h vgm_drv.bin

crt0.o: crt0.s
	$(ASMZ80) -o crt0.o crt0.s

vgm_drv.h: vgm_drv.bin
	bin2c -o vgm_drv.h vgm_drv.bin

vgm_drv.bin: vgm_drv.ihx
	hex2bin vgm_drv.ihx

vgm_drv.o: vgm_drv.c
	$(SDCC) $(SDCCFLAGS) -o vgm_drv.o -c vgm_drv.c

vgm_drv.ihx: vgm_drv.o crt0.o
	cp vgm_drv.o vgm_drv.rel
	cp crt0.o crt0.rel
	#$(SDCC) --verbose -mz80 --no-std-crt0 --code-loc 0x0101 --data-loc 0xfff -V -mz80 crt0.o vgm_drv.o
	$(SDCC) --verbose --xstack --fomit-frame-pointer $(SDCCFLAGS) --no-std-crt0 --code-loc 0x0101 --data-loc 0xfff -V -mz80 crt0.rel vgm_drv.rel
	python timez80.py vgm_drv.lst > vgm_drv_timed.lst
	#sdcc --no-std-crt0 --code-loc 0x0200 --data-loc 0xfff -V -mz80 crt0.o vgm_drv.o
	mv crt0.ihx vgm_drv.ihx

#vgm_drv.ihx: crt0.o
	#sdcc --no-std-crt0 --cyclomatic -mz80 vgm_drv.c --stack-loc 0x1fff --data-loc 0xfff 
#	sdcc -V --verbose --cyclomatic -mz80 -L crt0.o --no-std-crt0 --stack-loc 0x1fff --code-loc 0x0200 --data-loc 0xfff crt0.o vgm_drv.c

clean:
	rm -rf *.bin *.ihx *.lnk *.lst *.o *.map *.noi *.rel *.sym *.rel vgm_drv.h

