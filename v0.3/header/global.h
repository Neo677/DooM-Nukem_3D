#ifndef GLOBAL_H
# define GLOBAL_H

# include <math.h>
# include <stdio.h>
# include <memory.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>

#ifdef OS_MAC
# include "../minilibx_opengl_20191021/mlx.h"
#else
# include "../minilibx-linux/mlx.h"
# include "../minilibx-linux/mlx_int.h"
#endif

# define SENSI 0.0005f
# define MOV_SPEED 300
# define ROT_SPEED 5

# define GRAVITY          800.0f
# define JUMP_VELOCITY    350.0f
# define MAX_STEP_HEIGHT  24.0f
# define STEP_SMOOTH_SPEED 150.0f
# define MAX_FALL_VELOCITY 1000.0f

# define SCREENW 800
# define SCREENH 600

typedef struct s_render {
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bpp;
    int line_len;
    int endian;
}       t_render;



#endif