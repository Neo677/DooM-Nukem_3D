#include "env.h"
#include <math.h>

#define SKYBOX_WIDTH 2048 // Width of skybox texture usually (or adaptive)
#define PI 3.14159265358979323846

int init_skybox(t_env *env)
{
    env->skybox.num_textures = 3;
    env->skybox.textures = (t_texture *)malloc(sizeof(t_texture) * env->skybox.num_textures);
    if (!env->skybox.textures)
        return (-1);
    
    // Charger les textures
    printf("Loading skybox 1: CTYSKY01.bmp...\n");
    if (load_bmp_texture("assets/skybox/CTYSKY01.bmp", &env->skybox.textures[0]) != 0)
        return (-1);
        
    printf("Loading skybox 2: Orangecloudmtn.bmp...\n");
    if (load_bmp_texture("assets/skybox/Orangecloudmtn.bmp", &env->skybox.textures[1]) != 0)
        return (-1);
        
    printf("Loading skybox 3: sentinel_sky.bmp...\n");
    if (load_bmp_texture("assets/skybox/sentinel_sky.bmp", &env->skybox.textures[2]) != 0)
        return (-1);
        
    env->skybox.current_id = 2; // Default to sentinel_sky (Higher Res)
    env->skybox.enabled = 1;
    env->skybox.offset = 0.0;
    
    return (0);
}

void free_skybox(t_env *env)
{
    if (env->skybox.textures)
    {
        for (int i = 0; i < env->skybox.num_textures; i++)
        {
            if (env->skybox.textures[i].pixels)
                free(env->skybox.textures[i].pixels);
        }
        free(env->skybox.textures);
    }
}

void switch_skybox(t_env *env, int id)
{
    if (id >= 0 && id < env->skybox.num_textures)
    {
        env->skybox.current_id = id;
        printf("Skybox switched to %d\n", id);
    }
}

void toggle_skybox(t_env *env)
{
    env->skybox.enabled = !env->skybox.enabled;
    printf("Skybox %s\n", env->skybox.enabled ? "ENABLED" : "DISABLED");
}

void render_skybox(t_env *env)
{
    if (!env->skybox.enabled)
    {
        // Si désactivé, remplir avec couleur noire ou ciel simple
        for (int i = 0; i < env->w * env->h; i++)
            env->sdl.texture_pixels[i] = 0xFF000000;
        return;
    }

    t_texture *tex = &env->skybox.textures[env->skybox.current_id];
    if (!tex->pixels) return;

    // Calcul de l'angle initial
    // On mappe 2*PI à tex->width
    // angle de 0 à 2*PI
    
    double fov_rad = (FOV * PI) / 180.0;
    
    // Rendu sur TOUT l'écran (fond global)
    // Les murs et sols viendront par dessus
    int grid_bottom = env->h; 
    
    for (int x = 0; x < env->w; x++)
    {
        // Angle correspondant à la colonne x
        double ray_angle = env->player.angle - (fov_rad / 2.0) + ((double)x / env->w) * fov_rad;
        
        // Normaliser l'angle entre 0 et 2*PI
        while (ray_angle < 0) ray_angle += 2 * PI;
        while (ray_angle >= 2 * PI) ray_angle -= 2 * PI;
        
        // Trouver la colonne de texture X correspondante
         // Mapping cylindrique simple
        int tex_x = (int)((ray_angle / (2 * PI)) * tex->width) % tex->width;
        
        // Dessiner la colonne verticale pour le ciel
        for (int y = 0; y < grid_bottom; y++)
        {
            // On mappe la hauteur de l'écran sur la hauteur de la texture
            // Horizon (H/2) correspond au milieu de la texture
            int tex_y = (y * tex->height) / env->h;
            
            if (tex_y >= tex->height) tex_y = tex->height - 1;
            if (tex_y < 0) tex_y = 0;
            
            Uint32 color = tex->pixels[tex_y * tex->width + tex_x];
            
            env->sdl.texture_pixels[y * env->w + x] = color;
        }
    }
}
