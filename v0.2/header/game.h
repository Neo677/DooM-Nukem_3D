#ifndef GAME_H
# define GAME_H

# include <math.h>
# include <stdio.h>
# include <memory.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include "types.h"
# include "collision_vector.h"

#ifdef OS_MAC
#include "../minilibx_opengl_20191021/mlx.h"
#else
#include "../minilibx-linux/mlx.h"
#include "../minilibx-linux/mlx_int.h"
#endif


# define SENSI 0.0005f
# define MOV_SPEED 300
# define ROT_SPEED 5
# define RES_DIV 3
# define CAM_RADIUS 20.0f;
# define POL_RES 1.025
# define SHOULD_RASTERIZE 1
# define WWAVE_MAG 1
# define SLY_COLOR 0x4DB5FF

// #define screenW 1800 / RES_DIV
// #define screenH 1600 / RES_DIV
# define screenW 800
# define screenH 600

typedef struct s_texture_manager {
    t_texture textures[MAX_TEXTURES];
    int count;
    void *mlx;
}       t_texture_manager;

typedef struct s_global {
    Camera_t cam;
    polygon_t polys[MAX_POLYS];
    Vec2_t vert[MAX_VERTS];
    int g_keys[65536];
    int screenSpaceVisiblePlanes;
    screenSpacePoly_t screenSpacePoly[MAX_POLYS][MAX_VERTS];
    double lastTime;
    t_skybox skybox;
    t_texture_manager tex_manager;
    int polyOrder[MAX_POLYS];
    float polyDistances[MAX_POLYS];

    t_sector sectors[MAX_SECTORS];
    t_wall walls[MAX_WALLS];
    int sectorCount;
    int wallCount;
    int currentSectorId;
}           t_global;

extern t_global global;

typedef struct s_render {
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_len;
    int endian;
}     t_render;

#include "key.h"

#endif