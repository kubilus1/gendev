#include "tilemap.h"

void load_map(const tilemap* inmap, int xoffset, int yoffset, u16 plan, u16 tileoffset) {
    int i;
    int j;

    //int start_index = 1;
    int maph = inmap->height;
    int mapw = inmap->width;

    int tilex;
    int tiley;
    
    for(i=0;i<maph;i++) {
        if(i+yoffset >= planheight) {
            tiley = (i + yoffset) - planheight;
        } else {
            tiley = i + yoffset;
        }

        for(j=0;j<mapw;j++) {
            if(j+xoffset >= planwidth) {
                tilex = (j + xoffset) - planwidth;

            } else {
                tilex = j + xoffset;

            }
            VDP_setTileMap(
                    plan, 
                    (u16*)inmap->data[(i * mapw) + j] + tileoffset - 1,
                    tilex,
                    tiley
            );
        }
    }
}

void load_visible_map(const tilemap* inmap, int xoffset, int yoffset, u16 plan, u16 tileoffset) {
    u16 i;
    u16 j;
    u16 iW;
    u16 jW;
    u8 ycount;
    u8 xcount;

    mapw = inmap->width;
    maph = inmap->height;

    i = tY;
    iW = winY;
    for(ycount=0;ycount<winH;ycount++) {

        // wrap around
        if(i == planheight) {
            i -= planheight;
        } 
        if(iW >= maph) {
            iW -= maph;
        }

        j = tX;
        jW = winX;
        for(xcount=0;xcount<winW;xcount++){
            if(j == planwidth) {
                j -= planwidth;
            } 
            if(jW == mapw) {
                jW = 0;
            }
            VDP_setTileMap(
                    plan, 
                    (u16*)inmap->data[(iW * mapw) + jW] + tileoffset - 1,
                    j,
                    i
            );
            j++;
            jW++;
        }
        i++;
        iW++;
    }

}

void load_map_row(const tilemap* inmap, u16 plan, u8 row, u16 tileoffset) {
    u8 i;
    u8 j;
    u16 iW;
    u16 jW;
    u8 xcount;

    i = tY + row;
    iW = winY + row;

    // wrap around
    if(i >= planheight) {
        i -= planheight;
    } 
    if(iW >= maph) {
        iW -= maph;
    }

    j = tX;
    jW = winX;
    for(xcount=0;xcount<winW;xcount++){
        if(j == planwidth) {
            j = 0;
        } 
        if(jW == mapw) {
            jW = 0;
        }
        VDP_setTileMap(
                plan, 
                (u16*)inmap->data[(iW * mapw) + jW] + tileoffset - 1,
                j,
                i
        );
        j++;
        jW++;
    }
}

void load_map_col(const tilemap* inmap, u16 plan, u8 col, u16 tileoffset) {
    u8 i;
    u8 j;
    u16 iW;
    u16 jW;
    u8 ycount;

    i = tX + col;
    iW = winX + col;

    // wrap around
    if(i >= planwidth) {
        i -= planwidth;
    } 
    if(iW >= mapw) {
        iW -= mapw;
    }

    j = tY;
    jW = winY;
    for(ycount=0;ycount<winH;ycount++){
        if(j == planheight) {
            j = 0;
        } 
        if(jW == maph) {
            jW = 0;
        }
        VDP_setTileMap(
                plan, 
                (u16*)inmap->data[(jW * mapw) + iW] + tileoffset - 1,
                i,
                j
        );
        jW++;
        j++;
    }
}
