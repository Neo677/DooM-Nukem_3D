#include "env.h"
#include "loader_sectors.h"
#include "entities.h"
#include <string.h>

static void free_all(t_env *env)
{
    
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
    
    
    free_sectors(env);
    
    
    free_textures(env);
    
    
    if (env->enemy_textures)
    {
        for (int i = 0; i < env->num_enemy_textures; i++)
        {
            if (env->enemy_textures[i].pixels)
            {
                free(env->enemy_textures[i].pixels);
                env->enemy_textures[i].pixels = NULL;
            }
        }
        free(env->enemy_textures);
        env->enemy_textures = NULL;
    }
    
    free_skybox(env);
    free_entities(&env->entity_mgr); 
    
    if (env->zbuffer) { free(env->zbuffer); env->zbuffer = NULL; }
    if (env->ytop_pool) { free(env->ytop_pool); env->ytop_pool = NULL; }
    if (env->ybottom_pool) { free(env->ybottom_pool); env->ybottom_pool = NULL; }
    if (env->angle_table) { free(env->angle_table); env->angle_table = NULL; }
    if (env->cos_table) { free(env->cos_table); env->cos_table = NULL; }
    if (env->sin_table) { free(env->sin_table); env->sin_table = NULL; }
    if (env->collision_buffer) { free(env->collision_buffer); env->collision_buffer = NULL; }
    if (env->sdl.texture_pixels) { free(env->sdl.texture_pixels); env->sdl.texture_pixels = NULL; }
    if (env->sdl.texture) { SDL_DestroyTexture(env->sdl.texture); env->sdl.texture = NULL; }
    if (env->sdl.renderer) { SDL_DestroyRenderer(env->sdl.renderer); env->sdl.renderer = NULL; }
    if (env->sdl.window) { SDL_DestroyWindow(env->sdl.window); env->sdl.window = NULL; }
    SDL_Quit();
}

int init_game(int ac, char **av)
{
    t_env env;
    
    
    memset(&env, 0, sizeof(t_env));
    
    printf("Debug: sizeof(t_env) = %lu bytes\n", sizeof(t_env));
    
    
    if (ac < 2)
    {
        printf("Usage: %s <map_file.dn>\n", av[0]);
        printf("Example: %s maps/sectors.dn\n", av[0]);
        return (1);
    }
    
    
    env.w = 800;
    env.h = 600;
    env.running = 0;
    env.fps = 0;
    env.fps_count = 0;
    env.frame_timer = SDL_GetTicks();
    env.last_time = SDL_GetTicks();
    env.wall_textures = NULL;
    env.num_textures = 0;
    env.enemy_textures = NULL;
    env.num_enemy_textures = 0;
    env.zbuffer = NULL;
    env.ytop_pool = NULL;
    env.ybottom_pool = NULL;
    env.angle_table = NULL;
    env.cos_table = NULL;
    env.sin_table = NULL;
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
    
    
    env.render_mode = MODE_3D;  
    env.view_2d.zoom = 50.0;    
    env.view_2d.offset.x = 0.0;
    env.view_2d.offset.y = 0.0;
    env.view_2d.show_rays = 1;   
    env.view_2d.show_grid = 1;   
    env.view_2d.show_minimap = 0; 
    
    
    env.mouse_captured = 0;       
    
    printf("Debug: Calling init_sdl with w=%d, h=%d\n", env.w, env.h);
    
    
    if (init_sdl(&env))
    {
        free_all(&env);
        return (1);
    }
    
    
    env.zbuffer = (double *)malloc(sizeof(double) * env.w);
    env.ytop_pool = (int *)malloc(sizeof(int) * env.w * MAX_RECURSION_DEPTH);
    env.ybottom_pool = (int *)malloc(sizeof(int) * env.w * MAX_RECURSION_DEPTH);
    env.angle_table = (double *)malloc(sizeof(double) * env.w);
    env.cos_table = (double *)malloc(sizeof(double) * env.w);
    env.sin_table = (double *)malloc(sizeof(double) * env.w);
    env.collision_buffer = (t_v2 *)malloc(sizeof(t_v2) * MAX_COLLISION_BUFFER);
    
    if (!env.zbuffer || !env.ytop_pool || !env.ybottom_pool || !env.collision_buffer || !env.angle_table || !env.cos_table || !env.sin_table)
    {
        DEBUG_LOG("Erreur allocation buffers\n");
        free_all(&env);
        return (1);
    }
    
    
    VERBOSE_LOG("Initialisation systeme de textures\n");
    if (init_textures(&env) != 0)
    {
        DEBUG_LOG("Erreur initialisation textures\n");
        free_all(&env);
        return (1);
    }
    
    
    if (init_skybox(&env) != 0)
    {
        DEBUG_LOG("Erreur initialisation skybox\n");
        
        env.skybox_enabled = 0;
    }
    
    
    VERBOSE_LOG("Chargement des sprites ennemis...\n");
    if (load_enemy_sprites(&env) != 0)
    {
        DEBUG_LOG("Erreur lors du chargement des sprites ennemis\n");
        
    }
    
    
    init_map(&env);
    init_player(&env);
    
    
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
    
    
    if (env.sector_map.nb_sectors == 4 && strstr(av[1], "stairs.dn"))
    {
        t_sector *elevator = &env.sector_map.sectors[3];
        elevator->target_floor = 3.0;
        elevator->target_ceil = 4.0; 
        elevator->speed = 0.02;
        elevator->state = 1; 
        elevator->trigger_id = 999; 
        printf("✅ DEMO: Elevator activated in Sector 3 (Stairs Map)!\n");
        
        
        env.player.pos.x = 2.0;
        env.player.pos.y = 2.0;
        env.player.angle = 0.0;
        env.player.current_sector = 0;
    }
    
    
    
    char entities_path[256];
    snprintf(entities_path, sizeof(entities_path), "maps/entities.dn");
    
    if (load_entities(&env, entities_path) != 0)
    {
        DEBUG_LOG("Failed to load entities from: %s\n", entities_path);
        
    }
    
    
    int menu_result = show_menu(&env);
    
    if (menu_result == 0)
    {
        
        free_all(&env);
        return (0);
    }
    
    
    game_loop(&env);
    
    
    free_all(&env);
    
    return (0);
}