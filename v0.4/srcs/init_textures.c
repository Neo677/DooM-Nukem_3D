#include "env.h"
#include <stdio.h>
#include <stdlib.h>

// Déclaration externe
extern int load_bmp_texture(const char *path, t_texture *texture);

int init_textures(t_env *env)
{
    printf("Chargement des textures...\n");
    
    // Charger textures de murs
    env->num_textures = 2;
    env->wall_textures = malloc(sizeof(t_texture) * env->num_textures);
    
    if (!env->wall_textures)
    {
        printf("Erreur malloc wall_textures\n");
        return -1;
    }
    
    // Charger wall1.bmp
    if (load_bmp_texture("textures/wall1.bmp", &env->wall_textures[0]) != 0)
    {
        printf("Erreur: impossible de charger wall1.bmp\n");
        return -1;
    }
    
    // Charger wall2.bmp
    if (load_bmp_texture("textures/wall2.bmp", &env->wall_textures[1]) != 0)
    {
        printf("Erreur: impossible de charger wall2.bmp\n");
        return -1;
    }
    
    // IMPORTANT : Initialiser floor_texture et ceiling_texture à zéro
    env->floor_texture.pixels = NULL;
    env->floor_texture.width = 0;
    env->floor_texture.height = 0;
    
    env->ceiling_texture.pixels = NULL;
    env->ceiling_texture.width = 0;
    env->ceiling_texture.height = 0;
    
    // Charger sol
    if (load_bmp_texture("textures/floor1.bmp", &env->floor_texture) == 0)
    {
        printf("✅ Texture de sol chargée (pixels=%p, %dx%d)\n", 
               (void*)env->floor_texture.pixels,
               env->floor_texture.width,
               env->floor_texture.height);
    }
    else
    {
        printf("⚠️  Texture floor1.bmp non chargée\n");
    }
    
    // Charger plafond
    if (load_bmp_texture("textures/ceiling1.bmp", &env->ceiling_texture) == 0)
    {
        printf("✅ Texture de plafond chargée (pixels=%p, %dx%d)\n",
               (void*)env->ceiling_texture.pixels,
               env->ceiling_texture.width,
               env->ceiling_texture.height);
    }
    else
    {
        printf("⚠️  Texture ceiling1.bmp non chargée\n");
        // Fallback : utiliser la même que le sol si disponible
        if (env->floor_texture.pixels)
        {
            printf("ℹ️  Utilisation texture sol pour plafond\n");
            env->ceiling_texture = env->floor_texture;
        }
    }
    
    printf("✅ Système de textures initialisé (%d murs + sol + plafond)\n", 
           env->num_textures);
    
    return 0;
}

void free_textures(t_env *env)
{
    // Libérer textures de murs
    if (env->wall_textures)
    {
        for (int i = 0; i < env->num_textures; i++)
        {
            if (env->wall_textures[i].pixels)
                free(env->wall_textures[i].pixels);
        }
        free(env->wall_textures);
    }
    
    // Libérer floor
    if (env->floor_texture.pixels && 
        env->floor_texture.pixels != env->ceiling_texture.pixels)
    {
        free(env->floor_texture.pixels);
    }
    
    // Libérer ceiling (seulement si différent du floor)
    if (env->ceiling_texture.pixels && 
        env->ceiling_texture.pixels != env->floor_texture.pixels)
    {
        free(env->ceiling_texture.pixels);
    }
}
