#include "env.h"
#include <stdio.h>

int init_textures(t_env *env)
{
    // Textures murales (déjà existant)
    env->num_textures = 2;
    env->wall_textures = (t_texture*)malloc(sizeof(t_texture) * env->num_textures);
    
    if (!env->wall_textures)
    {
        printf("Erreur: allocation textures échouée\n");
        return -1;
    }
    
    for (int i = 0; i < env->num_textures; i++)
    {
        env->wall_textures[i].pixels = NULL;
        env->wall_textures[i].width = 0;
        env->wall_textures[i].height = 0;
    }
    
    printf("Chargement des textures...\n");
    
    // Murs
    if (load_bmp_texture("textures/wall1.bmp", &env->wall_textures[0]) != 0)
        printf("⚠️  Texture wall1.bmp non trouvée\n");
    
    if (load_bmp_texture("textures/wall2.bmp", &env->wall_textures[1]) != 0)
        printf("⚠️  Texture wall2.bmp non trouvée\n");
    
    // NOUVEAU : Initialiser textures floor/ceiling
    env->floor_texture.pixels = NULL;
    env->floor_texture.width = 0;
    env->floor_texture.height = 0;
    
    env->ceiling_texture.pixels = NULL;
    env->ceiling_texture.width = 0;
    env->ceiling_texture.height = 0;
    
    // Charger sol
    if (load_bmp_texture("textures/floor1.bmp", &env->floor_texture) != 0)
    {
        printf("⚠️  Texture floor1.bmp non trouvée\n");
    }
    else
    {
        printf("✅ Texture de sol chargée\n");
    }
    
    // Charger plafond (vous pouvez utiliser une autre texture)
    if (load_bmp_texture("textures/ceiling1.bmp", &env->ceiling_texture) != 0)
    {
        // Fallback : utiliser la même que le sol
        printf("ℹ️  Pas de texture plafond, utilisation texture sol\n");
        if (env->floor_texture.pixels)
        {
            env->ceiling_texture = env->floor_texture;  // Réutiliser
        }
    }
    else
    {
        printf("✅ Texture de plafond chargée\n");
    }
    
    printf("✅ Système de textures initialisé (%d murs + sol + plafond)\n", 
           env->num_textures);
    return 0;
}

// Libérer les textures
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
    env->num_textures = 0;
}
