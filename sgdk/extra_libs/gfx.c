#include "gfx.h"

static u16 vdp_offset = 1;


u16 load_bmp(u32 *data, u16 w, u16 h) {
    u16 tmp_offset = vdp_offset;
    
    VDP_loadBMPTileData(
            data,
            vdp_offset,
            w/8,
            h/8,
            w/8
    );

    vdp_offset += ((w/8) * (h/8));
    return tmp_offset;
}
