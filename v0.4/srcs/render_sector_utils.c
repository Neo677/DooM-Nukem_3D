#include "render_sector.h"
#include <math.h>

#ifndef MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

// Helper: Clip value
int clamp(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// Projection simple et robuste (avec gestion Z near)
// Returns 0 if fully behind, 1 if visible
int transform_vertex(t_env *env, t_vertex v, double *rx, double *rz)
{
    double tx = v.x - env->player.pos.x;
    double ty = v.y - env->player.pos.y;
    
    double cs = cos(env->player.angle);
    double sn = sin(env->player.angle);
    
    *rz = tx * cs + ty * sn;
    *rx = tx * sn - ty * cs;
    
    // Z-Near clipping tolerance
    return (*rz > Z_NEAR_PLANE);
}
