#ifndef UTILS_H
# define UTILS_H

# include <SDL2/SDL.h>
# include <stdlib.h>
# include <stdio.h>
# include <math.h>

typedef struct s_point {
    int x;
    int y;
}   t_point;

typedef struct {
    double x;
    double y;
}   t_v2;

typedef struct {
    double x;
    double y;
    double z;
}           t_v3;

typedef struct s_rectangle {
    Uint32 inside_color;
    Uint32 line_color;
    int filled;
    int line_size;
}             t_rectangle;

typedef struct s_circle {
    Uint32 color;
    Uint32 line_color;
    t_point center;
    int radius;
}       t_circle;


// helpers
static inline t_point new_point(int x, int y) {
    return (t_point){x, y};
}

static inline t_v2 new_v2(int x, int y) {
    return (t_v2){x, y};
}

static inline t_v3 new_v3(double x, double y, double z) {
    return (t_v3){x, y, z};
}

static inline t_circle new_circle(Uint32 line_color, Uint32 color, t_point center, int radius) {
    return (t_circle){color, line_color, center, radius};
}

#endif