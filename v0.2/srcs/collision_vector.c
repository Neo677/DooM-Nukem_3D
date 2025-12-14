#include "../header/game.h"
#include "../header/collision_vector.h"
#include "../header/rasterize.h"


float dotPoints(float x1, float y1, float x2, float y2) {
    return (x1 * x2 + y1 * y2);
}

float ft_dot(Vec2_t pointA, Vec2_t pointB) {
    return (dotPoints(pointA.x, pointA.y, pointB.x, pointB.y));
}

Vec2_t normalize(Vec2_t vec)
{
    float len = sqrt((vec.x * vec.x) + (vec.y * vec.y));
    Vec2_t normalize;
    
    if (len < 1e-6f)
    {
        normalize.x = 0.0f;
        normalize.y = 0.0f;
        return (normalize);
    }
    
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
    float dx = line.p2.x - line.p1.x;
    float dy = line.p2.y - line.p1.y;
    float lineLenSq = dx * dx + dy * dy;
    
    if (lineLenSq < 1e-8f)
        return line.p1;
    
    float dot = (((point.x - line.p1.x) * dx) + ((point.y - line.p1.y) * dy)) / lineLenSq;

    if (dot > 1.0f) {
        dot = 1.0f;
    } else if (dot < 0.0f) {
        dot = 0.0f;
    }
    Vec2_t closestPoint;
    closestPoint.x = line.p1.x + (dot * dx);
    closestPoint.y = line.p1.y + (dot * dy);
    return (closestPoint);
}

int pointInSector(t_sector *sector, Vec2_t point)
{
    float vx[MAX_VERTS];
    float vy[MAX_VERTS];

    for (int i = 0; i < sector->wallCount; i++) {
        t_wall *wall = &global.walls[sector->wallIds[i]];
        vx[i] = wall->p1.x;
        vy[i] = wall->p1.y;
    }
    return (pointInPoly(sector->wallCount, vx, vy, point.x, point.y));
}

int findSectorContainingPoint(Vec2_t point)
{
    for (int i = 0; i < global.sectorCount; i++) {
        if (pointInSector(&global.sectors[i], point))
            return (i);
    }
    return (-1);
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

static int canTransitionToSector(int fromSectorId, int toSectorId)
{
    if (fromSectorId < 0 || toSectorId < 0)
        return (0);
    
    t_sector *fromSector = &global.sectors[fromSectorId];
    t_sector *toSector = &global.sectors[toSectorId];
    
    float currentFootZ = global.cam.footZ;
    float newFloorZ = toSector->floorHeight;
    float heightDiff = newFloorZ - currentFootZ;
    
    float headroom = toSector->ceilingHeight - toSector->floorHeight;
    
    if (headroom < EYE_HEIGHT + 5.0f)
        return (0);
    if (heightDiff > 0)
    {
        if (heightDiff <= MAX_STEP_HEIGHT)
        {
            if (!global.cam.onGround)
                return (1);
            
            global.cam.targetZ = newFloorZ + EYE_HEIGHT;
            return (1);
        }
        else
        {
            return (0);
        }
    }
    
    if (heightDiff < 0)
    {
        if (fabs(heightDiff) <= MAX_STEP_HEIGHT)
        {
            if (!global.cam.onGround)
                return (1);
            
            global.cam.targetZ = newFloorZ + EYE_HEIGHT;
            return (1);
        }
        else
        {
            global.cam.onGround = 0;
            return (1);
        }
    }
    
    return (1);
    
    (void)fromSector;
}

Vec2_t resolveCollision(Vec2_t newPos)
{
    int targetSectorId = findSectorContainingPoint(newPos);
    int oldSectorId = global.currentSectorId;

    if (targetSectorId < 0)
        return (global.cam.camPos);

    if (targetSectorId != oldSectorId && oldSectorId >= 0)
    {
        if (!canTransitionToSector(oldSectorId, targetSectorId))
        {
            return (global.cam.camPos);
        }
    }

    global.currentSectorId = targetSectorId;

    int iterations = 0;
    int maxIterations = 3;

    while (iterations < maxIterations)
    {
        Vec2_t totalPush = (Vec2_t){0, 0};
        int collisionCount = 0;
        t_sector *sector = &global.sectors[targetSectorId];

        for (int i = 0; i < sector->wallCount; i++)
        {
            t_wall *wall = &global.walls[sector->wallIds[i]];
            int isTraversable = 0;

            if (wall->isPortal && wall->backSectorId >= 0)
            {
                if (canTransitionToSector(targetSectorId, wall->backSectorId))
                {
                    isTraversable = 1;
                }
            }
            
            if (isTraversable)
                continue;

            lineSeg_t wallSeg;
            wallSeg.p1 = wall->p1;
            wallSeg.p2 = wall->p2;

            Vec2_t closestPoint = closestPointOnLine(wallSeg, newPos);
            Vec2_t toPoint = vecMinus(newPos, closestPoint);
            float distSq = toPoint.x * toPoint.x + toPoint.y * toPoint.y;

            if (distSq < CAMERA_RADIUS * CAMERA_RADIUS)
            {
                collisionCount++;
                float dist = sqrt(distSq);
                float pushAmount = CAMERA_RADIUS - dist + 0.1f;

                if (dist > 1e-4f)
                {
                    toPoint.x /= dist;
                    toPoint.y /= dist;
                    totalPush.x += toPoint.x * pushAmount;
                    totalPush.y += toPoint.y * pushAmount;
                }
                else
                {
                    Vec2_t wallVec = vecMinus(wallSeg.p2, wallSeg.p1);
                    float wallLen = sqrt(wallVec.x * wallVec.x + wallVec.y * wallVec.y);
                    
                    if (wallLen > 1e-4f)
                    {
                        Vec2_t normal;
                        normal.x = -wallVec.y / wallLen;
                        normal.y = wallVec.x / wallLen;
                        
                        totalPush.x += normal.x * pushAmount;
                        totalPush.y += normal.y * pushAmount;
                    }
                    else
                    {
                        totalPush.x += pushAmount;
                    }
                }
            }
        }

        if (collisionCount == 0)
            break;

        float pushMagnitude = sqrt(totalPush.x * totalPush.x + totalPush.y * totalPush.y);
        float maxPush = CAMERA_RADIUS * 2.0f;
        if (pushMagnitude > maxPush)
        {
            totalPush.x *= maxPush / pushMagnitude;
            totalPush.y *= maxPush / pushMagnitude;
        }
        
        newPos.x += totalPush.x;
        newPos.y += totalPush.y;
        iterations++;
    }

    return (newPos);
}

int checkCollision(Vec2_t newPos)
{
    for (int polyIdx = 0; polyIdx < MAX_POLYS; polyIdx++) {
        if (global.polys[polyIdx].vertCnt < 2) 
            continue;
        for (int i = 0; i < global.polys[polyIdx].vertCnt; i++) {
            lineSeg_t wall;
            wall.p1 = global.polys[polyIdx].vert[i];
            wall.p2 = global.polys[polyIdx].vert[(i + 1) % global.polys[polyIdx].vertCnt];
            if (lineCircleCollision(wall, newPos, CAMERA_RADIUS))
                return (1);
        }
    }
    return (0);
}