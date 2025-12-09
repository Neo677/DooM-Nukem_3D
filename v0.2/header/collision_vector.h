#ifndef COLLISION_VECTOR_H
# define COLLISION_VECTOR_H

# include "types.h"
# include <math.h>

#define CAMERA_RADIUS 20.0f

typedef struct s_global t_global;
extern t_global global;

float   dotPoints(float x1, float y1, float x2, float y2);
float   ft_dot(Vec2_t pointA, Vec2_t pointB);
float   len_vec(Vec2_t pointA, Vec2_t pointB);
Vec2_t  normalize(Vec2_t vec);
Vec2_t  vecMinus(Vec2_t v1, Vec2_t v2);
Vec2_t  vecPlus(Vec2_t v1, Vec2_t v2);
Vec2_t  vecMulF(Vec2_t v1, float val);
Vec2_t  resolveCollision(Vec2_t newPos);
Vec2_t  closestPointOnLine(lineSeg_t line, Vec2_t point);
int     isPointOnLine(lineSeg_t line, Vec2_t point);
int     lineCircleCollision(lineSeg_t line, Vec2_t circleCenter, float circleRadius);
int     checkCollision(Vec2_t newPos);

#endif