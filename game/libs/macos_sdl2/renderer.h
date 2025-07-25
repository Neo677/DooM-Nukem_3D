#ifndef RENDERER_H
# define RENDERER_H

# define SDL_MAIN_HANDLED

# ifdef _WIN64
    #include "libs/win_sdl2/include/SDL2/SDL.h"
# elif __APPLE__
    #include "SDL2/SDL.h"
#endif

# include <stdio.h>
# include "typedefs.h"
# include "player.h"
# include "game_state.h"
# include "utils.h"

typedef struct _r_plane
{
    int t[1024];
    int b[1024];
      
}   plane_lut_t;

typedef struct _wall
{
    vec2_t a;
    vec2_t b;
    double portal_top_height;
    double portal_bot_height;
    bool is_portal;
}   wall_t;

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
    plane_lut_t portals_floor_ylut;
    plane_lut_t portals_ceilx_ylut;
    plane_lut_t floor_ylut;
    plane_lut_t ceil_ylut;
}   sector_t;

typedef struct _sector_queue
{
    sector_t sectors[1024];
    int num_sectors; 
} sector_queue_t;

void R_Init(SDL_Window *main_win, game_state_t *game_state);
void R_Shutdown();
void R_Render(player_t *player, game_state_t *game_state);
void R_DrawWalls(player_t *player, game_state_t *game_state);
sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int ceil_clr, unsigned int floor_clr);
void R_SectorAddWall(sector_t *sector, wall_t vertices);
void R_AddSectorToQueue(sector_t *sector);
wall_t R_CreateWWall(int ax, int ay, int bx, int by);
wall_t R_CreatePortal(int ax, int ay, int bx, int by, int th, int bh);


#endif