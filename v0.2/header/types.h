#ifndef TYPES_H
# define TYPES_H

#define MAX_POLYS 10
#define MAX_VERTS 8

typedef struct Vec2_s
{
    float x;
    float y;
}        Vec2_t;

typedef struct lineSeg_s
{
    Vec2_t p1, p2;
}        lineSeg_t;

typedef struct polygon_s
{
    Vec2_t vert[MAX_VERTS];
    int vertCnt;
    float height;
    float curDist;
    int color;
    int textureId;
}        polygon_t;

typedef struct screenSpacePoly_s
{
    Vec2_t vert[MAX_VERTS];
    int vertCnt;
    float distFromCamera;
    int planeIdInPoly;
}        screenSpacePoly_t;

typedef struct Camera_s
{
    float camAngle;
    float stepWave;
    Vec2_t camPos;
    Vec2_t oldCamPos;
}   Camera_t;

typedef struct color_s
{
    unsigned char R, G, B;
}           color_t;

typedef struct s_skybox {
    void *img;
    char *addr;
    int width;
    int height;
    int bits_per_pixel;
    int line_len;
    int endian;
    int loaded;
}          t_skybox;

typedef enum e_texture_type {
    TEXTURE_WALL,
    TEXTURE_FLOOR,
    TEXTURE_CEILING,
    TEXTURE_LIQUID,
    TEXTURE_SPECIAL,
}       t_texture_type;

typedef struct s_texture {
    void *img;
    char *addr;
    int width;
    int height;
    int bits_per_pixel;
    int line_len;
    int endian;
    int loaded;
    char name[64];
    t_texture_type type;
}   t_texture;

# define MAX_TEXTURES 256

#endif