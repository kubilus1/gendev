CC = m68k-elf-gcc
AS = m68k-elf-as
OBJC = m68k-elf-objcopy
LD = m68k-elf-ld
RM = rm -f
ASMZ80 = zasm
BINTOC = bintos

OPTION =
#INCS = -I. -I$(GENDEV)/m68k-elf/include -I$(GENDEV)/m68k-elf/m68k-elf/include
INCS = -I. -I$(GENDEV)/m68k-elf/include 
CCFLAGS = $(OPTION) -m68000 -Wall -O2 -c -fomit-frame-pointer
HWCCFLAGS = $(OPTION) -m68000 -Wall -O1 -c -fomit-frame-pointer
Z80FLAGS = -vb2
ASFLAGS = -m68000 --register-prefix-optional
LIBS = -L$(GENDEV)/m68k-elf/lib -L$(GENDEV)/m68k-elf/m68k-elf/lib -lmd -lgcc -lnosys -lc
LINKFLAGS = -T $(GENDEV)/ldscripts/sgdk.ld -Wl,-Map=output.map -nostdlib $(GENDEV)/m68k-elf/lib/libmd.a

OBJS = rom_head.o sega.o main.o 

all: out.bin 

sega.o: rom_head.bin
	$(AS) $(ASFLAGS) sega.s -o $@

%.bin: %.elf
	$(OBJC) -O binary $< temp.bin
	dd if=temp.bin of=$@ bs=8K conv=sync

%.elf: $(OBJS)
	$(CC) -o $@ $(LINKFLAGS) $(OBJS) $(LIBS)

%.o80: %.s80
	$(ASMZ80) $(Z80FLAGS) -o $@ $<

%.c: %.o80
	$(BINTOC) $<

%.o: %.c
	$(CC) $(CCFLAGS) $(INCS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

rom_head.bin: rom_head.o
	$(CC) -o $@ $(LINKFLAGS) $<

clean:
	$(RM) *.o *.bin *.elf *.map
