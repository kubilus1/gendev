#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "hw_md.h"
#include "vgm_client.h"

#include "yfc2.h"
#include "ghz.h"
#include "rrrf.h"
#include "rrpc.h"

#define MAX_SONGS 4

int main(void)
{
    int song_addrs[MAX_SONGS]= {
        &ghz_vgm,
        &rrpc_vgm,
        &yfc2_vgm,
        &rrrf_vgm
    };
    int song_idx = 0;
    short buttons = 0;
    short stopstart = 0;

    clear_screen();
    put_str("Z80 VGM Music Driver", TEXT_WHITE, (40-16)/2, 2);

    // load Z80 driver
    load_driver();

    load_song(song_addrs[song_idx]);
    start_it();

    put_str("Push a button to change songs.", TEXT_GREEN, 2, 4);

    while(1)
    {
        delay(5);
        buttons = get_pad(0);
        if(buttons & (SEGA_CTRL_A | SEGA_CTRL_B | SEGA_CTRL_C))
        {
            stop_it();
            song_idx+=1;
            if(song_idx == MAX_SONGS) song_idx = 0;
            load_song(song_addrs[song_idx]);
            start_it();
        }
        else if(buttons & SEGA_CTRL_START)
        {
            if(stopstart)
            {
                stopstart = 0;
                start_it();
                put_str("         ", TEXT_WHITE, 2, 8);
            }
            else
            {
                stopstart = 1;
                stop_it();
                put_str("Paused", TEXT_WHITE, 2, 8);
            }

        }
    }
    return 0;
}

