#include "env.h"
#include "render_sector.h"
#define PI 3.14159265358979323846

// Forward declaration
void render_2d(t_env *env);

// Rendu 3D interne (anciennement render_3d)
static void render_3d_internal(t_env *env)
{
    // Variables inutilisées pour le rendu portails
    // double  fov_rad = (FOV * PI) / 180.0;
    // double  ray_angle;
    // t_ray_hit hit;
    // int     x;
    
    // NOUVEAU : Dessiner Skybox EN PREMIER
    render_skybox(env);

    // MODE PORTAL (PHASE 3)
    // On désactive l'ancien rendu raycasting pour le "Full Portal Rendering"
    
    /* ANCIEN RENDU RAYCASTING
    // NOUVEAU : Dessiner floor et ceiling
    render_floor_ceiling(env);
    
    // Ensuite dessiner les murs par-dessus
    for (x = 0; x < env->w; x++)
    {
        // ... (code raycasting) ...
    }
    */
    
    // NOUVEAU RENDU SECTEURS
    if (env->sector_map.nb_sectors > 0 && env->player.current_sector != -1)
    {
        // Allocation buffers clipping
        int *ytop = (int *)malloc(sizeof(int) * env->w);
        int *ybottom = (int *)malloc(sizeof(int) * env->w);
        
        if (ytop && ybottom)
        {
            // Init buffers (Tout l'écran visible)
            for (int i = 0; i < env->w; i++)
            {
                ytop[i] = 0;
                ybottom[i] = env->h - 1;
            }
            
            // Lancer le rendu récursif depuis le secteur du joueur
            render_sectors_recursive(env, env->player.current_sector, 0, env->w - 1, ytop, ybottom);
            
            free(ytop);
            free(ybottom);
        }
    }

    // NOUVEAU : Afficher minimap en overlay si activée
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