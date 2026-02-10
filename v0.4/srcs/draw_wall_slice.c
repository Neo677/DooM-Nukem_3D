#include "env.h"
void    draw_wall_slice(t_env *env, int x, double wall_dist, int wall_type)
{
    int     wall_height;
    int     draw_start;
    int     draw_end;
    Uint32  color;
    int     y;
    
    if (wall_dist < 0.1)
        wall_dist = 0.1;
    
    wall_height = (int)(env->h / wall_dist);
    
    draw_start = (env->h / 2) - (wall_height / 2);
    draw_end = (env->h / 2) + (wall_height / 2);
    
    
    if (draw_start < 0)
        draw_start = 0;
    if (draw_end >= env->h)
        draw_end = env->h - 1;
    
    
    if (wall_type == 1)
        color = 0xFFFF0000;  
    else
        color = 0xFF00FF00;  
    
    int brightness = (int)(255.0 / (1.0 + wall_dist * wall_dist * 0.1));
    if (brightness > 255)
        brightness = 255;
    if (brightness < 0)
        brightness = 0;
    
    
    if (wall_type == 1)
        color = 0xFF000000 | (brightness << 16);  
    else
        color = 0xFF000000 | (brightness << 8);   
    
    
    for (y = 0; y < draw_start; y++)
    {
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = 0xFF333333;
    }
    
    
    for (y = draw_start; y <= draw_end; y++)
    {
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = color;
    }
    
    
    for (y = draw_end + 1; y < env->h; y++)
    {
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = 0xFF666666;
    }
}
