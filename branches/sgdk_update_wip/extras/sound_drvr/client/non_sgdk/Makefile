ifdef $(GENDEV)
ROOTDIR = $(GENDEV)
else
ROOTDIR = /opt/toolchains/gen
endif

LDSCRIPTSDIR = $(ROOTDIR)/ldscripts

LIBPATH = -L$(ROOTDIR)/m68k-elf/lib -L$(ROOTDIR)/m68k-elf/lib/gcc/m68k-elf/4.6.2 -L$(ROOTDIR)/m68k-elf/m68k-elf/lib
INCPATH = -I. -I../include -I../../driver -I../../ -I$(ROOTDIR)/m68k-elf/include -I$(ROOTDIR)/m68k-elf/m68k-elf/include

#MDCCFLAGS = -m68000 -Wall -Ofast -c -fomit-frame-pointer
MDCCFLAGS = -m68000 -Wall -O1 -c -fomit-frame-pointer
MDHWFLAGS = -m68000 -Wall -O1 -c -fomit-frame-pointer
MDLDFLAGS = -T $(LDSCRIPTSDIR)/md.ld -Wl,-Map=output.map -nostdlib
MDASFLAGS = -m68000 --register-prefix-optional

MDPREFIX = $(ROOTDIR)/m68k-elf/bin/m68k-elf-
MDCC = $(MDPREFIX)gcc
MDAS = $(MDPREFIX)as
MDLD = $(MDPREFIX)ld
MDOBJC = $(MDPREFIX)objcopy

ASMZ80 = zasm
BINTOC = $(ROOTDIR)/bin/bin2c
FLAGSZ80 = -vb2

DD = dd
RM = rm -f

TARGET = z80vgm
LIBS = $(LIBPATH) -lc -lgcc -lnosys
OBJS = crt0.o vgm_client.o hw_md.o font.o main.o

all: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	$(MDOBJC) -O binary $< temp.bin
	# Not sure why we need to always make this 3MB, for real HW?
	#$(DD) if=temp.bin of=$@ bs=3072K conv=sync
	cp temp.bin $@

$(TARGET).elf: $(OBJS) $(FILES) 
	$(MDCC) $(MDLDFLAGS) $(OBJS) $(LIBS) $(FILES) -o $(TARGET).elf

%.o80: %.s80
	$(ASMZ80) $(FLAGSZ80) -o $@ $<

%.c: %.o80
	$(BINTOC) $<

main.o: main.c
	$(MDCC) $(MDHWFLAGS) $(INCPATH) $< -o $@

%.o: %.c
	$(MDCC) $(MDCCFLAGS) $(INCPATH) $< -o $@

%.o: %.s
	$(MDAS) $(MDASFLAGS) $(INCPATH) $< -o $@

clean:
	$(RM) *.o *.o80 *.bin *.elf *.map *.log
