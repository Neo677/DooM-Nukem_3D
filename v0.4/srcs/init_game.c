#include "env.h"
#include "loader_sectors.h"
#include "entities.h"

static void free_all(t_env *env)
{
    // Liberer map grid
    if (env->map.grid)
    {
        for (int i = 0; i < env->map.height; i++)
        {
            if (env->map.grid[i])
                free(env->map.grid[i]);
        }
        free(env->map.grid);
        env->map.grid = NULL;
    }
    
    // Liberer sectors
    free_sectors(env);
    
    // Liberer textures
    free_textures(env);
    free_skybox(env);
    free_entities(&env->entity_mgr);
    
    if (env->zbuffer)
        free(env->zbuffer);
    if (env->ytop_buffer)
        free(env->ytop_buffer);
    if (env->ybottom_buffer)
        free(env->ybottom_buffer);
    if (env->collision_buffer)
        free(env->collision_buffer);
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
    
    // VÉRIFICATION : Il faut un fichier de map en argument
    if (ac < 2)
    {
        printf("Usage: %s <map_file.dn>\n", av[0]);
        printf("Example: %s maps/sectors.dn\n", av[0]);
        return (1);
    }
    
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
    env.zbuffer = NULL;
    env.ytop_buffer = NULL;
    env.ybottom_buffer = NULL;
    env.collision_buffer = NULL;
    env.collision_buffer_size = MAX_COLLISION_BUFFER;
    env.map.grid = NULL;
    env.map.width = 0;
    env.map.height = 0;
    env.sector_map.sectors = NULL;
    env.sector_map.vertices = NULL;
    env.sector_map.nb_sectors = 0;
    env.sector_map.nb_vertices = 0;
    env.entity_mgr.entities = NULL;
    env.entity_mgr.count = 0;
    
    // NOUVEAU : Init vue 2D
    env.render_mode = MODE_3D;  // Demarrer en 3D
    env.view_2d.zoom = 50.0;    // 1 unite = 50 pixels
    env.view_2d.offset.x = 0.0;
    env.view_2d.offset.y = 0.0;
    env.view_2d.show_rays = 1;   // Rayons visibles par defaut
    env.view_2d.show_grid = 1;   // Grille visible par defaut
    env.view_2d.show_minimap = 0; // Minimap desactivee par defaut
    
    // NOUVEAU : Init capture souris
    env.mouse_captured = 0;       // Souris libre par defaut
    
    // Initialiser SDL
    if (init_sdl(&env))
    {
        free_all(&env);
        return (1);
    }
    
    // Allouer buffers
    env.zbuffer = (double *)malloc(sizeof(double) * env.w);
    env.ytop_buffer = (int *)malloc(sizeof(int) * env.w);
    env.ybottom_buffer = (int *)malloc(sizeof(int) * env.w);
    env.collision_buffer = (t_v2 *)malloc(sizeof(t_v2) * MAX_COLLISION_BUFFER);
    
    if (!env.zbuffer || !env.ytop_buffer || !env.ybottom_buffer || !env.collision_buffer)
    {
        DEBUG_LOG("Erreur allocation buffers\n");
        free_all(&env);
        return (1);
    }
    
    // Initialiser systeme de textures
    VERBOSE_LOG("Initialisation systeme de textures\n");
    if (init_textures(&env) != 0)
    {
        DEBUG_LOG("Erreur initialisation textures\n");
        free_all(&env);
        return (1);
    }
    
    // NOUVEAU : Init Skybox
    if (init_skybox(&env) != 0)
    {
        DEBUG_LOG("Erreur initialisation skybox\n");
        // Non-fatal, on peut continuer sans skybox
        env.skybox.enabled = 0;
    }
    
    // Initialiser map et joueur
    init_map(&env);
    init_player(&env);
    
    // Charger la map depuis l'argument (av[1])
    VERBOSE_LOG("Loading Sectors from: %s\n", av[1]);
    if (load_sectors(&env, av[1]) == 0)
    {
        VERBOSE_LOG("Sectors loaded successfully.\n");
        for(int i = 0; i < env.sector_map.nb_sectors; i++)
            DEBUG_LOG("Sector %d: %d vertices\n", i, env.sector_map.sectors[i].nb_vertices);
        
        env.player.current_sector = find_sector(&env, env.player.pos.x, env.player.pos.y);
        VERBOSE_LOG("Player Start Sector: %d\n", env.player.current_sector);
        
        if (env.player.current_sector == -1 && env.sector_map.nb_sectors > 0)
        {
            env.player.current_sector = 0;
        }
    }
    else
    {
        DEBUG_LOG("Failed to load sectors from: %s\n", av[1]);
        free_all(&env);
        return (1);
    }
    
    // Charger les entités (construire le chemin depuis le nom de la map)
    // Ex: maps/sectors.dn -> maps/entities.dn
    char entities_path[256];
    snprintf(entities_path, sizeof(entities_path), "maps/entities.dn");
    
    if (load_entities(&env, entities_path) != 0)
    {
        DEBUG_LOG("Failed to load entities from: %s\n", entities_path);
        // Non-fatal, on peut continuer sans entités
    }
    
    // Afficher le menu de demarrage
    int menu_result = show_menu(&env);
    
    if (menu_result == 0)
    {
        // L'utilisateur a quitte depuis le menu
        free_all(&env);
        return (0);
    }
    
    // Lancer jeu
    game_loop(&env);
    
    // Cleanup
    free_all(&env);
    
    return (0);
}