#include "env.h"
#include "render_sector.h"
#include "entities.h"
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
    
    // NOUVEAU: Pre-compute trigonometry tables for this frame
    if (env->angle_table && env->cos_table && env->sin_table)
    {
        double fov_rad = (FOV * PI) / 180.0;
        double start_angle = env->player.angle - (fov_rad / 2.0);
        
        for (int x = 0; x < env->w; x++)
        {
            double angle = start_angle + ((double)x / env->w) * fov_rad;
            env->angle_table[x] = angle;
            env->cos_table[x] = cos(angle);
            env->sin_table[x] = sin(angle);
        }
    }
    
    // NOUVEAU : Precompute skybox (la box 3D)
    precompute_skybox(env);
    
    // NOUVEAU : Dessiner le skybox en fond complet AVANT les secteurs
    if (env->skybox_enabled)
    {
        for (int x = 0; x < env->w; x++)
        {
            // Calculer l'angle du rayon pour cette colonne
            double fov_rad = (FOV * PI) / 180.0;
            double ray_angle = env->player.angle - (fov_rad / 2.0) + ((double)x / env->w) * fov_rad;
            
            // Dessiner le skybox sur toute la hauteur
            draw_skybox_column(env, x, 0, env->h - 1, ray_angle);
        }
    }

    // Portal rendering with sectors
    if (env->sector_map.nb_sectors > 0 && env->player.current_sector != -1)
    {
        // Use pre-allocated buffers (Chunk 0)
        int *ytop = env->ytop_pool;
        int *ybottom = env->ybottom_pool;
        
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

    // NOUVEAU : Rendre les ennemis après les secteurs
    render_enemies(env);

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