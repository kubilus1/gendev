#include "genesis.h"
#include "ghz.h"
#include "rrpc.h"
#include "yfc2.h"
#include "rrrf.h"
#include "vgm_client.h"

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
    short startstop = 0;

    int i = 0;
    int joy_delay = 0;
    u16 state = 0;

    VDP_setScreenWidth320();
    VDP_setHInterrupt(0);
    VDP_setHilightShadow(0);
    VDP_setPaletteColor(PAL1, 15, 0x0888);
    VDP_setTextPalette(0);

    VGM_load_driver();

    VGM_load_song(song_addrs[song_idx]);
    VGM_start();

    VDP_drawText(" VGM TEST:", 5, 2);
    VDP_setTextPalette(1);
    VDP_drawText(" Press a button to change track...", 5, 10);

    while(1)
    {
        if( joy_delay > 0 ){
            joy_delay--;
            VDP_drawText("Changing track.", joy_delay, 12);
        }
        else
        {
            VDP_drawText("                                        ", 0, 12);
            state = JOY_readJoypad(0);
        }
        if(state & BUTTON_A || state & BUTTON_B || state & BUTTON_C)
        {
            VGM_stop();
            song_idx+=1;
            if(song_idx == MAX_SONGS) song_idx = 0;
            VGM_load_song(song_addrs[song_idx]);
            VGM_start();
            state = 0;
            joy_delay = 50;
        }
        else if(state & BUTTON_START)
        {   
            if(startstop)
            {
                startstop = 0;
                VGM_start();
                VDP_drawText("         ",5, 15);
            }
            else
            {
                startstop = 1;
                VGM_stop();
                VDP_drawText("PAUSED",5, 15);
            }

            // Avoid muliple hits of this button.
            VDP_waitVSync();
            VDP_waitVSync();
            VDP_waitVSync();
            VDP_waitVSync();
        }

        VDP_waitVSync();
    }
}
