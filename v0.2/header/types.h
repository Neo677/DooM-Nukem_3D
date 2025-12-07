#ifndef TYPES_H
#define TYPES_H

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

#endif