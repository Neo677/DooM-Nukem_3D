#include "../header/game.h"

int createWall(Vec2_t p1, Vec2_t p2, int frontSectorId)
{
    int wallId;
    t_wall *wall;

    if (global.wallCount >= MAX_WALLS)
        return (-1);
    wallId = global.wallCount;
    wall = &global.walls[wallId];
    wall->id = wallId;
    wall->p1 = p1;
    wall->p2 = p2;
    wall->frontSectorId = frontSectorId;
    wall->backSectorId = -1;
    wall->middleTextureId = 0;
    wall->upperTextureId = -1;
    wall->lowerTextureId = -1;
    wall->isPortal = 0;
    wall->twoSided = 0;
    global.wallCount++;
    return (wallId);
}

// Map data encodes heights in micro-units (e.g., 60,000,000 -> 60.0). Scale back to world units.
#define MAP_HEIGHT_SCALE 1000000.0f

void convertPolysToSectors(void)
{
    int i;
    int j;
    t_sector *sector;
    Vec2_t p1;
    Vec2_t p2;
    int wallId;

    global.sectorCount = 0;
    global.wallCount = 0;
    i = 0;
    while (i < MAX_POLYS)
    {
        if (global.polys[i].vertCnt < 3)
        {
            i++;
            continue;
        }
        sector = &global.sectors[global.sectorCount];
        sector->id = global.sectorCount;
        sector->floorHeight = 0.0f;
        sector->ceilingHeight = global.polys[i].height / MAP_HEIGHT_SCALE;
        sector->floorTextureId = -1;
        sector->ceilingTextureId = -1;
        sector->lightLevel = 255;
        sector->wallCount = global.polys[i].vertCnt;
        sector->visited = 0;
        j = 0;
        while (j < global.polys[i].vertCnt)
        {
            p1 = global.polys[i].vert[j];
            p2 = global.polys[i].vert[(j + 1) % global.polys[i].vertCnt];
            wallId = createWall(p1, p2, global.sectorCount);
            if (wallId >= 0)
            {
                sector->wallIds[j] = wallId;
                global.walls[wallId].middleTextureId = global.polys[i].textureId;
            }
            j++;
        }
        global.sectorCount++;
        i++;
    }
}

int wallsAreConnected(t_wall *w1, t_wall *w2)
{
    float epsilon;
    float dist_p1_p2;
    float dist_p2_p1;
    float dist_p1_p1;
    float dist_p2_p2;

    epsilon = 1.0f;
    dist_p1_p2 = len(w1->p1, w2->p2);
    dist_p2_p1 = len(w1->p2, w2->p1);
    if (dist_p1_p2 < epsilon && dist_p2_p1 < epsilon)
        return (1);
    dist_p1_p1 = len(w1->p1, w2->p1);
    dist_p2_p2 = len(w1->p2, w2->p2);
    if (dist_p1_p1 < epsilon && dist_p2_p2 < epsilon)
        return (1);
    return (0);
}

void detectPortals(void)
{
    int portalCount;
    int i;
    int j;

    portalCount = 0;
    i = 0;
    while (i < global.wallCount)
    {
        j = i + 1;
        while (j < global.wallCount)
        {
            if (wallsAreConnected(&global.walls[i], &global.walls[j]))
            {
                global.walls[i].backSectorId = global.walls[j].frontSectorId;
                global.walls[j].backSectorId = global.walls[i].frontSectorId;
                global.walls[i].isPortal = 1;
                global.walls[j].isPortal = 1;
                global.walls[i].twoSided = 1;
                global.walls[j].twoSided = 1;
                global.walls[i].upperTextureId = global.walls[i].middleTextureId;
                global.walls[i].lowerTextureId = global.walls[i].middleTextureId;
                global.walls[j].upperTextureId = global.walls[j].middleTextureId;
                global.walls[j].lowerTextureId = global.walls[j].middleTextureId;
                global.walls[i].middleTextureId = -1;
                global.walls[j].middleTextureId = -1;
                portalCount++;
            }
            j++;
        }
        i++;
    }
    (void)portalCount;
}

void sortPolysByDeph(void)
{
    int i;
    int j;
    float centerX;
    float centerY;
    float dx;
    float dy;
    int tmp;

    i = 0;
    while (i < MAX_POLYS)
    {
        global.polyOrder[i] = i;
        i++;
    }
    i = 0;
    while (i < MAX_POLYS)
    {
        if (global.polys[i].vertCnt < 2)
        {
            global.polyDistances[i] = -1.0f;
            i++;
            continue;
        }
        centerX = 0;
        centerY = 0;
        j = 0;
        while (j < global.polys[i].vertCnt)
        {
            centerX += global.polys[i].vert[j].x;
            centerY += global.polys[i].vert[j].y;
            j++;
        }
        centerX /= global.polys[i].vertCnt;
        centerY /= global.polys[i].vertCnt;
        dx = centerX - global.cam.camPos.x;
        dy = centerY - global.cam.camPos.y;
        global.polyDistances[i] = sqrt(dx * dx + dy * dy);
        global.polys[i].curDist = global.polyDistances[i];
        i++;
    }
    i = 0;
    while (i < MAX_POLYS - 1)
    {
        j = i + 1;
        while (j < MAX_POLYS)
        {
            if (global.polyDistances[global.polyOrder[i]] < global.polyDistances[global.polyOrder[j]])
            {
                tmp = global.polyOrder[i];
                global.polyOrder[i] = global.polyOrder[j];
                global.polyOrder[j] = tmp;
            }
            j++;
        }
        i++;
    }
}
