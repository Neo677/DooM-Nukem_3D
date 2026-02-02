#include "env.h"
#define PI 3.14159265358979323846

// Forward declaration
void render_2d(t_env *env);

// Rendu 3D interne (anciennement render_3d)
static void render_3d_internal(t_env *env)
{
    double  fov_rad = (FOV * PI) / 180.0;
    double  ray_angle;
    t_ray_hit hit;
    int     x;
    
    // NOUVEAU : Dessiner floor et ceiling EN PREMIER
    render_floor_ceiling(env);
    
    // Ensuite dessiner les murs par-dessus
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
    
    // NOUVEAU : Afficher minimap en overlay si activée
    draw_minimap(env);
}

// Wrapper principal - switch entre 2D et 3D
void render_3d(t_env *env)
{
    if (env->render_mode == MODE_2D)
    {
        render_2d(env);
    }
    else  // MODE_3D
    {
        render_3d_internal(env);
    }
}