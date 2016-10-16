#include "genesis.h"

#include "meshs.h"

#define MAX_POINTS 256 

#define DEBUG 0

Vect3D_f16 pts_3D[MAX_POINTS];
Vect2D_s16 pts_2D[MAX_POINTS];

#define NUM_STARS 64

Vect3D_f16 stars_3D[MAX_POINTS];
Vect3D_f16 stars_t3D[MAX_POINTS];
Vect2D_u16 stars_t2D[MAX_POINTS];

extern Mat3D_f16 MatInv;
extern Mat3D_f16 Mat;

//Vect3D_f16 vtab_3D[MAX_POINTS];
//Vect2D_s16 vtab_2D[MAX_POINTS];

Rotation3D rotation;
Translation3D translation;
Transformation3D transformation;

Vect3D_f16 rotstep;

Vect3D_f16 camvec = {FIX16(0),FIX16(0),FIX16(12)}; 

u16 flatDrawing;

u8 pal_offset;

char str[16];
void updatePointsPos(polymesh inpoly);
void drawPoints(u8 col, polymesh inpoly);
void doActionJoy(u8 numjoy, u16 value);
void handleJoyEvent(u16 joy, u16 changed, u16 state);

u8 light_mask[4];

fix16 get_distance(Vect3D_f16 v1, Vect3D_f16 v2) {
#if 0
        fix16 xd,yd,zd;

        xd = v2.x - v1.x;
        yd = v2.y - v1.y;
        zd = v2.z - v1.z;
        return (xd*xd + yd*yd + zd*zd);
#endif
        //fix16 distance_2 = fix16Sqrt((xd*xd + yd*yd + zd*zd));
        //fix16 distance_2 = fix16Sqrt(xd*xd+yd*yd+zd*zd);

    // Taxicab distance for now    
#if 1
    return (abs(v2.x-v1.x) + abs(v2.y-v1.y) + abs(v2.z-v1.z));
#endif
}

u16 Partition_polymesh(polymesh **data, u16 p, u16 r)      
{                                                   
    polymesh* x = data[p];                               
    u16 i = p - 1;                                  
    u16 j = r + 1;                                  
                                                    
    while (TRUE)                                    
    {                                               
        i++;                                        
        while ((i < r) && (data[i]->distance < x->distance)) i++;       
        j--;                                       
        while ((j > p) && (data[j]->distance > x->distance)) j--;      
                                                    
        if (i < j)                                  
        {                                           
            polymesh* tmp;                               
                                                    
            tmp = data[i];                          
            data[i] = data[j];                      
            data[j] = tmp;                         
        }                                           
        else                                        
            return j;                               
    }                                               
}                                                   
                                                    
void sort_polymeshs(polymesh** data, u16 p, u16 r) {
    if (p < r)                                      
    {                                               
        u16 q = Partition_polymesh(data, p, r);       
        sort_polymeshs(data, p, q);                   
        sort_polymeshs(data, q + 1, r);               
    }                                               
}

void render(polymesh** poly_list, u8 list_len, Transformation3D* transform) {
    Vect3D_f16 v1;
#if DEBUG    
    Vect2D_s16 cpts_2D[10];
#endif
    u8 i;

    // Calculate distance
    for(i=0;i<list_len;i++) {
        M3D_transform(transform, &(*poly_list[i]).centroid, &v1, 1);
#if DEBUG
        M3D_project_s16(&v1, &(cpts_2D[i]), 1);
#endif
        //tp.distance = get_distance(v1, camvec);
        (*poly_list[i]).distance = get_distance(v1, camvec);
    }

    M3D_transform(transform, stars_3D, stars_t3D, NUM_STARS);
    M3D_project_s16(stars_t3D, stars_t2D, NUM_STARS);  

    BMP_setPixels_V2D(stars_t2D, 15, NUM_STARS);
#if DEBUG
    fix16ToStr(v1.x, str, 2);
    BMP_drawText(str, 0, 4);
    fix16ToStr(v1.y, str, 2);
    BMP_drawText(str, 0, 5);
    fix16ToStr(v1.z, str, 2);
    BMP_drawText(str, 0, 6);
    fix16ToStr(poly_list[1]->distance, str, 2);
    BMP_drawText(str, 0, 7);
#endif

    // Sort
    sort_polymeshs(poly_list, 0, list_len-1);

    // Render
    for(i=list_len;i>0;i--) {
        drawPoints(0xFF, *poly_list[i-1]);
    }



#if DEBUG
    Line m;
    m.col = 4;
    for(i=list_len;i>0;i--) {
        m.pt1 = cpts_2D[i];
        m.pt2 = cpts_2D[i];

        m.pt2.x+=20;
        m.pt2.y+=20;
        m.pt1.x-=20;
        m.pt1.y-=20;

        BMP_drawLine(&m);
        
        m.pt1 = cpts_2D[i];
        m.pt2 = cpts_2D[i];

        m.pt2.x+=20;
        m.pt2.y-=20;
        m.pt1.x-=20;
        m.pt1.y+=20;

        BMP_drawLine(&m);
    }
#endif

}

int main()
{

    VDP_setScreenWidth256();
    VDP_setHInterrupt(0);
    VDP_setHilightShadow(0);

    // speed up controller checking
    JOY_setSupport(PORT_1, JOY_SUPPORT_6BTN);
    JOY_setSupport(PORT_2, JOY_SUPPORT_OFF);

    JOY_setEventHandler(handleJoyEvent);

    //BMP_init(TRUE, PAL1, FALSE);
    BMP_init(FALSE, PAL0, FALSE);

    //BGR ?

    //VDP_setPaletteColor(7,RGB24_TO_VDPCOLOR(0x770000));
    //VDP_setPaletteColor(8,RGB24_TO_VDPCOLOR(0xAA0000));
    //VDP_setPaletteColor(9,RGB24_TO_VDPCOLOR(0xFF0000));
    //VDP_setPaletteColor(4,RGB24_TO_VDPCOLOR(0xCC0000));
    //VDP_setPaletteColor(5,RGB24_TO_VDPCOLOR(0xFF0000));

    //VDP_setPaletteColor(7,RGB24_TO_VDPCOLOR(0x880000));
    //VDP_setPaletteColor(8,RGB24_TO_VDPCOLOR(0xFF0000));
    //VDP_setPaletteColor(9,RGB24_TO_VDPCOLOR(0x0000FF));

    pal_offset = 3;

    VDP_setPaletteColors(pal_offset, polymesh_palette, polymesh_pal_len);

    M3D_reset();
    M3D_setCamDistance(camvec.z);
    M3D_setLightEnabled(1);
    M3D_setLightXYZ(FIX16(0.9), FIX16(0.9), FIX16(-0.9));

    // allocate translation and rotation structure
    M3D_setTransform(&transformation, &translation, &rotation);
    M3D_setTranslation(&transformation, FIX16(0), FIX16(0), FIX16(20));
    M3D_setRotation(&transformation, FIX16(0), FIX16(0), FIX16(0));

    flatDrawing = 1;
    rotstep.y = FIX16(0.2);
    
    light_mask[0] = 0x00;
    light_mask[1] = 0x10; 
    light_mask[2] = 0x20;
 
    int i;
    for(i=0;i<NUM_STARS;i++) {
        stars_3D[i].x=fix16Div(random(), FIX16(1));
        stars_3D[i].y=fix16Div(random(), FIX16(1));
        stars_3D[i].z=fix16Div(random(), FIX16(1));
    }

    while (1)
    {
        doActionJoy(JOY_1, JOY_readJoypad(JOY_1));

        M3D_setCamDistance(camvec.z);

        // do work here
        rotation.x += rotstep.x;
        rotation.y += rotstep.y;
        rotation.z += rotstep.z;
        transformation.rebuildMat = 1;

        // ensure previous flip buffer request has been started
        BMP_waitWhileFlipRequestPending();
        BMP_showFPS(1);

        BMP_clear();

        render(polymesh_list, polymesh_list_len, &transformation);

#if DEBUG
        BMP_drawText("trans z:", 0, 2);
        fix16ToStr(translation.z, str, 2);
        BMP_drawText(str, 10, 2);
        BMP_drawText("cam dist:", 0, 3);
        //fix16ToStr(camvec.z, str, 2);
        //BMP_drawText(str, 11, 3);
#endif
        BMP_flip(1);
    }
}

void updatePointsPos(polymesh inpoly)
{
    // transform 3D point
    M3D_transform(&transformation, inpoly.coords, pts_3D, inpoly.num_verts);
    // project 3D point (f16) to 2D point (s16)
    M3D_project_s16(pts_3D, pts_2D, inpoly.num_verts);
}

void drawPoints(u8 col, polymesh inpoly)
{
    updatePointsPos(inpoly);

    if (flatDrawing)
    {
        Vect2D_s16 v[4];
        char str[10];
        const Vect3D_f16 *norm;
        const u16 *poly_ind;
        u16 i,j;
        u16* base_col;
        u16 num_verts;
        u8 light = 0;

        norm = inpoly.face_norm;
        poly_ind = inpoly.poly_ind;
        base_col = inpoly.face_color;


        // # of polymesh faces
        i = inpoly.num_faces;

        while (i--)
        {
            Vect2D_s16 *pt_dst = v;
            fix16 dp;

            // Cycle through number of indices in polymesh face
            num_verts = *poly_ind++;

            for(j=0;j<num_verts;j++) {
                *pt_dst++ = pts_2D[*poly_ind++];
            }
            *pt_dst--;
            
            dp = fix16Mul(transformation.lightInv.x, norm->x) +
                 fix16Mul(transformation.lightInv.y, norm->y) +
                 fix16Mul(transformation.lightInv.z, norm->z);

            col = *base_col + pal_offset;
            
            if (dp > 0) {
                light += (dp >> (FIX16_FRAC_BITS - 1));
                if (light >= 3) {
                    col = (col << 4) | col;
                } else {
                    col = light_mask[light] | col;
                }
            } else {                
                col = light_mask[0] | col;
            }

            if (!BMP_isPolygonCulled(v, num_verts)){
                BMP_drawPolygon(v, num_verts, col);
#if DEBUG
                uintToStr(light, str, 2);
                BMP_drawText(str, 10, i);
                uintToStr(col, str, 2);
                BMP_drawText(str, 20, i);
#endif
            } else {
#if DEBUG
                BMP_drawText("CULLED", 10, i);            
#endif
            }
            base_col++;
            norm++;
        }
    }
    else
    {
        Line l;
        const u16 *line_ind;
        u16 i;

        l.col = col;
        line_ind = inpoly.line_ind;
        
        // Number of lines
        i = inpoly.num_edges;

        while (i--)
        {
            l.pt1 = pts_2D[*line_ind++];
            l.pt2 = pts_2D[*line_ind++];

            BMP_drawLine(&l);
        }
    }
}


void doActionJoy(u8 numjoy, u16 value)
{
    if (numjoy == JOY_1)
    {
        if (value & BUTTON_UP)
        {
            if (value & BUTTON_A) translation.y += FIX16(0.2);
            else rotstep.x += FIX16(0.05);
        }

        if (value & BUTTON_DOWN)
        {
            if (value & BUTTON_A) translation.y -= FIX16(0.2);
            else rotstep.x -= FIX16(0.05);
        }

        if (value & BUTTON_LEFT)
        {
            if (value & BUTTON_A) translation.x -= FIX16(0.2);
            else rotstep.y += FIX16(0.05);
        }

        if (value & BUTTON_RIGHT)
        {
            if (value & BUTTON_A) translation.x += FIX16(0.2);
            else rotstep.y -= FIX16(0.05);
        }

        if (value & BUTTON_Y)
        {
            if (value & BUTTON_X) camvec.z += FIX16(1.0);
            else camvec.z += FIX16(0.1);
        }

        if (value & BUTTON_Z)
        {
            if (value & BUTTON_X) camvec.z -= FIX16(1.0);
            else camvec.z -= FIX16(0.1);
        }

        if (value & BUTTON_B)
        {
            if (value & BUTTON_A) translation.z += FIX16(1);
            else translation.z += FIX16(0.1);
        }

        if (value & BUTTON_C)
        {
            if (value & BUTTON_A) translation.z -= FIX16(1);
            else translation.z -= FIX16(0.1);
        }

        if (value & BUTTON_START)
        {
            rotstep.x = FIX16(0.0);
            rotstep.y = FIX16(0.0);
        }
    }
}

void handleJoyEvent(u16 joy, u16 changed, u16 state)
{
    if (joy == JOY_1)
    {
        if (changed & ~state & BUTTON_START)
        {
            flatDrawing = 1 - flatDrawing;
        }
    }
}
