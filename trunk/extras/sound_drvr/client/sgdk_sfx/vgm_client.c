//#include <sys/types.h>
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "vgm_drv.h"
#include "vgm_client.h"

#include "genesis.h"

typedef union Address_t {
    struct {
        u8 l;
        u8 h;
    };
    u16 w;
} Address;

volatile unsigned int *addr_z80 = (unsigned int *)0xA00FFF;
volatile uint8_t *init_z80 = (uint8_t *)0xA01003;
volatile uint8_t *play_z80 = (uint8_t *)0xA01004;
volatile u8 *sfx_play = (u8 *)0xA01025;
volatile u16 *addr_sfx = (u16 *)0xA01027;
volatile u8 *bank_sfx = (u8 *)0xA01026;
volatile u16 *len_sfx = (u16 *)0xA01029;

void VGM_load_driver(void)
{
    Z80_init();
    Z80_requestBus(1);
    Z80_upload(0, &vgm_drv_bin, (u16)vgm_drv_bin_size, 1);
    Z80_releaseBus();

    //Is this reliable?  
    waitTick(1); 
}


void VGM_load_song(unsigned int addr)
{
    char *p = (char *)addr_z80;
    char *q = (char *)&addr;

    // Reset the sound chip
    YM2612_reset();
    PSG_init();

    // Load the songs address
    Z80_requestBus(1);
    p[0] = q[3];
    p[1] = q[2];
    p[2] = q[1];
    p[3] = q[0];
    Z80_releaseBus();
    
    // Pause and initialize
    Z80_requestBus(1);
    *init_z80 = 0x01;
    *play_z80 = 0x00;
    Z80_releaseBus();

}

void VGM_play_sfx(unsigned int addr, u16 len) 
{

    u16 z80addr;
    u8 z80bank;
    z80bank = ((addr & 0xFF8000) >> 15);
    z80addr = ((addr & ~0xFF8000) + 0x8000);

    char *p = (char *)addr_sfx;
    char *q = (char *)&z80addr;
    char *r = (char *)len_sfx;
    char *s = (char *)&len;
    char *t = (char *)bank_sfx;
    char *u = (char *)&z80bank;

    Z80_requestBus(1);
    p[0] = q[1];
    p[1] = q[0];
    r[0] = s[1];
    r[1] = s[0];
    t[0] = u[0];
    *sfx_play = 0x01;
    Z80_releaseBus();
}

void VGM_start(void)
{    
    Z80_requestBus(1);
    *play_z80 = 0x01;
    Z80_releaseBus();
}

void VGM_stop(void)
{
    Z80_requestBus(1);
    *play_z80 = 0x00;
    Z80_releaseBus();
}

void VGM_debug(void) {
    char buf[50];

    Z80_requestBus(1);
    
    Address curaddr= *(Address *)0xA01019;
    uint8_t curinit = *(uint8_t *)0xA01003; 
    uint8_t curplay = *(uint8_t *)0xA01004;
    Address curoffset= *(Address *)0xA01021;
    uint8_t vgmptr = *(uint8_t *)0xA011A3;
    Address startaddr = *(Address *)0xA01015;
    u8 addrbank = *(u8 *)0xA01013;
    u8 bank_p = *(u8 *)0xA01022;
    uint8_t opcode = *(uint8_t *)0xA010B0;
    Address pcm_addr = *(Address *)0xA01020;
    Address pcm_len = *(Address *)0xA0101E;
    Address debug = *(Address *)0xA010B1;
    u8 vgm_bank = *(u8 *)0xA013C5;
    u8 bank_8x = *(u8 *)0xA0102B;
    u8 bank_bkp = *(u8 *)0xA013C4;

    Z80_releaseBus();

    sprintf(buf, "CURADDR: 0x%X %X  ", curaddr.h, curaddr.l);
    VDP_drawText(buf, 2, 0);

    sprintf(buf, "VGMPTR: 0x%X  ", vgmptr);
    VDP_drawText(buf, 2, 1);

    sprintf(buf, "CURINIT: 0x%X  ", curinit);
    VDP_drawText(buf, 2, 2);

    sprintf(buf, "CURPLAY: 0x%X  ", curplay);
    VDP_drawText(buf, 2, 3);

    sprintf(buf, "CUROFFSET: 0x%X %X  ", curoffset.h, curoffset.l);
    VDP_drawText(buf, 2, 4);

    sprintf(buf, "STARTADDR: 0x%X %X ", startaddr.h, startaddr.l);
    VDP_drawText(buf, 2, 5);

    sprintf(buf, "ADDRBANK: 0x%X  ", addrbank);
    VDP_drawText(buf, 2, 6);

    sprintf(buf, "BANK_P: 0x%X   ", bank_p);
    VDP_drawText(buf, 2, 7);

    sprintf(buf, "PCMADDR: 0x%X %X   ", pcm_addr.h, pcm_addr.l);
    VDP_drawText(buf, 2, 8);

    sprintf(buf, "PCMLEN: 0x%X %X   ", pcm_len.h, pcm_len.l);
    VDP_drawText(buf, 2, 9);
    
    sprintf(buf, "VGMBANK: 0x%X   ", vgm_bank);
    VDP_drawText(buf, 2, 10);
    sprintf(buf, "BANK_8x: 0x%X   ", bank_8x);
    VDP_drawText(buf, 2, 11);
    sprintf(buf, "BANK_BKP: 0x%X   ", bank_bkp);
    VDP_drawText(buf, 2, 12);
    
    
    
    sprintf(buf, "DEBUG: 0x%X %X   ", debug.h, debug.l);
    VDP_drawText(buf, 2, 13);
    

}
