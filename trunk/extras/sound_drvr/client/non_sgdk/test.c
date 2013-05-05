#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "hw_md.h"
//#include "huh.h"
#include "yfc2.h"
#include "rrrf.h"
#include "rrsn.h"
#include "rrpc.h"

#include "vgm_client.h"
//#include "drumtest.h"
#include "wtf.h"
#include "ghz.h"
#include "dt2.h"
#include "shz.h"
#include "pcmtest2.h"


int main(void)
{
    int song_addr;
    char buf[50];
    clear_screen();
    put_str("Z80 VGM Music Test", TEXT_WHITE, (40-16)/2, 2);

    // load Z80 driver
    load_driver();
    delay(1);
     
    //song_addr = &yfc2_vgm;
    song_addr = &yfc2_vgm;

    load_song(song_addr);
    //load_song(&yfc2_zgm);
    start_it();

    sprintf(buf, "Song Addr: 0x%X ", song_addr);
    put_str(buf, TEXT_GREEN, 2, 4);


    while(1)
    {
        delay(1);
    }

    while(1)
    {
        delay(10);
        z80_busrequest(Z80_BUS_REQUEST);
        uint8_t curaddr_l= *(uint8_t *)0xA01019;
        uint8_t curaddr_h = *(uint8_t *)0XA01020;
        uint8_t curinit = *(uint8_t *)0xA01003; 
        uint8_t curplay = *(uint8_t *)0xA01004;
        uint8_t curoffset_l= *(uint8_t *)0xA01021;
        uint8_t curoffset_h = *(uint8_t *)0xA01022;
        uint16_t startaddr = *(uint16_t *)0xA01007;
        uint16_t curbank = *(uint16_t *)0xA0101d; 
        z80_busrequest(Z80_BUS_RELEASE);

        sprintf(buf, "CURADDR: 0x%X %X", curaddr_h, curaddr_l);
        put_str(buf, TEXT_GREEN, 2, 5);

        sprintf(buf, "CURINIT: 0x%X ", curinit);
        put_str(buf, TEXT_GREEN, 2, 6);

        sprintf(buf, "CURPLAY: 0x%X ", curplay);
        put_str(buf, TEXT_GREEN, 2, 7);
    
        sprintf(buf, "CUROFFSET: 0x%X%X ", curoffset_h, curoffset_l);
        put_str(buf, TEXT_GREEN, 2, 8);
    
        sprintf(buf, "STARTADDR: 0x%X ", startaddr);
        put_str(buf, TEXT_GREEN, 2, 9);
    
        sprintf(buf, "CURBANK: 0x%X ", curbank);
        put_str(buf, TEXT_GREEN, 2, 10);
    
    }

    clear_screen();
    return 0;
}

