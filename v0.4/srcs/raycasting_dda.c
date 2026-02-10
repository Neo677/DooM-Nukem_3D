#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846


t_ray_hit cast_ray_dda(t_env *env, double ray_angle)
{
    t_ray_hit hit;
    
    
    double ray_dir_x = cos(ray_angle);
    double ray_dir_y = sin(ray_angle);
    
    
    int map_x = (int)env->player.pos.x;
    int map_y = (int)env->player.pos.y;
    
    
    double delta_dist_x = fabs(1.0 / ray_dir_x);
    double delta_dist_y = fabs(1.0 / ray_dir_y);
    
    
    int step_x = (ray_dir_x < 0) ? -1 : 1;
    int step_y = (ray_dir_y < 0) ? -1 : 1;
    
    
    double side_dist_x, side_dist_y;
    
    if (ray_dir_x < 0)
        side_dist_x = (env->player.pos.x - map_x) * delta_dist_x;
    else
        side_dist_x = (map_x + 1.0 - env->player.pos.x) * delta_dist_x;
        
    if (ray_dir_y < 0)
        side_dist_y = (env->player.pos.y - map_y) * delta_dist_y;
    else
        side_dist_y = (map_y + 1.0 - env->player.pos.y) * delta_dist_y;
    
    
    int hit_wall = 0;
    while (!hit_wall && map_x >= 0 && map_x < env->map.width && 
           map_y >= 0 && map_y < env->map.height)
    {
        
        if (side_dist_x < side_dist_y)
        {
            side_dist_x += delta_dist_x;
            map_x += step_x;
            hit.side = 0;  
        }
        else
        {
            side_dist_y += delta_dist_y;
            map_y += step_y;
            hit.side = 1;  
        }
        
        
        if (map_x >= 0 && map_x < env->map.width && 
            map_y >= 0 && map_y < env->map.height)
        {
            if (env->map.grid[map_y][map_x] != 0)
            {
                hit_wall = 1;
                hit.wall_type = env->map.grid[map_y][map_x];
            }
        }
        else
            break;  
    }
    
    
    if (hit.side == 0)
        hit.distance = (map_x - env->player.pos.x + 
                       (1 - step_x) / 2) / ray_dir_x;
    else
        hit.distance = (map_y - env->player.pos.y + 
                       (1 - step_y) / 2) / ray_dir_y;
    
    
    if (hit.side == 0)
        hit.wall_x = env->player.pos.y + hit.distance * ray_dir_y;
    else
        hit.wall_x = env->player.pos.x + hit.distance * ray_dir_x;
    hit.wall_x -= floor(hit.wall_x);  
    
    hit.map_x = map_x;
    hit.map_y = map_y;
    
    return hit;
}


void render_3d_dda(t_env *env)
{
    double fov_rad = (FOV * PI) / 180.0;
    double ray_angle;
    t_ray_hit hit;
    
    for (int x = 0; x < env->w; x++)
    {
        
        ray_angle = env->player.angle - (fov_rad / 2.0) + (x * fov_rad / env->w);
        
        
        hit = cast_ray_dda(env, ray_angle);
        
        
        env->zbuffer[x] = hit.distance;
        
        
        if (env->num_textures > 0)
            draw_wall_slice_textured(env, x, &hit);
        else
            draw_wall_slice(env, x, hit.distance, hit.wall_type);  
    }
}
