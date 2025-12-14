#include "../header/game.h"

float cross2dpoints(float x1, float y1, float x2, float y2)
{
    return (x1 * y2 - y1 * x2);
}

Vec2_t intersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    Vec2_t p;
    float det;
    float temp1;
    float temp2;

    det = cross2dpoints(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    if (fabsf(det) < 1e-6f)
    {
        p.x = NAN;
        p.y = NAN;
        return (p);
    }
    temp1 = cross2dpoints(x1, y1, x2, y2);
    temp2 = cross2dpoints(x3, y3, x4, y4);
    p.x = cross2dpoints(temp1, x1 - x2, temp2, x3 - x4) / det;
    p.y = cross2dpoints(temp1, y1 - y2, temp2, y3 - y4) / det;
    return (p);
}

float len(Vec2_t pointA, Vec2_t pointB)
{
    float distY;
    float distX;

    distY = pointB.y - pointA.y;
    distX = pointB.x - pointA.x;
    return (sqrt(distX * distX + distY * distY));
}

float closestVertexInPoly(polygon_t poly, Vec2_t pos)
{
    float dist;
    int i;
    float d;

    dist = 9999999;
    i = 0;
    while (i < poly.vertCnt)
    {
        d = len(pos, poly.vert[i]);
        if (d < dist)
            dist = d;
        i++;
    }
    return (dist);
}
