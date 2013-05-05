all: vgm_drv.h

crt0.o: crt0.s
	as-z80 -o crt0.o crt0.s

vgm_drv.h: vgm_drv.bin
	bin2c -o vgm_drv.h vgm_drv.bin

vgm_drv.bin: vgm_drv.ihx
	hex2bin vgm_drv.ihx

vgm_drv.o: vgm_drv.c
	sdcc -mz80 -o vgm_drv.o -c vgm_drv.c

vgm_drv.ihx: vgm_drv.o crt0.o
	sdcc --verbose -mz80 --no-std-crt0 --code-loc 0x0101 --data-loc 0xfff -V -mz80 crt0.o vgm_drv.o
	#sdcc --no-std-crt0 --code-loc 0x0200 --data-loc 0xfff -V -mz80 crt0.o vgm_drv.o
	mv crt0.ihx vgm_drv.ihx

#vgm_drv.ihx: crt0.o
	#sdcc --no-std-crt0 --cyclomatic -mz80 vgm_drv.c --stack-loc 0x1fff --data-loc 0xfff 
#	sdcc -V --verbose --cyclomatic -mz80 -L crt0.o --no-std-crt0 --stack-loc 0x1fff --code-loc 0x0200 --data-loc 0xfff crt0.o vgm_drv.c

clean:
	rm -rf *.asm *.bin *.ihx *.lnk *.lst *.o *.map *.noi *.rel *.sym *.rel vgm_drv.h

