#include "env.h"

// Dessiner une colonne de mur avec texture
void draw_wall_slice_textured(t_env *env, int x, t_ray_hit *hit)
{
    if (hit->wall_type <= 0 || hit->wall_type > env->num_textures)
    {
        // Pas de texture, dessiner en couleur unie
        draw_wall_slice(env, x, hit->distance, hit->wall_type);
        return;
    }
    
    // Calculer hauteur du mur à l'écran
    int wall_height = (int)(env->h / hit->distance);
    if (wall_height < 1) wall_height = 1;
    
    int draw_start = (env->h / 2) - (wall_height / 2);
    int draw_end = (env->h / 2) + (wall_height / 2);
    
    if (draw_start < 0) draw_start = 0;
    if (draw_end >= env->h) draw_end = env->h - 1;
    
    // Récupérer la texture
    t_texture *tex = &env->wall_textures[hit->wall_type - 1];
    
    if (!tex->pixels)
    {
        // Texture non chargée, fallback
        draw_wall_slice(env, x, hit->distance, hit->wall_type);
        return;
    }
    
    // Position X sur la texture (0 à tex->width)
    int tex_x = (int)(hit->wall_x * tex->width);
    if (tex_x < 0) tex_x = 0;
    if (tex_x >= tex->width) tex_x = tex->width - 1;
    
    // Le plafond est déjà rendu par render_floor_ceiling() - ne pas écraser!
    
    // Dessiner le mur texturing avec perspective
    for (int y = draw_start; y <= draw_end; y++)
    {
        // Position Y sur la texture (mapping perspective)
        int d = y * 256 - env->h * 128 + wall_height * 128;
        int tex_y = ((d * tex->height) / wall_height) / 256;
        
        // Clamper
        if (tex_y < 0) tex_y = 0;
        if (tex_y >= tex->height) tex_y = tex->height - 1;
        
        // Récupérer le pixel de la texture
        Uint32 color = tex->pixels[tex_y * tex->width + tex_x];
        
        // Distance fog (assombrir selon distance)
        int brightness = (int)(255.0 / (1.0 + hit->distance * hit->distance * 0.05));
        if (brightness > 255) brightness = 255;
        if (brightness < 0) brightness = 0;
        
        // Moduler la couleur selon la distance
        unsigned char r = ((color >> 16) & 0xFF) * brightness / 255;
        unsigned char g = ((color >> 8) & 0xFF) * brightness / 255;
        unsigned char b = (color & 0xFF) * brightness / 255;
        
        // Assombrir les murs horizontaux (NS) vs verticaux (EW)
        if (hit->side == 1)
        {
            r = r * 3 / 4;
            g = g * 3 / 4;
            b = b * 3 / 4;
        }
        
        color = 0xFF000000 | (r << 16) | (g << 8) | b;
        
        if (x >= 0 && x < env->w && y >= 0 && y < env->h)
            env->sdl.texture_pixels[x + y * env->w] = color;
    }
    
    // Le sol est déjà rendu par render_floor_ceiling() - ne pas écraser!
}
