#ifndef GLOBAL_H
# define GLOBAL_H

# include <math.h>
# include <stdio.h>
# include <memory.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <math.h>

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
# define TAU (2.0f  PI)
# define PI_2 (PI / 2.0f)
# define PI_4 (PI / 4.0f)
# define DEG2RAD(_d) ((_d) * (PI / 180.0f))
# define RAD2DEG(_d) ((_d) * (180.0f / PI))
# define SCREENH 220
# define SCREENW 380
# define EYE_Z 1.65f // hauteut des yeux du joueuer
# define HFOV DEG2RAD(90.0f) // champ de vision horizontal en radians
# define VFOV 0.5f // facteur du FOV vertical
# define ZNEAR 0.0001f  // avoid dist = 0
# define ZFAR 128.0f // clipping (proche / loin) projection des rayon

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
# define clamp(_x. _mi, _ma) (min(max(_x, _mi), _ma))
# define ifnan(_x, _alt) ( {__typeof__(_x) __x = (_x); isnan(__x) ? (_alt) : __x; } )

# define point_side 

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