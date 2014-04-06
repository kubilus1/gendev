#ifndef _MAP
#define _MAP
#include "genesis.h"

u8 planwidth;
u8 planheight;

// winX and winY -> tile location in tilemap
u16 winX;
u16 winY;

// tX and tY -> tile position in the plane
u16 tX;
u16 tY;

// maph and mapw -> height and width of tilemap
u16 maph;
u16 mapw;

// winH and winW -> height and width of visible window in tiles
#define winH 28
#define winW 40

typedef struct t_map{
    u16 width;
    u16 height;
    u8* data;
} tilemap;

void load_map(const tilemap* inmap, int xoffset, int yoffset, u16 plan, u16 tileoffset);
void load_visible_map(const tilemap* inmap, int xoffset, int yoffset, u16 plan, u16 tileoffset);
void load_map_row(const tilemap* inmap, u16 plan, u8 row, u16 tileoffset);
void load_map_col(const tilemap* inmap, u16 plan, u8 col, u16 tileoffset);
#endif
