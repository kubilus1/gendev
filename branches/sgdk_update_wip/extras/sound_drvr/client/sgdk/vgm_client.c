//#include <sys/types.h>
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "vgm_drv.h"
#include "vgm_client.h"

#include "genesis.h"

volatile unsigned int *addr_z80 = (unsigned int *)0xA00FFF;
volatile uint8_t *init_z80 = (uint8_t *)0xA01003;
volatile uint8_t *play_z80 = (uint8_t *)0xA01004;

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

