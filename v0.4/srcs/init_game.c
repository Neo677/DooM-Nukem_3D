#include "env.h"

static void free_all(t_env *env)
{
    // Libérer textures
    free_textures(env);
    
    if (env->zbuffer)
        free(env->zbuffer);
    if (env->sdl.texture_pixels)
        free(env->sdl.texture_pixels);
    if (env->sdl.texture)
        SDL_DestroyTexture(env->sdl.texture);
    if (env->sdl.renderer)
        SDL_DestroyRenderer(env->sdl.renderer);
    if (env->sdl.window)
        SDL_DestroyWindow(env->sdl.window);
    SDL_Quit();
}

int init_game(int ac, char **av)
{
    t_env env;
    (void)ac;
    (void)av;
    
    // Initialiser valeurs
    env.w = 800;
    env.h = 600;
    env.running = 0;
    env.fps = 0;
    env.fps_count = 0;
    env.frame_timer = SDL_GetTicks();
    env.last_time = SDL_GetTicks();
    env.wall_textures = NULL;
    env.num_textures = 0;
    
    // Initialiser SDL
    if (init_sdl(&env))
    {
        free_all(&env);
        return (1);
    }
    
    // Allouer zbuffer
    env.zbuffer = (double *)malloc(sizeof(double) * env.w);
    if (!env.zbuffer)
    {
        printf("Erreur malloc zbuffer\n");
        free_all(&env);
        return (1);
    }
    
    // Initialiser map et player
    init_map(&env);
    init_player(&env);
    
    // Charger les textures
    printf("\n=== Initialisation système de textures ===\n");
    if (init_textures(&env) != 0)
    {
        printf("⚠️  Textures non chargées, utilisation couleurs unies\n");
    }
    
    // Afficher le menu de démarrage
    int menu_result = show_menu(&env);
    
    if (menu_result == 0)
    {
        free_all(&env);
        return (0);
    }
    
    // Lancer jeu
    game_loop(&env);
    free_all(&env);
    return (0);
}