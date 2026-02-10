#include "env.h"
#include <math.h>
#include <string.h>


static int load_cubemap_face(const char *path, t_texture *texture)
{
    if (!path || !texture)
        return (-1);
    
    if (load_bmp_texture(path, texture) != 0)
    {
        printf("❌ Failed to load cubemap face: %s\n", path);
        return (-1);
    }
    printf("✅ Loaded cubemap face: %s (%dx%d)\n", path, texture->width, texture->height);
    return (0);
}


static void free_skybox_faces(t_skybox *sb, int up_to_face)
{
    if (!sb)
        return;
    
    for (int i = 0; i < up_to_face && i < 6; i++)
    {
        if (sb->textures[i].pixels)
        {
            free(sb->textures[i].pixels);
            sb->textures[i].pixels = NULL;
            sb->textures[i].width = 0;
            sb->textures[i].height = 0;
        }
    }
}


static int load_skybox_set(t_env *env, int skybox_id,
    const char *bottom, const char *top, const char *back,
    const char *left, const char *front, const char *right)
{
    if (!env || skybox_id < 0 || skybox_id >= MAX_SKYBOX)
        return (-1);
    
    t_skybox *sb = &env->skyboxes[skybox_id];
    
    
    if (load_cubemap_face(bottom, &sb->textures[0]) != 0)
    {
        free_skybox_faces(sb, 0);
        return (-1);
    }
    if (load_cubemap_face(top, &sb->textures[1]) != 0)
    {
        free_skybox_faces(sb, 1);
        return (-1);
    }
    if (load_cubemap_face(back, &sb->textures[2]) != 0)
    {
        free_skybox_faces(sb, 2);
        return (-1);
    }
    if (load_cubemap_face(left, &sb->textures[3]) != 0)
    {
        free_skybox_faces(sb, 3);
        return (-1);
    }
    if (load_cubemap_face(front, &sb->textures[4]) != 0)
    {
        free_skybox_faces(sb, 4);
        return (-1);
    }
    if (load_cubemap_face(right, &sb->textures[5]) != 0)
    {
        free_skybox_faces(sb, 5);
        return (-1);
    }
    
    printf("🌌 Skybox %d loaded successfully (6 faces loaded)\n", skybox_id);
    return (0);
}


static void free_single_skybox(t_skybox *sb)
{
    if (!sb)
        return;
    
    if (sb->name)
    {
        free(sb->name);
        sb->name = NULL;
    }
    
    free_skybox_faces(sb, 6);
}

int init_skybox(t_env *env)
{
    if (!env)
        return (-1);
    
    printf("\n=== Initializing Cubemap Skybox System ===\n");
    
    
    env->num_skyboxes = 0;
    env->current_skybox = 0;
    env->skybox_enabled = 1;
    
    
    env->skybox_box_size = 10.0;
    env->skybox_computed = 0;
    
    
    for (int i = 0; i < MAX_SKYBOX; i++)
    {
        env->skyboxes[i].name = NULL;
        for (int j = 0; j < 6; j++)
        {
            env->skyboxes[i].textures[j].pixels = NULL;
            env->skyboxes[i].textures[j].width = 0;
            env->skyboxes[i].textures[j].height = 0;
        }
    }
    
    
    printf("\n--- Loading Skybox 0: Sentinel Sky ---\n");
    env->skyboxes[0].name = strdup("Sentinel Sky");
    if (!env->skyboxes[0].name)
    {
        printf("❌ Failed to allocate skybox name\n");
        return (-1);
    }
    
    if (load_skybox_set(env, 0,
        "assets/skybox/sentinel_sky.bmp",  
        "assets/skybox/sentinel_sky.bmp",  
        "assets/skybox/sentinel_sky.bmp",  
        "assets/skybox/sentinel_sky.bmp",  
        "assets/skybox/sentinel_sky.bmp",  
        "assets/skybox/sentinel_sky.bmp") != 0)  
    {
        printf("❌ Failed to load Skybox 0\n");
        free_single_skybox(&env->skyboxes[0]);
        return (-1);
    }
    env->num_skyboxes++;
    
    
    printf("\n--- Loading Skybox 1: City Sky ---\n");
    env->skyboxes[1].name = strdup("City Sky");
    if (!env->skyboxes[1].name)
    {
        printf("❌ Failed to allocate skybox name\n");
        free_skybox(env);
        return (-1);
    }
    
    if (load_skybox_set(env, 1,
        "assets/skybox/CTYSKY01.bmp",  
        "assets/skybox/CTYSKY01.bmp",  
        "assets/skybox/CTYSKY01.bmp",  
        "assets/skybox/CTYSKY01.bmp",  
        "assets/skybox/CTYSKY01.bmp",  
        "assets/skybox/CTYSKY01.bmp") != 0)  
    {
        printf("❌ Failed to load Skybox 1\n");
        free_single_skybox(&env->skyboxes[1]);
        free_skybox(env);
        return (-1);
    }
    env->num_skyboxes++;
    
    
    printf("\n--- Loading Skybox 2: Orange Cloud Mountain ---\n");
    env->skyboxes[2].name = strdup("Orange Cloud");
    if (!env->skyboxes[2].name)
    {
        printf("❌ Failed to allocate skybox name\n");
        free_skybox(env);
        return (-1);
    }
    
    if (load_skybox_set(env, 2,
        "assets/skybox/Orangecloudmtn.bmp",  
        "assets/skybox/Orangecloudmtn.bmp",  
        "assets/skybox/Orangecloudmtn.bmp",  
        "assets/skybox/Orangecloudmtn.bmp",  
        "assets/skybox/Orangecloudmtn.bmp",  
        "assets/skybox/Orangecloudmtn.bmp") != 0)  
    {
        printf("❌ Failed to load Skybox 2\n");
        free_single_skybox(&env->skyboxes[2]);
        free_skybox(env);
        return (-1);
    }
    env->num_skyboxes++;
    
    printf("\n✅ Skybox system initialized successfully (%d skyboxes loaded)\n", env->num_skyboxes);
    printf("Active skybox: #%d - %s\n\n", env->current_skybox, env->skyboxes[env->current_skybox].name);
    
    return (0);
}

void free_skybox(t_env *env)
{
    if (!env)
        return;
    
    printf("Freeing skybox resources...\n");
    
    
    for (int i = 0; i < MAX_SKYBOX; i++)
    {
        free_single_skybox(&env->skyboxes[i]);
    }
    
    env->num_skyboxes = 0;
}

void switch_skybox(t_env *env, int id)
{
    if (!env)
        return;
    
    if (id >= 0 && id < env->num_skyboxes)
    {
        env->current_skybox = id;
        printf("Skybox switched to #%d - %s\n", id, env->skyboxes[id].name);
    }
    else
    {
        printf("Invalid skybox ID: %d (valid range: 0-%d)\n", id, env->num_skyboxes - 1);
    }
}

void toggle_skybox(t_env *env)
{
    if (!env)
        return;
    
    env->skybox_enabled = !env->skybox_enabled;
    printf("Skybox %s\n", env->skybox_enabled ? "ENABLED" : "DISABLED");
}


void render_skybox(t_env *env)
{
    (void)env;
    printf("[WARNING] render_skybox() is deprecated - use draw_skybox() instead\n");
}