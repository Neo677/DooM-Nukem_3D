#include <stdio.h>

#include "p_player.h"
#include "g_game_state.h"
#include "w_window.h"
#include "r_renderer.h"
#include "k_keyboard.h"

#define SCRNW 1024
#define SCRNH 768
#define FPS 120

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
    game_state_t game_state = G_Init(SCRNW, SCRNH, FPS);
    player_t player = P_Init(100, 100, 0, M_PI/2); // Position de spawn au centre
    K_InitKeymap();
    W_Init(SCRNW, SCRNH);
    R_Init(W_Get(), &game_state);

    // === MAP COHÉRENTE AVEC L'ÉCHELLE DU JOUEUR ===
    // Joueur : rayon=5, hauteur=32
    // Secteurs avec des hauteurs logiques pour tester la collision 3D
    
    // Sol principal (hauteur normale d'une pièce)
    sector_t floor_main = R_CreateSector(50, 0, 0x808080, 0xa0a0a0, 0x606060);
    
    // Murs bas (obstacles qu'on peut enjamber)
    sector_t wall_low = R_CreateSector(15, 0, 0xd6382d, 0xf54236, 0x9c2921);
    
    // Murs moyens (hauteur d'homme)
    sector_t wall_medium = R_CreateSector(35, 0, 0x29ba48, 0x43f068, 0x209138);
    
    // Murs hauts (plafond)
    sector_t wall_high = R_CreateSector(60, 0, 0xa3a24b, 0xd9d764, 0x858338);
    
    // NOUVELLES STRUCTURES POUR GRANDES ÉCHELLES
    // Tour très haute (gratte-ciel)
    sector_t tower_tall = R_CreateSector(200, 0, 0x4b0082, 0x6a0dad, 0x301934);
    
    // Montagne (structure géante)
    sector_t mountain = R_CreateSector(500, 0, 0x8b4513, 0xcd853f, 0x654321);
    
    // Plateformes à différentes hauteurs pour escalade
    sector_t platform_low = R_CreateSector(25, 20, 0x4169e1, 0x6495ed, 0x2e4bc7);
    sector_t platform_mid = R_CreateSector(30, 60, 0x32cd32, 0x90ee90, 0x228b22);
    sector_t platform_high = R_CreateSector(35, 120, 0xff6347, 0xff7f50, 0xcd5c5c);
    sector_t platform_sky = R_CreateSector(40, 300, 0x87ceeb, 0xb0e0e6, 0x4682b4);

    // === GÉOMÉTRIE DE LA MAP ===
    
    // Bordures de la map (murs hauts infranchissables)
    int border_north[4*4] = {
        50, 50,   150, 50,   // Nord
        150, 50,  150, 55,
        150, 55,  50, 55,
        50, 55,   50, 50
    };
    
    int border_south[4*4] = {
        50, 145,  150, 145,  // Sud
        150, 145, 150, 150,
        150, 150, 50, 150,
        50, 150,  50, 145
    };
    
    int border_west[4*4] = {
        50, 50,   55, 50,    // Ouest
        55, 50,   55, 150,
        55, 150,  50, 150,
        50, 150,  50, 50
    };
    
    int border_east[4*4] = {
        145, 50,  150, 50,   // Est
        150, 50,  150, 150,
        150, 150, 145, 150,
        145, 150, 145, 50
    };

    // Obstacle bas (peut être enjambé si joueur monte)
    int obstacle_low[4*4] = {
        70, 70,   90, 70,
        90, 70,   90, 90,
        90, 90,   70, 90,
        70, 90,   70, 70
    };

    // Obstacle moyen (hauteur d'homme)
    int obstacle_medium[4*4] = {
        110, 70,  130, 70,
        130, 70,  130, 90,
        130, 90,  110, 90,
        110, 90,  110, 70
    };

    // Plateforme basse (peut monter dessus)
    int platform_low_geom[4*4] = {
        70, 110,  90, 110,
        90, 110,  90, 130,
        90, 130,  70, 130,
        70, 130,  70, 110
    };

    // Plateforme haute
    int platform_high_geom[4*4] = {
        110, 110, 130, 110,
        130, 110, 130, 130,
        130, 130, 110, 130,
        110, 130, 110, 110
    };

    // NOUVELLES GÉOMÉTRIES POUR GRANDES ÉCHELLES
    
    // Tour très haute (gratte-ciel)
    int tower_geom[4*4] = {
        160, 60, 180, 60,
        180, 60, 180, 80,
        180, 80, 160, 80,
        160, 80, 160, 60
    };
    
    // Montagne (structure géante au centre)
    int mountain_geom[4*4] = {
        90, 90, 110, 90,
        110, 90, 110, 110,
        110, 110, 90, 110,
        90, 110, 90, 90
    };
    
    // Plateforme intermédiaire
    int platform_mid_geom[4*4] = {
        160, 110, 180, 110,
        180, 110, 180, 130,
        180, 130, 160, 130,
        160, 130, 160, 110
    };
    
    // Plateforme dans le ciel
    int platform_sky_geom[4*4] = {
        65, 65, 75, 65,
        75, 65, 75, 75,
        75, 75, 65, 75,
        65, 75, 65, 65
    };

    // Couloir étroit (test de glissement)
    int corridor_wall1[4*4] = {
        95, 55,   105, 55,
        105, 55,  105, 65,
        105, 65,  95, 65,
        95, 65,   95, 55
    };
    
    int corridor_wall2[4*4] = {
        95, 135,  105, 135,
        105, 135, 105, 145,
        105, 145, 95, 145,
        95, 145,  95, 135
    };

    // === CONSTRUCTION DES SECTEURS ===
    
    // Bordures (murs hauts)
    for (int i = 0; i < 16; i += 4) {
        wall_t w;
        
        w = R_CreateWall(border_north[i], border_north[i+1], border_north[i+2], border_north[i+3]);
        R_SectorAddWall(&wall_high, w);
        
        w = R_CreateWall(border_south[i], border_south[i+1], border_south[i+2], border_south[i+3]);
        R_SectorAddWall(&wall_high, w);
        
        w = R_CreateWall(border_west[i], border_west[i+1], border_west[i+2], border_west[i+3]);
        R_SectorAddWall(&wall_high, w);
        
        w = R_CreateWall(border_east[i], border_east[i+1], border_east[i+2], border_east[i+3]);
        R_SectorAddWall(&wall_high, w);
    }

    // Obstacles de différentes hauteurs
    for (int i = 0; i < 16; i += 4) {
        wall_t w;
        
        // Obstacle bas
        w = R_CreateWall(obstacle_low[i], obstacle_low[i+1], obstacle_low[i+2], obstacle_low[i+3]);
        R_SectorAddWall(&wall_low, w);
        
        // Obstacle moyen
        w = R_CreateWall(obstacle_medium[i], obstacle_medium[i+1], obstacle_medium[i+2], obstacle_medium[i+3]);
        R_SectorAddWall(&wall_medium, w);
        
        // Plateformes
        w = R_CreateWall(platform_low_geom[i], platform_low_geom[i+1], platform_low_geom[i+2], platform_low_geom[i+3]);
        R_SectorAddWall(&platform_low, w);
        
        w = R_CreateWall(platform_high_geom[i], platform_high_geom[i+1], platform_high_geom[i+2], platform_high_geom[i+3]);
        R_SectorAddWall(&platform_high, w);
        
        // Nouvelles structures hautes
        w = R_CreateWall(tower_geom[i], tower_geom[i+1], tower_geom[i+2], tower_geom[i+3]);
        R_SectorAddWall(&tower_tall, w);
        
        w = R_CreateWall(mountain_geom[i], mountain_geom[i+1], mountain_geom[i+2], mountain_geom[i+3]);
        R_SectorAddWall(&mountain, w);
        
        w = R_CreateWall(platform_mid_geom[i], platform_mid_geom[i+1], platform_mid_geom[i+2], platform_mid_geom[i+3]);
        R_SectorAddWall(&platform_mid, w);
        
        w = R_CreateWall(platform_sky_geom[i], platform_sky_geom[i+1], platform_sky_geom[i+2], platform_sky_geom[i+3]);
        R_SectorAddWall(&platform_sky, w);
        
        // Couloir
        w = R_CreateWall(corridor_wall1[i], corridor_wall1[i+1], corridor_wall1[i+2], corridor_wall1[i+3]);
        R_SectorAddWall(&wall_medium, w);
        
        w = R_CreateWall(corridor_wall2[i], corridor_wall2[i+1], corridor_wall2[i+2], corridor_wall2[i+3]);
        R_SectorAddWall(&wall_medium, w);
    }

    // Ajouter tous les secteurs à la queue de rendu
    R_AddSectorToQueue(&floor_main);
    R_AddSectorToQueue(&wall_low);
    R_AddSectorToQueue(&wall_medium);
    R_AddSectorToQueue(&wall_high);
    R_AddSectorToQueue(&platform_low);
    R_AddSectorToQueue(&platform_mid);
    R_AddSectorToQueue(&platform_high);
    R_AddSectorToQueue(&platform_sky);
    R_AddSectorToQueue(&tower_tall);
    R_AddSectorToQueue(&mountain);

    printf("=== MAP CRÉÉE AVEC GRANDE ÉCHELLE ===\n");
    printf("Joueur: rayon=%.1f, hauteur=%.1f\n", PLAYER_RADIUS, PLAYER_HEIGHT);
    printf("Limites de hauteur: z ∈ [-200, 1000]\n");
    printf("Obstacles:\n");
    printf("- Murs bas: hauteur=15 (enjambable à z>15)\n");
    printf("- Murs moyens: hauteur=35 (enjambable à z>35)\n");
    printf("- Murs hauts: hauteur=60 (enjambable à z>60)\n");
    printf("- Tour: hauteur=200 (enjambable à z>200)\n");
    printf("- Montagne: hauteur=500 (enjambable à z>500)\n");
    printf("Plateformes d'escalade:\n");
    printf("- Basse: z=20, hauteur=25 (total=45)\n");
    printf("- Moyenne: z=60, hauteur=30 (total=90)\n");
    printf("- Haute: z=120, hauteur=35 (total=155)\n");
    printf("- Ciel: z=300, hauteur=40 (total=340)\n");
    printf("Position spawn: (100, 100, 0)\n");
    printf("=====================================\n");

    GameLoop(&game_state, &player);

    return 0;
}