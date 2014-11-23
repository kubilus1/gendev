#include "genesis.h"

#ifndef _SPRITE
#define _SPRITE

typedef struct
{
    s16 posx;
    s16 posy;
    u16 tile_attr;
    u8 size;
    u8 link;
    u16 startaddr;
    u8 steps;
    u8 curstep;
    u8 tilesize;
    u8 pal;
    u8 prev_link;
} spritedef;


spritedef _sprite_all[MAX_SPRITE];

u8 _sprite_init;

void animate_sprite(spritedef * sprt);

int add_sprite(spritedef sprite);
int drop_sprite(int index);
void hscroll_sprites(s8 hscroll); 
void vscroll_sprites(s8 vscroll);

#endif
