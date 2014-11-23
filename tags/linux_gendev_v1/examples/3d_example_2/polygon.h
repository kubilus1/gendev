#include "genesis.h"

#ifndef _POLYGON_H_
#define _POLYGON_H_

typedef struct t_polygon {
    int num_verts;
    int num_faces;
    int num_edges;
    Vect3D_f16 centroid;
    Vect3D_f16* coords;
    u16* poly_ind;
    u16* line_ind;
    Vect3D_f16* face_norm;
    fix16 distance;
} polygon;

#endif
