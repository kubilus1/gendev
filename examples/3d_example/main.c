#include "genesis.h"

#include "meshs.h"

#define MAX_POINTS 512 

Vect3D_f16 pts_3D[MAX_POINTS];
Vect2D_s16 pts_2D[MAX_POINTS];


extern Mat3D_f16 MatInv;
extern Mat3D_f16 Mat;


Vect3D_f16 vtab_3D[MAX_POINTS];
Vect2D_s16 vtab_2D[MAX_POINTS];

Rotation3D rotation;
Translation3D translation;
Transformation3D transformation;

Vect3D_f16 rotstep;

Vect3D_f16 camvec = {FIX16(0),FIX16(0),FIX16(12)}; 

u16 flatDrawing;


char str[16];
void updatePointsPos(polygon inpoly);
void drawPoints(u8 col, polygon inpoly);
void doActionJoy(u8 numjoy, u16 value);
void handleJoyEvent(u16 joy, u16 changed, u16 state);


int get_distance(Vect3D_f16 v1, Vect3D_f16 v2) {
        //xd = v2.x - 0;
        //yd = v2.y - 0;
        //zd = v2.z - camdist;
        //fix16 distance_2 = fix16Sqrt((xd*xd + yd*yd + zd*zd));
        //fix16 distance_2 = fix16Sqrt(xd*xd+yd*yd+zd*zd);

    // Taxicab distance for now    
    return (abs(v2.x-v1.x) + abs(v2.y-v1.y) + abs(v2.z-v1.z));
}

u16 Partition_polygon(polygon **data, u16 p, u16 r)      
{                                                   
    polygon* x = data[p];                               
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
            polygon* tmp;                               
                                                    
            tmp = data[i];                          
            data[i] = data[j];                      
            data[j] = tmp;                         
        }                                           
        else                                        
            return j;                               
    }                                               
}                                                   
                                                    
void sort_polygons(polygon** data, u16 p, u16 r) {
    if (p < r)                                      
    {                                               
        u16 q = Partition_polygon(data, p, r);       
        sort_polygons(data, p, q);                   
        sort_polygons(data, q + 1, r);               
    }                                               
}

void render(polygon** poly_list, u8 list_len, Transformation3D* transform) {
    Vect3D_f16 v1;
    u8 i;

    // Calculate distance
    for(i=0;i<list_len;i++) {
        M3D_transform(transform, &(*poly_list[i]).centroid, &v1, 1);
        //tp.distance = get_distance(v1, camvec);
        (*poly_list[i]).distance = get_distance(v1, camvec);
    }

    // Sort
    sort_polygons(poly_list, 0, list_len-1);

    // Render
    for(i=list_len;i>0;i--) {
        drawPoints(0xFF, *poly_list[i-1]);
    }
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

    BMP_init(TRUE, PAL1, FALSE);
    //BMP_init(FALSE, PAL1, FALSE);

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

        render(polygon_list, polygon_list_len, &transformation);

        BMP_drawText("trans z:", 0, 2);
        fix16ToStr(translation.z, str, 2);
        BMP_drawText(str, 10, 2);
        BMP_drawText("cam dist:", 0, 3);
        //fix16ToStr(camvec.z, str, 2);
        //BMP_drawText(str, 11, 3);

        BMP_flip(1);
    }
}

void updatePointsPos(polygon inpoly)
{
    // transform 3D point
    M3D_transform(&transformation, inpoly.coords, pts_3D, inpoly.num_verts);
    // project 3D point (f16) to 2D point (s16)
    M3D_project_s16(pts_3D, pts_2D, inpoly.num_verts);
}

void drawPoints(u8 col, polygon inpoly)
{
    updatePointsPos(inpoly);

    if (flatDrawing)
    {
        Vect2D_s16 v[4];
        char str[10];
        const Vect3D_f16 *norm;
        const u16 *poly_ind;
        u16 i,j;
        u16 num_verts;

        norm = inpoly.face_norm;
        poly_ind = inpoly.poly_ind;

        // # of polygon faces
        i = inpoly.num_faces;

        while (i--)
        {
            Vect2D_s16 *pt_dst = v;
            fix16 dp;
            u8 col = 2;

            // Cycle through number of indices in polygon face
            num_verts = *poly_ind++;

            for(j=0;j<num_verts;j++) {
                *pt_dst++ = pts_2D[*poly_ind++];
            }
            *pt_dst--;
            
            dp = fix16Mul(transformation.lightInv.x, norm->x) +
                 fix16Mul(transformation.lightInv.y, norm->y) +
                 fix16Mul(transformation.lightInv.z, norm->z);
            norm++;

            if (dp > 0) col += (dp >> (FIX16_FRAC_BITS - 2));

            if (!BMP_isPolygonCulled(v, num_verts))
                BMP_drawPolygon(v, num_verts, col);
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
