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
    if (lineLen < 0.0001f)
        return line.p1;
    float lineLenSq = lineLen * lineLen;
    float dot = (((point.x - line.p1.x) * (line.p2.x - line.p1.x)) + ((point.y - line.p1.y) * (line.p2.y - line.p1.y))) / lineLenSq;

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

// Vérifie si le joueur peut passer d'un secteur à un autre (step-up/down)
static int canTransitionToSector(int fromSectorId, int toSectorId)
{
    if (fromSectorId < 0 || toSectorId < 0)
        return (0);
    
    t_sector *fromSector = &global.sectors[fromSectorId];
    t_sector *toSector = &global.sectors[toSectorId];
    
    float currentFootZ = global.cam.footZ;
    float newFloorZ = toSector->floorHeight;
    float heightDiff = newFloorZ - currentFootZ;
    
    // Vérifier le headroom (espace pour le joueur dans le nouveau secteur)
    float headroom = toSector->ceilingHeight - toSector->floorHeight;
    if (headroom < EYE_HEIGHT + 5.0f)
        return (0);  // Pas assez d'espace
    
    // CAS 1: Step-up (montée)
    if (heightDiff > 0)
    {
        if (heightDiff <= MAX_STEP_HEIGHT)
        {
            // Step-up automatique autorisé
            // Mettre à jour targetZ pour transition smooth
            global.cam.targetZ = newFloorZ + EYE_HEIGHT;
            return (1);
        }
        else
        {
            // Mur trop haut - BLOCAGE
            return (0);
        }
    }
    
    // CAS 2: Step-down (descente)
    if (heightDiff < 0)
    {
        if (fabs(heightDiff) <= MAX_STEP_HEIGHT)
        {
            // Petite descente - transition smooth
            global.cam.targetZ = newFloorZ + EYE_HEIGHT;
            return (1);
        }
        else
        {
            // Grande descente - le joueur va tomber (gravité)
            // Permettre le passage, la gravité fera le reste
            global.cam.onGround = 0;
            return (1);
        }
    }
    
    // CAS 3: Même hauteur
    return (1);
    
    (void)fromSector;
}

Vec2_t resolveCollision(Vec2_t newPos)
{
    int targetSectorId = findSectorContainingPoint(newPos);
    int oldSectorId = global.currentSectorId;

    // Si pas de secteur valide, bloquer
    if (targetSectorId < 0)
        return (global.cam.camPos);

    // Vérifier si on change de secteur
    if (targetSectorId != oldSectorId && oldSectorId >= 0)
    {
        // Vérifier si la transition est possible (step-up/down)
        if (!canTransitionToSector(oldSectorId, targetSectorId))
        {
            // Transition bloquée (mur trop haut ou pas assez d'espace)
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

            // Pour les portails, vérifier si on peut traverser (step-up/down)
            if (wall->isPortal && wall->backSectorId >= 0)
            {
                // Le portail est traversable si canTransitionToSector retourne vrai
                // (déjà vérifié plus haut lors du changement de secteur)
                continue;
            }

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

                if (dist > 0.001f)
                {
                    toPoint.x /= dist;
                    toPoint.y /= dist;
                    float pushAmount = CAMERA_RADIUS - dist + 0.1f;
                    totalPush.x += toPoint.x * pushAmount;
                    totalPush.y += toPoint.y * pushAmount;
                }
                else
                {
                    totalPush.x += CAMERA_RADIUS + 0.1f;
                }
            }
        }

        if (collisionCount == 0)
            break;

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