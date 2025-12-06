#ifndef GAME_H
#define GAME_H


#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "../minilibx-linux/mlx.h"

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

// // Linux
# define W 119
# define A 97
# define S 115
# define D 100
# define LEFT 65361
# define RIGHT 65363
# define PRESS_E 101
# define ESC 65307

#define screenW 800
#define screenH 600

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