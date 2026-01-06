#ifndef GLOBAL_H
# define GLOBAL_H

# include <math.h>
# include <stdio.h>
# include <memory.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <stdint.h> 
# include <stdbool.h>

#ifdef __APPLE__
# define OS_MAC
# include "../../minilibx_opengl_20191021/mlx.h"
#else
# include "../../minilibx-linux/mlx.h"
#endif

# define SENSI 0.0005f
# define MOV_SPEED 300
# define ROT_SPEED 5
# define GRAVITY          800.0f
# define JUMP_VELOCITY    350.0f
# define MAX_STEP_HEIGHT  24.0f
# define STEP_SMOOTH_SPEED 150.0f
# define MAX_FALL_VELOCITY 1000.0f
// # define SCREENW 800
// # define SCREENH 600

typedef float       f32;
typedef double      f64;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t     i8;
typedef int16_t    i16;
typedef int32_t    i32;
typedef int64_t    i64;
typedef size_t usize;
typedef size_t isize;

# define PI 3.14159265359f
# define TAU (2.0f * PI)
# define PI_2 (PI / 2.0f)
# define PI_4 (PI / 4.0f)
# define DEG2RAD(_d) ((_d) * (PI / 180.0f))
# define RAD2DEG(_d) ((_d) * (180.0f / PI))
# define SCREENH 600
# define SCREENW 800
# define EYE_Z 1.65f // hauteut des yeux du joueuer
# define HFOV DEG2RAD(90.0f) // champ de vision horizontal en radians
# define VFOV 0.5f // facteur du FOV vertical
# define ZNEAR 0.0001f  // avoid dist = 0
# define ZFAR 128.0f // clipping (proche / loin) projection des rayon
# define SECTOR_NONE 0
# define SECTOR_MAX 128 // to adapt after stabilisations

#ifdef OS_MAC
    // macOS keycodes
    # define W 13
    # define A 0
    # define S 1
    # define D 2
    # define LEFT 123
    # define RIGHT 124
    # define PRESS_E 14
    # define V 9
    # define SPACE 49
    # define ESC 53
    # define F1 122
    # define F2 120
    # define F3 99
    # define F4 118
    # define V 9
    # define LSHIFT 56
#else
    // Linux keycodes (default)
    # define W 119
    # define A 97
    # define S 115
    # define D 100
    # define LEFT 65361
    # define RIGHT 65363
    # define PRESS_E 101
    # define V 118
    # define SPACE 32
    # define ESC 65307
    # define F1 65470
    # define F2 65471
    # define F3 65472
    # define F4 65473
    # define V 118
    # define LSHIFT 65505
#endif

// vecteur en decimal (ex (123.45f, 456.78f) = pos du joueuer + direction du regard)
typedef struct s_v2 {
    f32 x;
    f32 y;
}             t_v2;

// vecteur en entier
typedef struct s_v2i {
    i32 x;
    i32 y;
}           t_v2i;

# define v2_2_v2i(_v) ({ __typeof__(_v) __v = (_v); (t_v2i) {__v.x, __v.y}; })
# define v2i_2_v2(_v) ({ __typeof__(_v) __v = (_v); (t_v2) {__v.x, __v.y}; })

# define dot(_v0, _v1) ({ __typeof__(_v0) __v0 = (_v0), __v1 = (_v1); (__v0.x * __v1.x) + (__v0.y * __v1.y)}; )
# define lenght(_v1) ({ __typeof__(_v1) __v1 = (_v1); sqrtf(dot(__v1, __v1));} )
# define normalize(_vn) ( {__typeof__(_vn) __vn = (_vn); const f32 1 = lenght(__vn; (__typeof__(_vn)) {__vn.x / 1, __vn.y / 1})}; )
# define min(_a, _b) ( {__typeof__(_a) __a = (_a), __b = (_b); __a < __b ? __a : __b; } )
# define max(_a, _b) ( {__typeof__(_a) __a = (_a), __b = (_b); __a > __b ? __a : __b; } )
# define clamp(_x, _mi, _ma) (min(max(_x, _mi), _ma))
# define ifnan(_x, _alt) ( {__typeof__(_x) __x = (_x); isnan(__x) ? (_alt) : __x; } )

// -1right, 0 on, 1 left
// # define point_side (_p, _a, _b) ( { __typeof__(_p) __p = (_p), __a = (_a), __b = (_b); -(((__p.x - __a.x) * (__b.y - __a.y)) - ((__p.y - __a.y) * (__b.x - __a.x))); } )

typedef struct s_wall {
    t_v2i   a;
    t_v2i   b;
    u32     color;
    int     portal;
}              t_wall;

typedef struct s_sector {
    int     id;
    usize   firstWall;
    usize   nWalls;
    f32     zFloor;
    f32     zCeil;
    u32     floorCol;
    u32     ceilCol;
}              t_sector;

typedef struct s_camera {
    t_v2    pos;
    f32     angle;
    f32     cosA;
    f32     sinA;
    int     sector;
}              t_camera;

typedef struct s_keys {
    bool up, down, left, right, f1;
}               t_keys;

typedef struct s_render {
    void    *mlx;
    void    *win;
    void    *img;
    u32     *pixels;
    i32     bpp;
    i32     lineLen;
    i32     endian;
}              t_render;

typedef struct s_engine {
    t_render    render;
    t_sector    sectors[SECTOR_MAX];
    usize       nSectors;
    t_wall      walls[512];
    usize       nWalls; 
    t_camera    camera;
    i32         yLo[SCREENW];
    i32         yHi[SCREENW];
    bool        isRunning;
    bool        debugMode;
    t_keys key;
} t_engine;

void present(t_engine *engine);


#endif