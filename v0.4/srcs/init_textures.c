#include "env.h"
#include <stdio.h>
#include <stdlib.h>
extern int load_bmp_texture(const char *path, t_texture *texture);

int init_textures(t_env *env)
{
    printf("Chargement des textures...\n");
    
    env->num_textures = 2;
    env->wall_textures = calloc(env->num_textures, sizeof(t_texture));
    
    if (!env->wall_textures)
    {
        printf("Erreur malloc wall_textures\n");
        return -1;
    }
    
    if (load_bmp_texture("textures/flat5_6.bmp", &env->wall_textures[0]) != 0)
    {
        printf("Erreur: impossible de charger flat5_6.bmp\n");
        return -1;
    }
    
    if (load_bmp_texture("textures/wall1.bmp", &env->wall_textures[1]) != 0)
    {
        printf("Erreur: impossible de charger wall1.bmp\n");
        return -1;
    }
    
    env->floor_texture.pixels = NULL;
    env->floor_texture.width = 0;
    env->floor_texture.height = 0;
    
    env->ceiling_texture.pixels = NULL;
    env->ceiling_texture.width = 0;
    env->ceiling_texture.height = 0;
    
    if (load_bmp_texture("textures/floor1.bmp", &env->floor_texture) == 0)
    {
        printf("✅ Texture de sol chargee (pixels=%p, %dx%d)\n", 
               (void*)env->floor_texture.pixels,
               env->floor_texture.width,
               env->floor_texture.height);
    }
    else
    {
        printf("⚠️  Texture floor1.bmp non chargee\n");
    }
    
    if (load_bmp_texture("textures/ceiling1.bmp", &env->ceiling_texture) == 0)
    {
        printf("✅ Texture de plafond chargee (pixels=%p, %dx%d)\n",
               (void*)env->ceiling_texture.pixels,
               env->ceiling_texture.width,
               env->ceiling_texture.height);
    }
    else
    {
        printf("⚠️  Texture ceiling1.bmp non chargee\n");

        if (env->floor_texture.pixels)
        {
            printf("ℹ️  Utilisation texture sol pour plafond\n");
            env->ceiling_texture = env->floor_texture;
        }
    }
    
    printf("✅ Systeme de textures initialise (%d murs + sol + plafond)\n", 
           env->num_textures);
    
    return 0;
}

void free_textures(t_env *env)
{
    if (env->wall_textures)
    {
        for (int i = 0; i < env->num_textures; i++)
        {
            if (env->wall_textures[i].pixels)
            {
                free(env->wall_textures[i].pixels);
                env->wall_textures[i].pixels = NULL;
            }
        }
        free(env->wall_textures);
        env->wall_textures = NULL;
    }
    
    if (env->floor_texture.pixels)
    {

        if (env->ceiling_texture.pixels == env->floor_texture.pixels)
        {
            env->ceiling_texture.pixels = NULL;
            env->ceiling_texture.width = 0;
            env->ceiling_texture.height = 0;
        }
        free(env->floor_texture.pixels);
        env->floor_texture.pixels = NULL;
        env->floor_texture.width = 0;
        env->floor_texture.height = 0;
    }
    
    if (env->ceiling_texture.pixels)
    {
        free(env->ceiling_texture.pixels);
        env->ceiling_texture.pixels = NULL;
        env->ceiling_texture.width = 0;
        env->ceiling_texture.height = 0;
    }
}
