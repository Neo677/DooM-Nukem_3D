#ifndef GAME_H
#define GAME_H


#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include "../minilibx-linux/mlx.h"
#include "../minilibx-linux/mlx_int.h"

// ----------------- KEY DEFINES -----------------
// Mac
// # define W 13
// # define A 0
// # define S 1
// # define D 2
// # define LEFT 123
// # define RIGHT 124
// # define ESCP 53

# define SENSI 0.0005f

# define MOV_SPEED 100
# define ROT_SPEED 1

// // Linux
# define KEY_W 119
# define KEY_A 97
# define KEY_S 115
# define KEY_D 100
# define KEY_LEFT 65361
# define KEY_RIGHT 65363
# define KEY_PRESS_E 101
# define KEY_ESC 65307

#define screenW 800
#define screenH 600

#define Should_Rasterize 1
#define Raster_resoltion 4
#define Rastere_Num_Verts 4

#define WWAVE_MAG 1
#define SLY_COLOR 0x4DB5FF



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