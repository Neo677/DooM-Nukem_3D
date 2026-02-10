#include "env.h"
#include "render_sector.h"
#include "entities.h"
#define PI 3.14159265358979323846


void render_2d(t_env *env);


static void render_3d_internal(t_env *env)
{
    
    
    
    
    
    
    
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
    
    
    precompute_skybox(env);
    
    
    if (env->skybox_enabled)
    {
        for (int x = 0; x < env->w; x++)
        {
            
            double fov_rad = (FOV * PI) / 180.0;
            double ray_angle = env->player.angle - (fov_rad / 2.0) + ((double)x / env->w) * fov_rad;
            
            
            draw_skybox_column(env, x, 0, env->h - 1, ray_angle);
        }
    }

    
    if (env->sector_map.nb_sectors > 0 && env->player.current_sector != -1)
    {
        
        int *ytop = env->ytop_pool;
        int *ybottom = env->ybottom_pool;
        
        if (ytop && ybottom)
        {
            
            for (int i = 0; i < env->w; i++)
            {
                ytop[i] = 0;
                ybottom[i] = env->h - 1;
            }
            
            
            render_sectors_recursive(env, env->player.current_sector, 0, env->w - 1, ytop, ybottom, 0);
        }
    }

    
    render_enemies(env);

    
    draw_minimap(env);
    
    /* Old debug call removed */
}


void render_3d(t_env *env)
{
    if (env->render_mode == MODE_2D)
    {
        render_2d(env);
    }
    else  
    {
        render_3d_internal(env);
    }
}