#ifndef _HW_MD_H
#define _HW_MD_H

#define SEGA_CTRL_BUTTONS   0x0FFF
#define SEGA_CTRL_UP        0x0001
#define SEGA_CTRL_DOWN      0x0002
#define SEGA_CTRL_LEFT      0x0004
#define SEGA_CTRL_RIGHT     0x0008
#define SEGA_CTRL_B         0x0010
#define SEGA_CTRL_C         0x0020
#define SEGA_CTRL_A         0x0040
#define SEGA_CTRL_START     0x0080
#define SEGA_CTRL_Z         0x0100
#define SEGA_CTRL_Y         0x0200
#define SEGA_CTRL_X         0x0400
#define SEGA_CTRL_MODE      0x0800

#define SEGA_CTRL_TYPE      0xF000
#define SEGA_CTRL_THREE     0x0000
#define SEGA_CTRL_SIX       0x1000
#define SEGA_CTRL_NONE      0xF000

/* default text colors */
#define TEXT_WHITE          0x0000
#define TEXT_GREEN          0x2000
#define TEXT_RED            0x4000

/* Z80 control flags */
#define Z80_BUS_REQUEST     0x0100
#define Z80_BUS_RELEASE     0x0000
#define Z80_ASSERT_RESET    0x0000
#define Z80_CLEAR_RESET     0x0100

#ifdef __cplusplus
extern "C" {
#endif

extern short set_sr(short new_sr);
extern short get_pad(short pad);
extern void clear_screen(void);
extern void put_str(char *str, int color, int x, int y);
extern void put_chr(char chr, int color, int x, int y);
extern void delay(int count);

extern void z80_busrequest(int flag);
extern void z80_reset(int flag);
extern void z80_memclr(void *dst, int len);
extern void z80_memcpy(void *dst, void *src, int len);

#ifdef __cplusplus
}
#endif

#endif
