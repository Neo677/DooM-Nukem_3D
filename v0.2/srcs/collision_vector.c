#include "../header/collision_vector.h"


float dotPoints(float x1, float y1, float x2, float y2)
{
    return (x1 * x2 + y1 * y2);
}

float ft_dot(Vec2_t pointA, Vec2_t pointB)
{
    return (dotPoints(pointA.x, pointA.y, pointB.x, pointB.y));
}

Vec2_t normalize(Vec2_t vec)
{
    float len = sqrt((vec.x * vec.x) + (vec.y * vec.y));
    Vec2_t normalize;
    normalize.x = vec.x / len;
    normalize.y = vec.y / len;

    return (normalize);
}

Vec2_t vecMinus(Vec2_t v1, Vec2_t v2)
{
    Vec2_t v3;
    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;

    return (v3);
}

Vec2_t vecPlus(Vec2_t v1, Vec2_t v2)
{
    Vec2_t v3;
    v3.x = v1.x + v2.x;
    v3.y = v1.y + v2.y;

    return (v3);
}

Vec2_t vecMulF(Vec2_t v1, float val)
{
    Vec2_t v2;
    v2.x = v1.x * val;
    v2.y = v1.y * val;
    return (v2);
}

float len_vec(Vec2_t pointA, Vec2_t pointB)
{
    float distX = pointB.x - pointA.x;
    float distY = pointB.y - pointA.y;

    return (sqrt(distX * distX + distY * distY));
}

Vec2_t closestPointOnLine(lineSeg_t line, Vec2_t point)
{
    float lineLen = len_vec(line.p1, line.p2);
    
    // Check for degenerate line (point) - avoid division by zero
    if (lineLen < 0.0001f)
        return line.p1;
    
    float lineLenSq = lineLen * lineLen;
    float dot = (((point.x - line.p1.x) * (line.p2.x - line.p1.x)) + 
                 ((point.y - line.p1.y) * (line.p2.y - line.p1.y))) / lineLenSq;

    if (dot > 1) {
        dot = 1;
    } else if (dot < 0) {
        dot = 0;
    }
    Vec2_t closestPoint;
    closestPoint.x = line.p1.x + (dot * (line.p2.x - line.p1.x));
    closestPoint.y = line.p1.y + (dot * (line.p2.y - line.p1.y));
    return (closestPoint);
}

int isPointOnLine(lineSeg_t line, Vec2_t point)
{
    float lineLen = len_vec(line.p1, line.p2);
    float pointDist1 = len_vec(point, line.p1);
    float pointDist2 = len_vec(point, line.p2);
    float resolution = 0.1f;
    float lineLenMarginHigh = lineLen + resolution;
    float lineLenMarginLow = lineLen - resolution;
    float distFromLineEnds = pointDist1 + pointDist2;

    if (distFromLineEnds >= lineLenMarginLow && distFromLineEnds <= lineLenMarginHigh)
        return (1);
    return (0);
}

int lineCircleCollision(lineSeg_t line, Vec2_t circleCenter, float circleRadius)
{
    Vec2_t closestPointToLine = closestPointOnLine(line, circleCenter);
    float circleToPointOnLineDist = len_vec(closestPointToLine, circleCenter);

    if (circleToPointOnLineDist < circleRadius)
        return (1);
    return (0);
}

Vec2_t resolveCollision(Vec2_t newPos)
{
    int iterations = 0;
    int maxIterations = 3;
    
    while (iterations < maxIterations)
    {
        Vec2_t totalPush = {0, 0};
        int collisionCount = 0;
        
        // Accumulate ALL collisions, not just the closest
        for (int polyIdx = 0; polyIdx < MAX_POLYS; polyIdx++)
        {
            if (polys[polyIdx].vertCnt < 2)
                continue;
            
            for (int i = 0; i < polys[polyIdx].vertCnt; i++)
            {
                lineSeg_t wall;
                wall.p1 = polys[polyIdx].vert[i];
                wall.p2 = polys[polyIdx].vert[(i + 1) % polys[polyIdx].vertCnt];
                
                Vec2_t closestPoint = closestPointOnLine(wall, newPos);
                Vec2_t toPoint = vecMinus(newPos, closestPoint);
                float distSq = toPoint.x * toPoint.x + toPoint.y * toPoint.y;
                
                // Check if colliding with this wall
                if (distSq < CAMERA_RADIUS * CAMERA_RADIUS)
                {
                    collisionCount++;
                    float dist = sqrt(distSq);
                    
                    if (dist > 0.001f)
                    {
                        // Normalize direction
                        toPoint.x /= dist;
                        toPoint.y /= dist;
                        // Calculate push amount
                        float pushAmount = CAMERA_RADIUS - dist + 0.1f;
                        // Accumulate push vectors
                        totalPush.x += toPoint.x * pushAmount;
                        totalPush.y += toPoint.y * pushAmount;
                    }
                    else
                    {
                        // Player is exactly on the wall - push in arbitrary direction
                        totalPush.x += CAMERA_RADIUS + 0.1f;
                    }
                }
            }
        }
        
        if (collisionCount == 0)
            break;
        
        // Apply accumulated push
        newPos.x += totalPush.x;
        newPos.y += totalPush.y;
        
        iterations++;
    }
    
    return newPos;
}

int checkCollision(Vec2_t newPos)
{
    for (int polyIdx = 0; polyIdx < MAX_POLYS; polyIdx++) {
        if (polys[polyIdx].vertCnt < 2) 
            continue;
        for (int i = 0; i < polys[polyIdx].vertCnt; i++) {
            lineSeg_t wall;
            wall.p1 = polys[polyIdx].vert[i];
            wall.p2 = polys[polyIdx].vert[(i + 1) % polys[polyIdx].vertCnt];

            if (lineCircleCollision(wall, newPos, CAMERA_RADIUS))
                return (1);
        }
    }
    return (0);
}