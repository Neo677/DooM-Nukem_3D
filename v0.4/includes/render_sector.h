#ifndef RENDER_SECTOR_H
# define RENDER_SECTOR_H

# include "env.h"

// Rendu recursif (Phase 3 Core)
// ytop/ybottom sont des tableaux de taille W (pixel clipping)
void render_sectors_recursive(t_env *env, int sector_id, int xmin, int xmax, int *ytop, int *ybottom, int depth);
void init_portal_renderer(t_env *env);

// Context pour le rendu de mur (Wall Loop)
typedef struct s_render_context {
    t_env       *env;
    t_sector    *sect;
    int         neighbor; // -1 if no neighbor

    // Screen X range for this wall
    int         sx1, sx2;
    int         begin_x, end_x;

    // Clipping arrays (local pointer to current arrays)
    int         *ytop;
    int         *ybottom;

    // Projected Y coordinates (current sector)
    int         sy1_ceil, sy2_ceil;
    int         sy1_floor, sy2_floor;

    // Projected Y coordinates (neighbor sector)
    int         n_sy1_ceil, n_sy2_ceil;
    int         n_sy1_floor, n_sy2_floor;

    // Texture mapping precomputed values
    double      iz1, iz2;
    double      uz1, uz2;
    t_texture   *wall_tex;
    t_texture   *upper_tex;
    t_texture   *lower_tex;

    // Recursion depth
    int         depth;
} t_render_context;

// Utils
int     clamp(int val, int min, int max);
int     transform_vertex(t_env *env, t_vertex v, double *rx, double *rz);

// Drawing
Uint32  apply_fog(Uint32 color, double dist);
void    vline(t_env *env, int x, int y1, int y2, Uint32 color, int *ytop, int *ybottom);
void    vline_textured(t_env *env, int x, int y1, int y2, int y_ceil, int y_floor, double u, t_texture *tex, int *ytop, int *ybottom, double depth);
void    draw_floor_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect);
void    draw_ceiling_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect);

// Core Recursive
void render_sectors_recursive(t_env *env, int sector_id, int xmin, int xmax, int *ytop, int *ybottom, int depth);
void init_portal_renderer(t_env *env);

#endif
