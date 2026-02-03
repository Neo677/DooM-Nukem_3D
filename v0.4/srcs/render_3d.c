#include "env.h"
#include "render_sector.h"
#define PI 3.14159265358979323846

// Forward declaration
void render_2d(t_env *env);

// Rendu 3D interne (anciennement render_3d)
static void render_3d_internal(t_env *env)
{
    // Variables inutilisees pour le rendu portails
    // double  fov_rad = (FOV * PI) / 180.0;
    // double  ray_angle;
    // t_ray_hit hit;
    // int     x;
    
    // NOUVEAU : Precompute skybox (la box 3D)
    precompute_skybox(env);

    // Portal rendering with sectors
    if (env->sector_map.nb_sectors > 0 && env->player.current_sector != -1)
    {
        // Use pre-allocated buffers
        int *ytop = env->ytop_buffer;
        int *ybottom = env->ybottom_buffer;
        
        if (ytop && ybottom)
        {
            // Init buffers (full screen visible)
            for (int i = 0; i < env->w; i++)
            {
                ytop[i] = 0;
                ybottom[i] = env->h - 1;
            }
            
            // Launch recursive rendering from player's sector
            render_sectors_recursive(env, env->player.current_sector, 0, env->w - 1, ytop, ybottom, 0);
        }
    }

    // NOUVEAU : Afficher minimap en overlay si activee
    draw_minimap(env);
    
    /* Old debug call removed */
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