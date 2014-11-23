| SEGA MegaDrive support code
| by Chilly Willy

        .text
        .align  2

| Initialize the hardware & load font tiles
        .global init_hardware
init_hardware:
        movem.l d2-d7/a2-a6,-(sp)

| init joyports
        lea     0xA10000,a5
        move.b  #0x40,0x09(a5)
        move.b  #0x40,0x0B(a5)
        move.b  #0x40,0x03(a5)
        move.b  #0x40,0x05(a5)

        lea     0xC00000,a3             /* VDP data reg */
        lea     0xC00004,a4             /* VDP cmd/sts reg */

| wait on VDP DMA (in case we reset in the middle of DMA)
        move.w  #0x8114,(a4)            /* display off, dma enabled */
0:
        move.w  (a4),d0                 /* read VDP status */
        btst    #1,d0                   /* DMA busy? */
        bne.b   0b                      /* yes */

        moveq   #0,d0
        move.w  #0x8000,d5              /* set VDP register 0 */
        move.w  #0x0100,d7

| Set VDP registers
        lea     InitVDPRegs(pc),a5
        moveq   #18,d1
1:
        move.b  (a5)+,d5                /* lower byte = register data */
        move.w  d5,(a4)                 /* set VDP register */
        add.w   d7,d5                   /* + 0x0100 = next register */
        dbra    d1,1b

| clear VRAM
        move.w  #0x8F02,(a4)            /* set INC to 2 */
        move.l  #0x40000000,(a4)        /* write VRAM address 0 */
        move.w  #0x7FFF,d1              /* 32K - 1 words */
2:
        move.w  d0,(a3)                 /* clear VRAM */
        dbra    d1,2b

| The VDP state at this point is: Display disabled, ints disabled, Name Tbl A at 0xC000,
| Name Tbl B at 0xE000, Name Tbl W at 0xB000, Sprite Attr Tbl at 0xA800, HScroll Tbl at 0xAC00,
| H40 V28 mode, and Scroll size is 64x32.

| Clear CRAM
        lea     InitVDPRAM(pc),a5
        move.l  (a5)+,(a4)              /* set reg 1 and reg 15 */
        move.l  (a5)+,(a4)              /* write CRAM address 0 */
        moveq   #31,d3
3:
        move.l  d0,(a3)
        dbra    d3,3b

| Clear VSRAM
        move.l  (a5)+,(a4)              /* write VSRAM address 0 */
        moveq   #19,d4
4:
        move.l  d0,(a3)
        dbra    d4,4b

| halt Z80 and init FM chip
        /* Allow the 68k to access the FM chip */
        move.w  #0x100,0xA11100
        move.w  #0x100,0xA11200

| reset YM2612
        lea     FMReset(pc),a5
        lea     0xA00000,a0
        move.w  #0x4000,d1
        moveq   #26,d2
5:
        move.b  (a5)+,d1                /* FM reg */
        move.b  (a5)+,0(a0,d1.w)        /* FM data */
        nop
        nop
        dbra    d2,5b

        moveq   #0x30,d0
        moveq   #0x5F,d2
6:
        move.b  d0,0x4000(a0)           /* FM reg */
        nop
        nop
        move.b  #0xFF,0x4001(a0)        /* FM data */
        nop
        nop
        move.b  d0,0x4002(a0)           /* FM reg */
        nop
        nop
        move.b  #0xFF,0x4003(a0)        /* FM data */
        nop
        nop
        addq.b  #1,d0
        dbra    d2,6b

| reset PSG
        lea     PSGReset(pc),a5
        lea     0xC00000,a0
        move.b  (a5)+,0x0011(a0)
        move.b  (a5)+,0x0011(a0)
        move.b  (a5)+,0x0011(a0)
        move.b  (a5),0x0011(a0)

| load font tile data
        move.w  #0x8F02,(a4)            /* INC = 2 */
        move.l  #0x40000000,(a4)        /* write VRAM address 0 */
        lea     font_data,a0
        move.w  #0x6B*8-1,d2
7:
        move.l  (a0)+,d0                /* font fg mask */
        move.l  d0,d1
        not.l   d1                      /* font bg mask */
        andi.l  #0x11111111,d0          /* set font fg color */
        andi.l  #0x00000000,d1          /* set font bg color */
        or.l    d1,d0
        move.l  d0,(a3)                 /* set tile line */
        dbra    d2,7b

| set the default palette for text
        move.l  #0xC0000000,(a4)        /* write CRAM address 0 */
        move.l  #0x00000CCC,(a3)        /* entry 0 (black) and 1 (lt gray) BGR */
        move.l  #0xC0200000,(a4)        /* write CRAM address 32 */
        move.l  #0x000000A0,(a3)        /* entry 16 (black) BGR and 17 (green) */
        move.l  #0xC0400000,(a4)        /* write CRAM address 64 */
        move.l  #0x0000000A,(a3)        /* entry 32 (black) BGR and 33 (red) */

        move.w  #0x8174,(a4)            /* display on, vblank enabled */
        move    #0x2000,sr              /* enable interrupts */

        movem.l (sp)+,d2-d7/a2-a6
        rts

| VDP register initialization values
InitVDPRegs:
        .byte   0x04    /* 8004 => write reg 0 = /IE1 (no HBL INT), /M3 (enable read H/V cnt) */
        .byte   0x14    /* 8114 => write reg 1 = /DISP (display off), /IE0 (no VBL INT), M1 (DMA enabled), /M2 (V28 mode) */
        .byte   0x30    /* 8230 => write reg 2 = Name Tbl A = 0xC000 */
        .byte   0x2C    /* 832C => write reg 3 = Name Tbl W = 0xB000 */
        .byte   0x07    /* 8407 => write reg 4 = Name Tbl B = 0xE000 */
        .byte   0x54    /* 8554 => write reg 5 = Sprite Attr Tbl = 0xA800 */
        .byte   0x00    /* 8600 => write reg 6 = always 0 */
        .byte   0x00    /* 8700 => write reg 7 = BG color */
        .byte   0x00    /* 8800 => write reg 8 = always 0 */
        .byte   0x00    /* 8900 => write reg 9 = always 0 */
        .byte   0x00    /* 8A00 => write reg 10 = HINT = 0 */
        .byte   0x00    /* 8B00 => write reg 11 = /IE2 (no EXT INT), full scroll */
        .byte   0x81    /* 8C81 => write reg 12 = H40 mode, no lace, no shadow/hilite */
        .byte   0x2B    /* 8D2B => write reg 13 = HScroll Tbl = 0xAC00 */
        .byte   0x00    /* 8E00 => write reg 14 = always 0 */
        .byte   0x01    /* 8F01 => write reg 15 = data INC = 1 */
        .byte   0x01    /* 9001 => write reg 16 = Scroll Size = 64x32 */
        .byte   0x00    /* 9100 => write reg 17 = W Pos H = left */
        .byte   0x00    /* 9200 => write reg 18 = W Pos V = top */

        .align  2

| VDP Commands
InitVDPRAM:
        .word   0x8104, 0x8F01  /* set registers 1 (display off) and 15 (INC = 1) */
        .word   0xC000, 0x0000  /* write CRAM address 0 */
        .word   0x4000, 0x0010  /* write VSRAM address 0 */

FMReset:
        /* disable LFO */
        .byte   0,0x22
        .byte   1,0x00
        /* disable timer & set channel 6 to normal mode */
        .byte   0,0x27
        .byte   1,0x00
        /* all KEY_OFF */
        .byte   0,0x28
        .byte   1,0x00
        .byte   1,0x04
        .byte   1,0x01
        .byte   1,0x05
        .byte   1,0x02
        .byte   1,0x06
        /* disable DAC */
        .byte   0,0x2A
        .byte   1,0x80
        .byte   0,0x2B
        .byte   1,0x00
        /* turn off channels */
        .byte   0,0xB4
        .byte   1,0x00
        .byte   0,0xB5
        .byte   1,0x00
        .byte   0,0xB6
        .byte   1,0x00
        .byte   2,0xB4
        .byte   3,0x00
        .byte   2,0xB5
        .byte   3,0x00
        .byte   2,0xB6
        .byte   3,0x00

| PSG register initialization values
PSGReset:
        .byte   0x9f    /* set ch0 attenuation to max */
        .byte   0xbf    /* set ch1 attenuation to max */
        .byte   0xdf    /* set ch2 attenuation to max */
        .byte   0xff    /* set ch3 attenuation to max */

        .align  2

| short set_sr(short new_sr);
| set SR, return previous SR
| entry: arg = SR value
| exit:  d0 = previous SR value
        .global set_sr
set_sr:
        moveq   #0,d0
        move.w  sr,d0
        move.l  4(sp),d1
        move.w  d1,sr
        rts

| short get_pad(short pad);
| return buttons for selected pad
| entry: arg = pad index (0 or 1)
| exit:  d0 = pad value (0 0 0 1 M X Y Z S A C B R L D U) or (0 0 0 0 0 0 0 0 S A C B R L D U)
        .global get_pad
get_pad:
        move.l  d2,-(sp)
        move.l  8(sp),d0        /* first arg is pad number */
        cmpi.w  #1,d0
        bhi     no_pad
        add.w   d0,d0
        addi.l  #0xA10003,d0    /* pad control register */
        movea.l d0,a0
        bsr.b   get_input       /* - 0 s a 0 0 d u - 1 c b r l d u */
        move.w  d0,d1
        andi.w  #0x0C00,d0
        bne.b   no_pad
        bsr.b   get_input       /* - 0 s a 0 0 d u - 1 c b r l d u */
        bsr.b   get_input       /* - 0 s a 0 0 0 0 - 1 c b m x y z */
        move.w  d0,d2
        bsr.b   get_input       /* - 0 s a 1 1 1 1 - 1 c b r l d u */
        andi.w  #0x0F00,d0      /* 0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 */
        cmpi.w  #0x0F00,d0
        beq.b   common          /* six button pad */
        move.w  #0x010F,d2      /* three button pad */
common:
        lsl.b   #4,d2           /* - 0 s a 0 0 0 0 m x y z 0 0 0 0 */
        lsl.w   #4,d2           /* 0 0 0 0 m x y z 0 0 0 0 0 0 0 0 */
        andi.w  #0x303F,d1      /* 0 0 s a 0 0 0 0 0 0 c b r l d u */
        move.b  d1,d2           /* 0 0 0 0 m x y z 0 0 c b r l d u */
        lsr.w   #6,d1           /* 0 0 0 0 0 0 0 0 s a 0 0 0 0 0 0 */
        or.w    d1,d2           /* 0 0 0 0 m x y z s a c b r l d u */
        eori.w  #0x1FFF,d2      /* 0 0 0 1 M X Y Z S A C B R L D U */
        move.w  d2,d0
        move.l  (sp)+,d2
        rts

| 3-button/6-button pad not found
no_pad:
        .ifdef  HAS_SMS_PAD
        move.b  (a0),d0         /* - 1 c b r l d u */
        andi.w  #0x003F,d0      /* 0 0 0 0 0 0 0 0 0 0 c b r l d u */
        eori.w  #0x003F,d0      /* 0 0 0 0 0 0 0 0 0 0 C B R L D U */
        .else
        move.w  #0xF000,d0      /* SEGA_CTRL_NONE */
        .endif
        move.l  (sp)+,d2
        rts

| read single phase from controller
get_input:
        move.b  #0x00,(a0)
        nop
        nop
        move.b  (a0),d0
        move.b  #0x40,(a0)
        lsl.w   #8,d0
        move.b  (a0),d0
        rts

| void clear_screen(void);
| clear the name table for plane B
        .global clear_screen
clear_screen:
        moveq   #0,d0
        lea     0xC00000,a0
        move.w  #0x8F02,4(a0)           /* set INC to 2 */
        move.l  #0x60000003,d1          /* VDP write VRAM at 0xE000 (scroll plane B) */
        move.l  d1,4(a0)                /* write VRAM at plane B start */
        move.w  #64*32-1,d1
1:
        move.w  d0,(a0)                 /* clear name pattern */
        dbra    d1,1b
        rts

| void put_str(char *str, int color, int x, int y);
| put string characters to the screen
| entry: first arg = string address
|        second arg = 0 for normal color font, N * 0x0200 for alternate color font (use CP bits for different colors)
|        third arg = column at which to start printing
|        fourth arg = row at which to start printing
        .global put_str
put_str:
        movea.l 4(sp),a0                /* string pointer */
        move.l  8(sp),d0                /* color palette */
        lea     0xC00000,a1
        move.w  #0x8F02,4(a1)           /* set INC to 2 */
        move.l  16(sp),d1               /* y coord */
        lsl.l   #6,d1
        or.l    12(sp),d1               /* cursor y<<6 | x */
        add.w   d1,d1                   /* pattern names are words */
        swap    d1
        ori.l   #0x60000003,d1          /* OR cursor with VDP write VRAM at 0xE000 (scroll plane B) */
        move.l  d1,4(a1)                /* write VRAM at location of cursor in plane B */
1:
        move.b  (a0)+,d0
        subi.b  #0x20,d0                /* font starts at space */
        move.w  d0,(a1)                 /* set pattern name for character */
        tst.b   (a0)
        bne.b   1b
        rts

| void put_chr(char chr, int color, int x, int y);
| put a character to the screen
| entry: first arg = character
|        second arg = 0 for normal color font, N * 0x0200 for alternate color font (use CP bits for different colors)
|        third arg = column at which to start printing
|        fourth arg = row at which to start printing
        .global put_chr
put_chr:
        movea.l 4(sp),a0                /* character */
        move.l  8(sp),d0                /* color palette */
        lea     0xC00000,a1
        move.w  #0x8F02,4(a1)           /* set INC to 2 */
        move.l  16(sp),d1               /* y coord */
        lsl.l   #6,d1
        or.l    12(sp),d1               /* cursor y<<6 | x */
        add.w   d1,d1                   /* pattern names are words */
        swap    d1
        ori.l   #0x60000003,d1          /* OR cursor with VDP write VRAM at 0xE000 (scroll plane B) */
        move.l  d1,4(a1)                /* write VRAM at location of cursor in plane B */

        move.l  a0,d1
        move.b  d1,d0
        subi.b  #0x20,d0                /* font starts at space */
        move.w  d0,(a1)                 /* set pattern name for character */
        rts

| void delay(int count);
| wait count number of vertical blank periods - relies on vblank running
| entry: arg = count
        .global delay
delay:
        move.l  4(sp),d0                /* count */
        add.l   gTicks,d0               /* add current vblank count */
0:
        cmp.l   gTicks,d0
        bgt.b   0b
        rts

| void set_palette(short *pal, int start, int count)
| copy count entries pointed to by pal into the palette starting at the index start
| entry: pal = pointer to an array of words holding the colors
|        start = index of the first color in the palette to set
|        count = number of colors to copy
        .global set_palette
set_palette:
        movea.l 4(sp),a0                /* pal */
        move.l  8(sp),d0                /* start */
        move.l  12(sp),d1               /* count */
        add.w   d0,d0                   /* start*2 */
        swap    d0                      /* high word holds address */
        ori.l   #0xC0000000,d0          /* write CRAM address (0 + index*2) */
        subq.w  #1,d1                   /* for dbra */

        lea     0xC00000,a1
        move.w  #0x8F02,4(a1)           /* set INC to 2 */
        move.l  d0,4(a1)                /* write CRAM */
0:
        move.w  (a0)+,(a1)              /* copy color to palette */
        dbra    d1,0b
        rts

| void z80_busrequest(int flag)
| set Z80 bus request
| entry: flag = request/release
        .global z80_busrequest
z80_busrequest:
        move.l  4(sp),d0                /* flag */
        andi.w  #0x0100,d0
        move.w  d0,0xA11100             /* set bus request */
0:
        move.w  0xA11100,d1
        and.w   d0,d1
        bne.b   0b
        rts

| void z80_reset(int flag)
| set Z80 reset
| entry: flag = assert/clear
        .global z80_reset
z80_reset:
        move.l  4(sp),d0                /* flag */
        andi.w  #0x0100,d0
        move.w  d0,0xA11200             /* set reset */
        rts

| void z80_memclr(void *dst, int len)
| clear Z80 sram
| entry: dst = pointer to sram to clear
|        len = length of memory to copy
| note: you need to request the Z80 bus first
        .global z80_memclr
z80_memclr:
        movea.l 4(sp),a1                /* dst */
        move.l  8(sp),d0                /* len */
        subq.w  #1,d0
        moveq   #0,d1
0:
        move.b  d1,(a1)+
        dbra    d0,0b
        rts

| void z80_memcpy(void *dst, void *src, int len)
| copy memory to Z80 sram
| entry: dst = pointer to memory to copy to
|        src = pointer to memory to copy from
|        len = length of memory to copy
| note: you need to request the Z80 bus first
        .global z80_memcpy
z80_memcpy:
        movea.l 4(sp),a1                /* dst */
        movea.l 8(sp),a0                /* src */
        move.l  12(sp),d0               /* len */
        subq.w  #1,d0
0:
        move.b  (a0)+,d1
        move.b  d1,(a1)+
        dbra    d0,0b
        rts

