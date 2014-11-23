#include "genesis.h"
#include "sprite.h"

char abuf[50];
u8 _sprite_init = 0;

void animate_sprite(spritedef *sprt)
{
    sprt->tile_attr = TILE_ATTR_FULL(
        sprt->pal,
        1,0,0,
        sprt->startaddr+(sprt->tilesize*(sprt->curstep % sprt->steps)));
    sprt->curstep++;
}

int add_sprite(spritedef sprite)
{
    // Somehow detect if the sprite_list is new.  Put something into position 0.

    if(_sprite_init == 0)
    {
        _sprite_init = 1;
        _sprite_all[0] = sprite;
        _sprite_all[0].link = 0;
        VDP_setSpriteP(0, &sprite);
        return 0;
    }

    //VDP_drawText("Searching ...", 0, 2);
    // Search the list
    int cur_idx = 0;
    int last_idx = 0;
    int next_idx = 0;

    while(1)
    {
        // If the next link is 0, we are at the end
        next_idx = _sprite_all[cur_idx].link;
        last_idx = cur_idx;
        if(next_idx == 0)
        {
            if(cur_idx == (MAX_SPRITE - 1))
            {
                //Check if sprite list is full
                //VDP_drawText("FULL!", 10, 3);
                return -1;
            }
            else
            {
                //Otherwise the next is free
                last_idx = cur_idx;
                cur_idx += 1;
                
                //sprintf(abuf, "CIDX: %d", cur_idx);
                //VDP_drawText(abuf, 0, 3);
                break;
            }
        }

        if((cur_idx + 1) != next_idx)
        {
            //We have a gap, use it
            last_idx = cur_idx;
            cur_idx += 1;
            //sprintf(abuf, "GAP CIDX:(%d), NIDX:(%d)", cur_idx, next_idx);
            //VDP_drawText(abuf, 0, 4);
            break;
        }
        cur_idx = next_idx;
    }

    // Setup new sprite
    _sprite_all[cur_idx] = sprite;
    _sprite_all[cur_idx].link = next_idx;

    // Update link of last_idx
    _sprite_all[last_idx].link = cur_idx;

    //Tell the VDP about the updates.
    VDP_setSpriteP(
        cur_idx, 
        &_sprite_all[cur_idx]
    );
    VDP_setSpriteP(
        last_idx, 
        &_sprite_all[last_idx]
    );

    return cur_idx;
}

int drop_sprite(int del_idx)
{
    int last_idx=0;
    int idx = 0;
    //sprintf(abuf, "DROP: %d", del_idx);
    //VDP_drawText(abuf, 20, 0);
    while(1)
    {
        //VDP_drawText("deleting...", 20, 1);
        // If the next link is 0, we are at the end
        last_idx = idx;
        idx = _sprite_all[idx].link;
        if(idx == 0)
            break;
        
        if(idx == del_idx)
        {
            //Found the item to delete
            _sprite_all[last_idx].link = _sprite_all[idx].link;
            break;
        }

    }
    //VDP_drawText("done...", 20, 1);
    //Tell the VDP about the updates.
    VDP_setSpriteP(
        last_idx, 
        &_sprite_all[last_idx]
    );
   
    return 0;
}

void hscroll_sprites(s8 hscroll) 
{
    int cur_idx = 0;
    int next_idx = 0;

    while(1)
    {
        // If the next link is 0, we are at the end
        next_idx = _sprite_all[cur_idx].link;
        _sprite_all[cur_idx].posx += hscroll;
        if(next_idx == 0)
        {
            break;
        }
        cur_idx = next_idx;
    }
}

void vscroll_sprites(s8 vscroll) 
{
    int cur_idx = 0;
    int next_idx = 0;

    while(1)
    {
        // If the next link is 0, we are at the end
        next_idx = _sprite_all[cur_idx].link;
        _sprite_all[cur_idx].posy += vscroll;
        if(next_idx == 0)
        {
            break;
        }
        cur_idx = next_idx;
    }
}
