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
#include <stdbool.h>
#include "types.h"
#include "p_player.h"
#include "g_game_state.h"
#include "u_utils.h"

// Constantes de rendu
#define FOV_ANGLE (M_PI / 3.0)  // 60 degres en radians
#define NEAR_PLANE 0.1
#define FAR_PLANE 1000.0

// Variables globales
extern SDL_Window* window;
extern SDL_Renderer* sdl_renderer;
extern SDL_Texture* screen_texture;
extern SDL_Texture* screen_texture;
extern unsigned int scrnw, scrnh;
extern sectors_store_t map_sectors;

// Fonctions d'initialisation et de nettoyage
void R_Init(SDL_Window *main_win, game_state_t *game_state);
void R_Shutdown();
void R_ShutdownScreen();
void R_InitScreen(int w, int h);

// Fonctions de rendu principales
void R_Render(player_t *player, game_state_t *game_state);
void R_UpdateScreen();

// Fonctions de dessin de base
void R_DrawPoint(int x, int y, unsigned int color);
void R_DrawLine(int x0, int y0, int x1, int y1, unsigned int color);
void R_DrawVerticalLine(int x, int y1, int y2, unsigned int color);
void R_ClearScreenBuffer();
void R_ClearScreenBuffer();
void R_AddSectorToMap(sector_t *sector);

// Helper functions for map creation (exposed for m_map.c)
sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int floor_clr, unsigned int ceil_clr);
wall_t R_CreateWall(int x1, int y1, int x2, int y2);
void R_SectorAddWall(sector_t *sector, wall_t wall);

#endif /* R_RENDERER */