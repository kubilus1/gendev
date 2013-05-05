#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hw_md.h"
#include "vgm_drv.h"

#include "vgm_client.h"

volatile unsigned int *addr_z80 = (unsigned int *)0xA00FFF;
volatile uint8_t *init_z80 = (uint8_t *)0xA01003;
volatile uint8_t *play_z80 = (uint8_t *)0xA01004;

void load_driver(void)
{
    // hold z80 bus - halt z80
    z80_busrequest(Z80_BUS_REQUEST);
    // clear z80 sram
    z80_memclr((void *)0xA00000, 0x2000);
    // copy driver to z80 sram
    z80_memcpy((void *)0xA00000, &vgm_drv_bin, (int)vgm_drv_bin_size);
    // reset z80
    z80_reset(Z80_ASSERT_RESET);
    // release z80 bus
    z80_busrequest(Z80_BUS_RELEASE);
    // finish z80 reset - start driver
    z80_reset(Z80_CLEAR_RESET);
    delay(2);
}


void load_song(unsigned int addr)
{
    char *p = (char *)addr_z80;
    char *q = (char *)&addr;

    // First load the songs address
    z80_busrequest(Z80_BUS_REQUEST);
    p[0] = q[3];
    p[1] = q[2];
    p[2] = q[1];
    p[3] = q[0];
    z80_busrequest(Z80_BUS_RELEASE);
    
    // Pause and initialize
    z80_busrequest(Z80_BUS_REQUEST);
    *init_z80 = 0x01;
    *play_z80 = 0x00;
    z80_busrequest(Z80_BUS_RELEASE);

}

void start_it(void)
{    
    z80_busrequest(Z80_BUS_REQUEST);
    *play_z80 = 0x01;
    z80_busrequest(Z80_BUS_RELEASE);
}

void stop_it(void)
{
    z80_busrequest(Z80_BUS_REQUEST);
    *play_z80 = 0x00;
    z80_busrequest(Z80_BUS_RELEASE);
    delay(2);
}

