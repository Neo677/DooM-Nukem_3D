#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

// Vecteur 2D
typedef struct _vec2 {
    double x;
    double y;
} vec2_t;

// Structure pour les quads de rendu
typedef struct _rquad {
    int ax, bx;    // X coordinates of points A & B
    int at, ab;    // A top & A bottom coordinates
    int bt, bb;    // B top & B bottom coordinates
} rquad_t;

// Structure de mur
typedef struct _wall {
    vec2_t a;
    vec2_t b;
    double portal_top_height;
    double portal_bot_height;
    bool is_portal;
    int neighbor_sector_id;
} wall_t;

// Structure de lookup table pour les plans
typedef struct _r_plane {
    int t[1024];
    int b[1024];
} plane_lut_t;

// Structure de secteur
typedef struct _sector {
    int id;
    wall_t walls[64];
    int num_walls;
    int height;
    int elevation;
    double dist;
    unsigned int color;
    unsigned int floor_clr;
    unsigned int ceil_clr;
    plane_lut_t portals_floorx_ylut;
    plane_lut_t portals_ceilx_ylut;
    plane_lut_t floorx_ylut;
    plane_lut_t ceilx_ylut;
} sector_t;

// Structure pour la queue de rendu (portails)
typedef struct _render_item {
    int sector_id;
    int min_x;
    int max_x;
} render_item_t;

typedef struct _render_queue {
    render_item_t items[1024];
    int head;
    int tail;
} render_queue_t;

// Structure de stockage des secteurs (Map)
typedef struct _sectors_store {
    sector_t sectors[1024];
    int num_sectors;
} sectors_store_t;

typedef struct {
    sector_t *sectors;
    int num_sectors;
    vec2_t spawn_point;
    double spawn_angle;
} map_t;

#endif /* TYPES_H */