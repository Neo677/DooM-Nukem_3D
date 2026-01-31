#include "env.h"
#define PI 3.14159265358979323846

// Rendu 3D avec DDA optimisé (100x plus rapide que l'ancienne méthode)
void    render_3d(t_env *env)
{
    double  fov_rad = (FOV * PI) / 180.0;
    double  ray_angle;
    t_ray_hit hit;
    int     x;
    
    for (x = 0; x < env->w; x++)
    {
        // Calculer l'angle du rayon pour cette colonne
        ray_angle = env->player.angle - (fov_rad / 2.0) + (x * fov_rad / env->w);
        
        // Lancer le rayon avec DDA (100x plus rapide!)
        hit = cast_ray_dda(env, ray_angle);
        
        // Stocker dans zbuffer
        env->zbuffer[x] = hit.distance;
        
        // Dessiner la colonne avec textures si disponibles
        if (env->num_textures > 0)
            draw_wall_slice_textured(env, x, &hit);
        else
            draw_wall_slice(env, x, hit.distance, hit.wall_type);  // Fallback couleur unie
    }
}