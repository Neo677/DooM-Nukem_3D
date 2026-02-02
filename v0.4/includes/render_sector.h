#ifndef RENDER_SECTOR_H
# define RENDER_SECTOR_H

# include "env.h"

// Rendu récursif (Phase 3 Core)
// ytop/ybottom sont des tableaux de taille W (pixel clipping)
void render_sectors_recursive(t_env *env, int sector_id, int xmin, int xmax, int *ytop, int *ybottom);
void init_portal_renderer(t_env *env);

#endif
