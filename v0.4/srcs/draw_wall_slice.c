#include "env.h"
void    draw_wall_slice(t_env *env, int x, double wall_dist, int wall_type)
{
    int     wall_height;
    int     draw_start;
    int     draw_end;
    Uint32  color;
    int     y;
    // Calculer hauteur du mur a l'ecran
    if (wall_dist < 0.1)
        wall_dist = 0.1;
    
    wall_height = (int)(env->h / wall_dist);
    
    draw_start = (env->h / 2) - (wall_height / 2);
    draw_end = (env->h / 2) + (wall_height / 2);
    
    // Clamper
    if (draw_start < 0)
        draw_start = 0;
    if (draw_end >= env->h)
        draw_end = env->h - 1;
    
    // Choisir couleur selon type de mur
    if (wall_type == 1)
        color = 0xFFFF0000;  // Rouge
    else
        color = 0xFF00FF00;  // Vert
    // Assombrir selon distance
    int brightness = (int)(255.0 / (1.0 + wall_dist * wall_dist * 0.1));
    if (brightness > 255)
        brightness = 255;
    if (brightness < 0)
        brightness = 0;
    
    // Pour mur rouge
    if (wall_type == 1)
        color = 0xFF000000 | (brightness << 16);  // Rouge module
    else
        color = 0xFF000000 | (brightness << 8);   // Vert module
    
    // Dessiner plafond (gris fonce)
    for (y = 0; y < draw_start; y++)
    {
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = 0xFF333333;
    }
    
    // Dessiner mur
    for (y = draw_start; y <= draw_end; y++)
    {
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = color;
    }
    
    // Dessiner sol (gris)
    for (y = draw_end + 1; y < env->h; y++)
    {
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = 0xFF666666;
    }
}
