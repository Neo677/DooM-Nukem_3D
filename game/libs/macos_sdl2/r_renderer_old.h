#ifndef R_RENDERER
#define R_RENDERER

#define SDL_MAIN_HANDLED

#ifdef _WIN64
    #include "libs/win_sdl2/include/SDL2/SDL.h"
#elif __APPLE__
    #include <SDL2/SDL.h>
#endif

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "p_player.h"
#include "g_game_state.h"
#include "u_utils.h"

// Constantes du renderer
#define MAX_SECTORS 1024
#define FOV_ANGLE (M_PI / 3.0)  // 60 degrés en radians
#define NEAR_PLANE 0.1
#define FAR_PLANE 1000.0

#define IS_CEIL 1
#define IS_FLOOR 2
#define IS_WALL 0

// Variables globales
extern sectors_queue_t sectors_queue;

// Fonctions d'initialisation et de nettoyage
void R_Init(SDL_Window *main_win, game_state_t *game_state);
void R_Shutdown();

// Fonctions de rendu principales
void R_Render(player_t *player, game_state_t *game_state);
void R_RenderSectors(player_t *player, game_state_t *game_state);

// Fonctions de manipulation des secteurs
sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int ceil_clr, unsigned int floor_clr);
void R_SectorAddWall(sector_t *sector, wall_t vertices);
void R_AddSectorToQueue(sector_t *sector);
wall_t R_CreateWall(int ax, int ay, int bx, int by);
wall_t R_CreatePortal(int ax, int ay, int bx, int by, int th, int bh);

// Fonctions de dessin de base
void R_DrawPoint(int x, int y, unsigned int color);
void R_DrawLine(int x0, int y0, int x1, int y1, unsigned int color);
void R_DrawVerticalLine(int x, int y1, int y2, unsigned int color);
void R_DrawVerticalWall(rquad_t *quad, unsigned int color);

// Fonctions utilitaires
void R_ClearScreenBuffer();
void R_UpdateScreen();
void UpdateLookupTables(rquad_t *quad, sector_t *sector);
void R_RenderFlatsForSector(sector_t *s, player_t *player, double fov, double dist_factor);

#endif /* R_RENDERER */



typedef struct _r_plane
{
    int t[1024];
    int b[1024];
} plane_lut_t;

typedef struct _wall
{
    vec2_t a;
    vec2_t b;
    double portal_top_height;
    double portal_bot_height;
    bool is_portal;
} wall_t;

typedef struct _sector
{
    int id;
    wall_t walls[10];
    int num_walls;
    int height;
    int elevation;
    double dist;
    unsigned int color;
    unsigned int floor_clr;
    unsigned int ceil_clr;

    plane_lut_t portals_floorx_ylut;
    plane_lut_t portals_ceilx_ylut;
    plane_lut_t floorx_ylut;
    plane_lut_t ceilx_ylut;
} sector_t;

typedef struct _sectors_queue
{
    sector_t sectors[1024];
    int num_sectors;
} sectors_queue_t;

sectors_queue_t sectors_queue;

void R_Init(SDL_Window *main_win, game_state_t *game_state);
void R_Shutdown();
void R_Render(player_t *player, game_state_t *game_state);
void R_DrawWalls(player_t *player, game_state_t *game_state);
sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int ceil_clr, unsigned int floor_clr);
void R_SectorAddWall(sector_t *sector, wall_t vertices);
void R_AddSectorToQueue(sector_t *sector);
wall_t R_CreateWall(int ax, int ay, int bx, int by);
wall_t R_CreatePortal(int ax, int ay, int bx, int by, int th, int bh);


#endif /* R_RENDERER */
