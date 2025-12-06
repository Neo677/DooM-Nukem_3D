#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "p_player.h"
#include "g_game_state.h"
#include "w_window.h"
#include "r_renderer.h"
#include "k_keyboard.h"

#define SCRNW 1024
#define SCRNH 768
#define FPS 120

// Forward declarations
void M_AutoConnectSectors(map_t *map);

void M_AddSectorToMap(map_t *map, sector_t sector)
{
    if (map->num_sectors < 1024)
    {
        // Assign ID based on index
        sector.id = map->num_sectors;
        map->sectors[map->num_sectors++] = sector;
        // R_AddSectorToMap is called later or we can sync it
        R_AddSectorToMap(&sector);
    }
}

// Helper to create a portal manually (used by reference map logic if needed, but we will use AutoConnect)
wall_t M_CreatePortal(int x1, int y1, int x2, int y2, int neighbor_id)
{
    wall_t w;
    w.a.x = x1; w.a.y = y1;
    w.b.x = x2; w.b.y = y2;
    w.is_portal = true;
    w.neighbor_sector_id = neighbor_id;
    w.portal_top_height = 0; 
    w.portal_bot_height = 0;
    return w;
}

void M_CreateReferenceMap(map_t *map)
{
    // Recreating the map from the reference repository
    // https://github.com/jeuxdemains/DOOM-like-game-engine-part-I/blob/main/main.c

    // Sectors s1 to s16
    // Colors:
    // s1-s4: 0xd6382d (Reddish)
    // s5-s7: 0x29ba48 (Greenish)
    // s8-s10: 0x29ba48 (Greenish)
    // s11-s16: 0xa3a24b (Yellowish)
    
    unsigned int c_red = 0xd6382d;
    unsigned int c_green = 0x29ba48;
    unsigned int c_yellow = 0xa3a24b;
    
    unsigned int floor_def = 0x808080;
    unsigned int ceil_def = 0x404040;

    // s1: height 10, elev 0
    sector_t s1 = R_CreateSector(10, 0, c_red, floor_def, ceil_def);
    // s2: height 20, elev 0
    sector_t s2 = R_CreateSector(20, 0, c_red, floor_def, ceil_def);
    // s3: height 30, elev 0
    sector_t s3 = R_CreateSector(30, 0, c_red, floor_def, ceil_def);
    // s4: height 40, elev 0
    sector_t s4 = R_CreateSector(40, 0, c_red, floor_def, ceil_def);

    // s5-s7: height 80, elev 0
    sector_t s5 = R_CreateSector(80, 0, c_green, floor_def, ceil_def);
    sector_t s6 = R_CreateSector(80, 0, c_green, floor_def, ceil_def);
    sector_t s7 = R_CreateSector(80, 0, c_green, floor_def, ceil_def);

    // s8-s10: height 80, elev 0
    sector_t s8 = R_CreateSector(80, 0, c_green, floor_def, ceil_def);
    sector_t s9 = R_CreateSector(80, 0, c_green, floor_def, ceil_def);
    sector_t s10 = R_CreateSector(80, 0, c_green, floor_def, ceil_def);

    // s11-s12: height 30, elev 0
    sector_t s11 = R_CreateSector(30, 0, c_yellow, floor_def, ceil_def);
    sector_t s12 = R_CreateSector(30, 0, c_yellow, floor_def, ceil_def);

    // s13-s14: height 10, elev 0
    sector_t s13 = R_CreateSector(10, 0, c_yellow, floor_def, ceil_def);
    sector_t s14 = R_CreateSector(10, 0, c_yellow, floor_def, ceil_def);
    
    // s15-s16: height 30, elev 10
    sector_t s15 = R_CreateSector(30, 10, c_yellow, floor_def, ceil_def);
    sector_t s16 = R_CreateSector(30, 10, c_yellow, floor_def, ceil_def);

    // Vertices
    int s1v[] = { 70, 220, 100, 220, 100, 220, 100, 240, 100, 240, 70, 240, 70, 240, 70, 220 };
    int s2v[] = { 70, 200, 100, 200, 100, 200, 100, 220, 100, 220, 70, 220, 70, 220, 70, 200 };
    int s3v[] = { 70, 180, 100, 180, 100, 180, 100, 200, 100, 200, 70, 200, 70, 200, 70, 180 };
    
    int s4v[] = { 
        70, 120, 100, 120,
        100, 120, 110, 140,
        110, 140, 110, 160,
        110, 160, 100, 180,
        100, 180, 70, 180,
        70, 180, 60, 160,
        60, 160, 60, 140,
        60, 140, 70, 120 
    };

    int s5v[] = { 30, 190, 40, 190, 40, 190, 50, 200, 50, 200, 50, 220, 50, 220, 30, 190 };
    int s6v[] = { 30, 120, 40, 120, 40, 120, 40, 190, 40, 190, 30, 190, 30, 190, 30, 120 };
    int s7v[] = { 60, 70, 60, 90, 60, 90, 40, 120, 40, 120, 30, 120, 30, 120, 60, 70 };

    int s8v[] = { 120, 200, 130, 190, 130, 190, 140, 190, 140, 190, 120, 220, 120, 220, 120, 200 };
    int s9v[] = { 130, 120, 140, 120, 140, 120, 140, 190, 140, 190, 130, 190, 130, 190, 130, 120 };
    int s10v[] = { 110, 70, 140, 120, 140, 120, 130, 120, 130, 120, 110, 90, 110, 90, 110, 70 };

    int s11v[] = { 30, 20, 50, 20, 50, 20, 50, 50, 50, 50, 30, 50, 30, 50, 30, 20 };
    int s12v[] = { 120, 20, 140, 20, 140, 20, 140, 50, 140, 50, 120, 50, 120, 50, 120, 20 };

    int s13v[] = { 30, 250, 60, 250, 60, 250, 60, 300, 60, 300, 30, 300, 30, 300, 30, 250 };
    int s14v[] = { 110, 250, 140, 250, 140, 250, 140, 300, 140, 300, 110, 300, 110, 300, 110, 250 };
    int s15v[] = { 40, 260, 50, 260, 50, 260, 50, 290, 50, 290, 40, 290, 40, 290, 40, 260 };
    int s16v[] = { 120, 260, 130, 260, 130, 260, 130, 290, 130, 290, 120, 290, 120, 290, 120, 260 };

    // Add walls
    for (int i = 0; i < 16; i += 4)
    {
        R_SectorAddWall(&s1, R_CreateWall(s1v[i], s1v[i+1], s1v[i+2], s1v[i+3]));
        R_SectorAddWall(&s2, R_CreateWall(s2v[i], s2v[i+1], s2v[i+2], s2v[i+3]));
        R_SectorAddWall(&s3, R_CreateWall(s3v[i], s3v[i+1], s3v[i+2], s3v[i+3]));
        
        R_SectorAddWall(&s5, R_CreateWall(s5v[i], s5v[i+1], s5v[i+2], s5v[i+3]));
        R_SectorAddWall(&s6, R_CreateWall(s6v[i], s6v[i+1], s6v[i+2], s6v[i+3]));
        R_SectorAddWall(&s7, R_CreateWall(s7v[i], s7v[i+1], s7v[i+2], s7v[i+3]));
        
        R_SectorAddWall(&s8, R_CreateWall(s8v[i], s8v[i+1], s8v[i+2], s8v[i+3]));
        R_SectorAddWall(&s9, R_CreateWall(s9v[i], s9v[i+1], s9v[i+2], s9v[i+3]));
        R_SectorAddWall(&s10, R_CreateWall(s10v[i], s10v[i+1], s10v[i+2], s10v[i+3]));
        
        R_SectorAddWall(&s11, R_CreateWall(s11v[i], s11v[i+1], s11v[i+2], s11v[i+3]));
        R_SectorAddWall(&s12, R_CreateWall(s12v[i], s12v[i+1], s12v[i+2], s12v[i+3]));
        
        R_SectorAddWall(&s13, R_CreateWall(s13v[i], s13v[i+1], s13v[i+2], s13v[i+3]));
        R_SectorAddWall(&s14, R_CreateWall(s14v[i], s14v[i+1], s14v[i+2], s14v[i+3]));
        R_SectorAddWall(&s15, R_CreateWall(s15v[i], s15v[i+1], s15v[i+2], s15v[i+3]));
        R_SectorAddWall(&s16, R_CreateWall(s16v[i], s16v[i+1], s16v[i+2], s16v[i+3]));
    }

    // s4 has 8 walls
    for (int i = 0; i < 32; i += 4)
    {
        R_SectorAddWall(&s4, R_CreateWall(s4v[i], s4v[i+1], s4v[i+2], s4v[i+3]));
    }

    // Add sectors to map
    M_AddSectorToMap(map, s1);
    M_AddSectorToMap(map, s2);
    M_AddSectorToMap(map, s3);
    M_AddSectorToMap(map, s4);
    M_AddSectorToMap(map, s5);
    M_AddSectorToMap(map, s6);
    M_AddSectorToMap(map, s7);
    M_AddSectorToMap(map, s8);
    M_AddSectorToMap(map, s9);
    M_AddSectorToMap(map, s10);
    M_AddSectorToMap(map, s11);
    M_AddSectorToMap(map, s12);
    M_AddSectorToMap(map, s13);
    M_AddSectorToMap(map, s14);
    M_AddSectorToMap(map, s15);
    M_AddSectorToMap(map, s16);
}

void M_AutoConnectSectors(map_t *map)
{
    // Iterate over all sectors
    for (int i = 0; i < map->num_sectors; i++)
    {
        sector_t *s1 = &map->sectors[i];
        
        for (int j = 0; j < map->num_sectors; j++)
        {
            if (i == j) continue;
            
            sector_t *s2 = &map->sectors[j];
            
            // Check for shared walls
            for (int w1 = 0; w1 < s1->num_walls; w1++)
            {
                for (int w2 = 0; w2 < s2->num_walls; w2++)
                {
                    wall_t *wall1 = &s1->walls[w1];
                    wall_t *wall2 = &s2->walls[w2];
                    
                    // Check if walls are identical (reversed)
                    // Allow small margin of error for floats, but these are ints
                    if (wall1->a.x == wall2->b.x && wall1->a.y == wall2->b.y &&
                        wall1->b.x == wall2->a.x && wall1->b.y == wall2->a.y)
                    {
                        // Found a shared wall!
                        wall1->is_portal = true;
                        wall1->neighbor_sector_id = s2->id;
                        
                        // Calculate portal heights
                        // Portal bottom is the highest floor
                        wall1->portal_bot_height = fmax(s1->elevation, s2->elevation);
                        
                        // Portal top is the lowest ceiling
                        double s1_ceil = s1->elevation + s1->height;
                        double s2_ceil = s2->elevation + s2->height;
                        wall1->portal_top_height = fmin(s1_ceil, s2_ceil);
                        
                        // Also update the other side? 
                        // The loop will reach it eventually when i=j and j=i
                        // But we can do it now to be sure
                        wall2->is_portal = true;
                        wall2->neighbor_sector_id = s1->id;
                        wall2->portal_bot_height = wall1->portal_bot_height;
                        wall2->portal_top_height = wall1->portal_top_height;
                        
                        printf("Connected Sector %d to %d via wall\n", s1->id, s2->id);
                    }
                }
            }
        }
    }
}

map_t M_CreateMap(void)
{
    map_t map = {0};
    map.sectors = malloc(sizeof(sector_t) * 1024);
    map.num_sectors = 0;
    
    // Spawn point from reference
    // player_t player = P_Init(40, 40, SCRNH * 10, M_PI/2);
    map.spawn_point.x = 40;
    map.spawn_point.y = 40;
    map.spawn_angle = M_PI/2;
    
    M_CreateReferenceMap(&map);
    M_AutoConnectSectors(&map);
    
    return map;
}

void GameLoop(game_state_t *game_state, player_t *player)
{
    while(game_state->is_running)
    {
        G_FrameStart();

        K_HandleEvents(game_state, player);
        R_Render(player, game_state);

        G_FrameEnd(game_state);
    }
}

int main()
{
    // Initialisation des systèmes de base
    game_state_t game_state = G_Init(SCRNW, SCRNH, FPS);
    K_InitKeymap();
    W_Init(SCRNW, SCRNH);
    R_Init(W_Get(), &game_state);

    // Création de la map (Map de référence du tutoriel)
    // Cette fonction génère les 16 secteurs et gère les connexions
    map_t game_map = M_CreateMap();
    
    // Initialisation du joueur au point de spawn de la map
    player_t player = P_Init(
        game_map.spawn_point.x,
        game_map.spawn_point.y,
        0,  // Hauteur initiale
        game_map.spawn_angle
    );

    // === MAP DE RÉFÉRENCE ===
    // Reproduction de la map du tutoriel Doom-like Part I
    // 16 secteurs interconnectés avec différentes hauteurs et couleurs.
    
    printf("=== MAP DE RÉFÉRENCE (DOOM-like Part I) ===\n");
    printf("Joueur: rayon=%.1f, hauteur=%.1f\n", PLAYER_RADIUS, PLAYER_HEIGHT);
    printf("Map chargée: 16 secteurs interconnectés (générée par M_CreateMap)\n");
    printf("Position spawn: (%.1f, %.1f, %.1f)\n", player.position.x, player.position.y, player.z);
    printf("==========================================\n");

    GameLoop(&game_state, &player);

    return 0;
}