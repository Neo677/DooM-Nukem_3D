#ifndef TYPES_H
#define TYPES_H

#define max_Polyes 10
#define max_Verts 8

typedef struct Vec2
{
    float x;
    float y;
}        Vec2;

typedef struct 
{
    Vec2 p1, p2;
}        lineSeg;

typedef struct
{
    Vec2 vert[max_Verts];
    int vertCnt;
    float height;
    float curDist;
}        polygon;

typedef struct 
{
    Vec2 vert[4];
    float distFromCamera;
    int planeIdInPoly;
}        screenSpacePoly;



#endif