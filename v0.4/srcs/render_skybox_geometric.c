#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846


static double clamp_d(double value, double min, double max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}



static void get_skybox_wall_params(double dir_x, double dir_y, int *face, double *u, double *dist)
{
    double abs_dx = fabs(dir_x);
    double abs_dy = fabs(dir_y);
    
    
    
    
    
    
    
    
    
    /*
        Faces mapping (from loader):
        [0]=bottom (Floor)
        [1]=top (Ceiling)
        [2]=back 
        [3]=left
        [4]=front
        [5]=right
        
        Assumptions:
        Right (X+) -> Face 5
        Left (X-) -> Face 3
        Front (Y- ?) -> Face 4
        Back (Y+ ?) -> Face 2
    */

    if (abs_dx >= abs_dy)
    {
        
        *dist = 1.0 / abs_dx;
        double y_hit = dir_y * (*dist); 
        
        if (dir_x > 0) 
        {
            *face = 5;
            
            
            
            
            
            *u = (y_hit + 1.0) * 0.5;
        }
        else 
        {
            *face = 3;
            
            
            
            
            *u = (1.0 - y_hit) * 0.5;
        }
    }
    else
    {
        
        *dist = 1.0 / abs_dy;
        double x_hit = dir_x * (*dist); 
        
        if (dir_y > 0) 
        {
            *face = 2; 
            
            
            
            
            
            
            
            
            
            
            
            
            *u = (1.0 - x_hit) * 0.5;
        }
        else 
        {
            *face = 4; 
            
            
            
            *u = (x_hit + 1.0) * 0.5;
        }
    }
    
    
    
}

void draw_skybox_geometric(t_env *env, int x, int y1, int y2, double angle)
{
    if (!env->skybox_enabled || env->current_skybox < 0) return;
    
    t_skybox *sb = &env->skyboxes[env->current_skybox];
    
    
    double dir_x = cos(angle);
    double dir_y = sin(angle);
    
    
    double dist_to_wall;
    int wall_face;
    double wall_u;
    
    get_skybox_wall_params(dir_x, dir_y, &wall_face, &wall_u, &dist_to_wall);
    
    
    
    
    
    
    
    double fov_scale = (env->w / 2.0) / tan((FOV * PI / 180.0) / 2.0);
    double horizon = env->player.horizon; 
    
    
    
    
    
    double y_top_proj = horizon - (1.0 / dist_to_wall) * fov_scale;
    
    
    double y_bot_proj = horizon - (-1.0 / dist_to_wall) * fov_scale;
    
    int y_start_wall = (int)y_top_proj;
    int y_end_wall = (int)y_bot_proj;
    
    
    t_texture *tex_wall = &sb->textures[wall_face];
    int tex_w_wall = tex_wall->width;
    int tex_h_wall = tex_wall->height;
    int u_pix_wall = (int)(wall_u * (tex_w_wall - 1));
    if (u_pix_wall < 0) u_pix_wall = 0;
    if (u_pix_wall >= tex_w_wall) u_pix_wall = tex_w_wall - 1;
    
    
    t_texture *tex_floor = &sb->textures[0]; 
    t_texture *tex_ceil = &sb->textures[1];  
    
    
    for (int y = y1; y <= y2; y++)
    {
        Uint32 color = 0;
        
        if (y < y_start_wall) 
        {
            if (tex_ceil->pixels)
            {
                
                
                
                
                
                
                
                
                
                
                
                double dy_screen = (double)y - horizon;
                if (dy_screen > -0.001) dy_screen = -0.001; 
                
                double dist_h = -1.0 * fov_scale / dy_screen;
                
                double map_x = dir_x * dist_h;
                double map_y = dir_y * dist_h;
                
                
                double u = (map_x + 1.0) * 0.5;
                double v = (map_y + 1.0) * 0.5;
                
                
                u = clamp_d(u, 0.0, 1.0);
                v = clamp_d(v, 0.0, 1.0);
                
                int tx = (int)(u * (tex_ceil->width - 1));
                int ty = (int)(v * (tex_ceil->height - 1));
                
                color = tex_ceil->pixels[ty * tex_ceil->width + tx];
            }
        }
        else if (y > y_end_wall) 
        {
            if (tex_floor->pixels)
            {
                
                
                
                double dy_screen = (double)y - horizon;
                if (dy_screen < 0.001) dy_screen = 0.001;
                
                double dist_h = fov_scale / dy_screen;
                
                double map_x = dir_x * dist_h;
                double map_y = dir_y * dist_h;
                
                
                double u = (map_x + 1.0) * 0.5;
                double v = (map_y + 1.0) * 0.5; 
                
                
                u = clamp_d(u, 0.0, 1.0);
                v = clamp_d(v, 0.0, 1.0);
                
                int tx = (int)(u * (tex_floor->width - 1));
                int ty = (int)(v * (tex_floor->height - 1));
                
                color = tex_floor->pixels[ty * tex_floor->width + tx];
            }
        }
        else 
        {
            if (tex_wall->pixels)
            {
                
                
                
                
                
                
                double v = ((double)y - y_top_proj) / (y_bot_proj - y_top_proj);
                
                v = clamp_d(v, 0.0, 1.0);
                
                int v_pix = (int)(v * (tex_h_wall - 1));
                color = tex_wall->pixels[v_pix * tex_w_wall + u_pix_wall];
            }
        }
        
        env->sdl.texture_pixels[y * env->w + x] = color;
    }
}
