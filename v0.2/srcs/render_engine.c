#include "../header/game.h"
#include "../header/rasterize.h"
#include "../header/ceiling.h"

#define NEAR_PLANE 0.1f
#define RENDER_CEILING (1 << 0)
#define RENDER_WALL    (1 << 1)
#define RENDER_FLOOR   (1 << 2)

void renderGround(t_render *render)
{
    float waveVal;
    int starty;
    int y;
    float rowDistance;
    int x;
    float darkness;
    int intensity;
    int color;

    waveVal = WWAVE_MAG * sinf(global.cam.stepWave);
    starty = screenH / 2 + (int)waveVal;
    if (starty < 0)
        starty = 0;
    if (starty >= screenH)
        starty = screenH - 1;
    y = starty;
    while (y < screenH)
    {
        rowDistance = (screenH / 2.0f) / (y - screenH / 2.0f + 0.1f);
        x = 0;
        while (x < screenW)
        {
            darkness = 1.0f - (rowDistance / 25.0f);
            if (darkness < 0.1f) darkness = 0.1f;
            if (darkness > 0.4f) darkness = 0.4f;
            intensity = (int)(40 * darkness);
            color = (intensity << 16) | (intensity << 8) | intensity;
            putPixel(render, x, y, color);
            x++;
        }
        y++;
    }
}

int clipWallToNearPlane(Vec2_t *p1, Vec2_t *p2, float *z1, float *z2, float *u1, float *u2)
{
    float dx1;
    float dy1;
    float dx2;
    float dy2;
    float zCam1;
    float zCam2;
    float t;
    float u1_orig;
    float u2_orig;

    u1_orig = *u1;
    u2_orig = *u2;
    dx1 = p1->x - global.cam.camPos.x;
    dy1 = p1->y - global.cam.camPos.y;
    dx2 = p2->x - global.cam.camPos.x;
    dy2 = p2->y - global.cam.camPos.y;
    zCam1 = dx1 * cosf(global.cam.camAngle) + dy1 * sinf(global.cam.camAngle);
    zCam2 = dx2 * cosf(global.cam.camAngle) + dy2 * sinf(global.cam.camAngle);
    if (zCam1 <= NEAR_PLANE && zCam2 <= NEAR_PLANE)
        return (0);
    if (zCam1 <= NEAR_PLANE)
    {
        t = (NEAR_PLANE - zCam1) / (zCam2 - zCam1 + 0.0001f);
        if (t > 0.0f && t < 1.0f)
        {
            p1->x = p1->x + t * (p2->x - p1->x);
            p1->y = p1->y + t * (p2->y - p1->y);
            *z1 = NEAR_PLANE;
            *u1 = u1_orig + t * (u2_orig - u1_orig);
        }
    }
    else if (zCam2 <= NEAR_PLANE)
    {
        t = (NEAR_PLANE - zCam1) / (zCam2 - zCam1 + 0.0001f);
        if (t > 0.0f && t < 1.0f)
        {
            p2->x = p1->x + t * (p2->x - p1->x);
            p2->y = p1->y + t * (p2->y - p1->y);
            *z2 = NEAR_PLANE;
            *u2 = u1_orig + t * (u2_orig - u1_orig);
        }
    }
    *z1 = zCam1 > NEAR_PLANE ? zCam1 : NEAR_PLANE;
    *z2 = zCam2 > NEAR_PLANE ? zCam2 : NEAR_PLANE;
    return (1);
}

int projectWallToScreen(t_wall *wall, int *screenX1, int *screenX2, float *z1_out, float *z2_out, float *u1_out, float *u2_out)
{
    Vec2_t p1;
    Vec2_t p2;
    float z1;
    float z2;
    float u1;
    float u2;
    float distX1;
    float distY1;
    float distX2;
    float distY2;
    float centerScreenW;
    float focalLength;
    float x1;
    float x2;
    float wallLen;

    p1 = wall->p1;
    p2 = wall->p2;
    
    // Calculate wall length for texture tiling
    wallLen = sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
    u1 = 0.0f;
    u2 = wallLen / 64.0f; // Tile every 64 units

    if (!clipWallToNearPlane(&p1, &p2, &z1, &z2, &u1, &u2))
        return (0);
    distX1 = p1.x - global.cam.camPos.x;
    distY1 = p1.y - global.cam.camPos.y;
    distX2 = p2.x - global.cam.camPos.x;
    distY2 = p2.y - global.cam.camPos.y;
    distX1 = distX1 * sin(global.cam.camAngle) - distY1 * cos(global.cam.camAngle);
    distX2 = distX2 * sin(global.cam.camAngle) - distY2 * cos(global.cam.camAngle);
    centerScreenW = screenW / 2;
    focalLength = (screenW / 2.0f) / tanf(FOV / 2.0f);
    x1 = -distX1 * focalLength / z1;
    x2 = -distX2 * focalLength / z2;
    *screenX1 = (int)(centerScreenW + x1);
    *screenX2 = (int)(centerScreenW + x2);
    *z1_out = z1;
    *z2_out = z2;
    *u1_out = u1;
    *u2_out = u2;
    return (1);
}

Vec2_t calculateWallHitPoint(t_wall *wall, int screenX, float z)
{
    float normalizedX;
    float tanHalfFOV;
    float rayAngleOffset;
    float rayAngle;
    float rayDirX;
    float rayDirY;
    Vec2_t hitPoint;

    (void)wall;
    normalizedX = ((float)screenX - screenW / 2.0f) / (screenW / 2.0f);
    tanHalfFOV = tanf(FOV / 2.0f);
    rayAngleOffset = atanf(normalizedX * tanHalfFOV);
    rayAngle = global.cam.camAngle + rayAngleOffset;
    rayDirX = cosf(rayAngle);
    rayDirY = sinf(rayAngle);
    hitPoint.x = global.cam.camPos.x + rayDirX * z;
    hitPoint.y = global.cam.camPos.y + rayDirY * z;
    return (hitPoint);
}

float calculateWallU(t_wall *wall, Vec2_t hitPoint)
{
    Vec2_t wallDir;
    float wallLengthSq;
    Vec2_t toHit;
    float dot;
    float u;

    wallDir.x = wall->p2.x - wall->p1.x;
    wallDir.y = wall->p2.y - wall->p1.y;
    wallLengthSq = wallDir.x * wallDir.x + wallDir.y * wallDir.y;
    if (wallLengthSq < 0.0001f)
        return (0.0f);
    toHit.x = hitPoint.x - wall->p1.x;
    toHit.y = hitPoint.y - wall->p1.y;
    dot = toHit.x * wallDir.x + toHit.y * wallDir.y;
    u = dot / wallLengthSq;
    if (u < 0.0f) u = 0.0f;
    if (u > 1.0f) u = 1.0f;
    return (u);
}

int getWallOrientation(t_wall *wall)
{
    Vec2_t wallDir;
    float angle;

    wallDir.x = wall->p2.x - wall->p1.x;
    wallDir.y = wall->p2.y - wall->p1.y;
    angle = atan2f(wallDir.y, wallDir.x);
    return ((int)((angle + M_PI) / (M_PI / 2.0f)) % 4);
}

void drawWallSlice(t_render *render, t_wall *wall, int screenX1, int screenX2, float z1, float z2, float bottomHeight, float topHeight, int textureId)
{
    float centerScreenH;
    float relativeTop;
    float relativeBottom;
    float heightScale;
    float y1_top;
    float y1_bot;
    float y2_top;
    float y2_bot;
    int startX;
    int endX;
    int x;
    float t;
    float z;
    int yTop;
    int yBot;
    int y;
    color_t c;
    int color;
    t_texture *tex;
    float vCoord;
    int texX;
    int texY;
    float wallHeightScreen;
    Vec2_t hitPoint;
    float uCoord;
    int r;
    int g;
    int b;

    if (screenX1 >= screenW || screenX2 < 0)
        return;
    centerScreenH = screenH / 2;
    relativeTop = topHeight - global.cam.camZ;
    relativeBottom = bottomHeight - global.cam.camZ;
    heightScale = (screenW / 2.0f) / tanf(FOV / 2.0f);
    y1_top = centerScreenH - (relativeTop * heightScale) / z1;
    y1_bot = centerScreenH - (relativeBottom * heightScale) / z1;
    y2_top = centerScreenH - (relativeTop * heightScale) / z2;
    y2_bot = centerScreenH - (relativeBottom * heightScale) / z2;
    startX = (screenX1 < 0) ? 0 : screenX1;
    endX = (screenX2 >= screenW) ? screenW - 1 : screenX2;
    x = startX;
    while (x <= endX)
    {
        t = (float)(x - screenX1) / (float)(screenX2 - screenX1 + 1);
        z = z1 + t * (z2 - z1);
        yTop = (int)(centerScreenH + (y1_top + t * (y2_top - y1_top)));
        yBot = (int)(centerScreenH + (y1_bot + t * (y2_bot - y1_bot)));
        if (yTop < 0)
            yTop = 0;
        if (yBot >= screenH)
            yBot = screenH - 1;
        tex = NULL;
        uCoord = 0.0f;
        if (textureId >= 0 && textureId < global.tex_manager.count)
        {
            tex = &global.tex_manager.textures[textureId];
            if (!(tex && tex->loaded && tex->addr))
                tex = NULL;
        }
        if (!tex)
        {
            x++;
            continue;
        }
        wallHeightScreen = (float)(yBot - yTop);
        if (wallHeightScreen < 1.0f) wallHeightScreen = 1.0f;
        y = yTop;
        while (y <= yBot)
        {
            hitPoint = calculateWallHitPoint(wall, x, z);
            uCoord = calculateWallU(wall, hitPoint);
            if (uCoord < 0.0f) uCoord = 0.0f;
            if (uCoord > 1.0f) uCoord = 1.0f;
            vCoord = (float)(y - yTop) / wallHeightScreen;
            if (vCoord < 0.0f) vCoord = 0.0f;
            if (vCoord > 1.0f) vCoord = 1.0f;
            texX = (int)(uCoord * tex->width) % tex->width;
            texY = (int)(vCoord * tex->height) % tex->height;
            color = getTexturePixel(textureId, texX, texY);
            c = getColorBydistance(z);
            r = ((color >> 16) & 0xFF) * c.R / 255;
            g = ((color >> 8) & 0xFF) * c.G / 255;
            b = (color & 0xFF) * c.B / 255;
            color = (r << 16) | (g << 8) | b;
            putPixel(render, x, y, color);
            y++;
        }
        x++;
    }
    (void)wall;
    (void)textureId;
}

void drawWallSliceClipped(t_render *render, t_wall *wall, int clippedX1, int clippedX2, float z1, float z2, int origX1, int origX2, float bottomHeight, float topHeight, int textureId, int sectorId, int flags, float u1, float u2)
{
    float centerScreenH;
    float relativeTop;
    float relativeBottom;
    float heightScale;
    float y1_top;
    float y1_bot;
    float y2_top;
    float y2_bot;
    float origWidth;
    int x;
    float t;
    float invZ1;
    float invZ2;
    float invZ;
    float z;
    int yTop;
    int yBot;
    int wallYTop;
    int wallYBot;
    t_texture *tex;
    float uCoord;
    float wallHeightScreen;
    int y;
    int color;
    float vCoord;
    int texX;
    int texY;
    color_t c;
    int r;
    int g;
    int b;

    float uz1;
    float uz2;
    float currentUz;

    if (clippedX1 > clippedX2)
        return;
    centerScreenH = screenH / 2;
    relativeTop = topHeight - global.cam.camZ;
    relativeBottom = bottomHeight - global.cam.camZ;
    heightScale = (screenW / 2.0f) / tanf(FOV / 2.0f);
    y1_top = centerScreenH - (relativeTop * heightScale) / z1;
    y1_bot = centerScreenH - (relativeBottom * heightScale) / z1;
    y2_top = centerScreenH - (relativeTop * heightScale) / z2;
    y2_bot = centerScreenH - (relativeBottom * heightScale) / z2;
    origWidth = (float)(origX2 - origX1);
    if (origWidth == 0)
        origWidth = 1;
    
    invZ1 = 1.0f / z1;
    invZ2 = 1.0f / z2;
    uz1 = u1 * invZ1;
    uz2 = u2 * invZ2;

    x = clippedX1;
    while (x <= clippedX2)
    {
        if (x < 0 || x >= screenW)
        {
            x++;
            continue;
        }
        t = (float)(x - origX1) / origWidth;
        invZ = invZ1 + t * (invZ2 - invZ1);
        z = 1.0f / invZ;
        if (z < 0.1f)
            z = 0.1f;
        yTop = (int)(y1_top + t * (y2_top - y1_top));
        yBot = (int)(y1_bot + t * (y2_bot - y1_bot));
        if (yTop < 0)
            yTop = 0;
        if (yBot >= screenH)
            yBot = screenH - 1;
        if ((flags & RENDER_CEILING) && global.ybuffer.yTop[x] < yTop)
        {
            renderCeilingSlice(render, x, global.ybuffer.yTop[x], yTop, sectorId);
            global.ybuffer.yTop[x] = yTop;
            global.ybuffer.ceilingSector[x] = sectorId;
        }
        wallYTop = (yTop > global.ybuffer.yTop[x]) ? yTop : global.ybuffer.yTop[x];
        wallYBot = (yBot < global.ybuffer.yBottom[x]) ? yBot : global.ybuffer.yBottom[x];
        if (flags & RENDER_WALL)
        {
            tex = NULL;
            uCoord = 0.0f;
            if (textureId >= 0 && textureId < global.tex_manager.count)
            {
                tex = &global.tex_manager.textures[textureId];
                if (tex && tex->loaded && tex->addr)
                {
                    currentUz = uz1 + t * (uz2 - uz1);
                    uCoord = currentUz * z;
                }
                else
                    tex = NULL;
            }
            wallHeightScreen = (float)(yBot - yTop);
            if (wallHeightScreen < 1.0f) wallHeightScreen = 1.0f;
            y = wallYTop;
            while (y <= wallYBot)
            {
                if (!tex)
                {
                    y++;
                    continue;
                }
                vCoord = (float)(y - yTop) / wallHeightScreen;
                if (vCoord < 0.0f) vCoord = 0.0f;
                if (vCoord > 1.0f) vCoord = 1.0f;
                texX = ((int)(uCoord * tex->width) % tex->width + tex->width) % tex->width;
                texY = ((int)(vCoord * tex->height) % tex->height + tex->height) % tex->height;
                if (texX < 0) texX = 0;
                if (texX >= tex->width) texX = tex->width - 1;
                if (texY < 0) texY = 0;
                if (texY >= tex->height) texY = tex->height - 1;
                color = getTexturePixel(textureId, texX, texY);
                c = getColorBydistance(z);
                r = (((color >> 16) & 0xFF) * c.R) / 255;
                g = (((color >> 8) & 0xFF) * c.G) / 255;
                b = ((color & 0xFF) * c.B) / 255;
                color = (r << 16) | (g << 8) | b;
                putPixel(render, x, y, color);
                if (y >= 0 && y < screenH && x >= 0 && x < screenW)
                    depthBuff[y][x] = z;
                y++;
            }
            if (wallYBot >= wallYTop)
            {
                if (flags & RENDER_CEILING) global.ybuffer.yTop[x] = wallYBot;
                if (flags & RENDER_FLOOR) global.ybuffer.yBottom[x] = wallYTop;
            }
        }
        if ((flags & RENDER_FLOOR) && yBot < global.ybuffer.yBottom[x])
        {
            renderFloorSlice(render, x, yBot, global.ybuffer.yBottom[x], sectorId);
            global.ybuffer.yBottom[x] = yBot;
            global.ybuffer.floorSector[x] = sectorId;
        }
        x++;
    }
    (void)wall;
}

void initOcclusionBuffer(void)
{
    int x;

    x = 0;
    while (x < screenW)
    {
        global.columnOccluded[x] = 0;
        x++;
    }
}

int isRangeFullyOccluded(int x1, int x2)
{
    int x;

    if (x1 < 0) x1 = 0;
    if (x2 >= screenW) x2 = screenW - 1;
    if (x1 > x2) return (1);
    x = x1;
    while (x <= x2)
    {
        if (!global.columnOccluded[x])
            return (0);
        x++;
    }
    return (1);
}

void markColumnsOccluded(int x1, int x2)
{
    int x;

    if (x1 < 0) x1 = 0;
    if (x2 >= screenW) x2 = screenW - 1;
    x = x1;
    while (x <= x2)
    {
        global.columnOccluded[x] = 1;
        x++;
    }
}

void renderSectorRecursive(t_render *render, int startSectorId, t_render_window initialWindow, int unused)
{
    t_render_entry stack[MAX_PORTAL_DEPTH];
    int stackTop;
    t_render_entry entry;
    t_sector *sector;
    int i;
    t_wall *wall;
    int screenX1;
    int screenX2;
    float z1;
    float z2;
    float u1;
    float u2;
    int tmpX;
    float tmpZ;
    float tmpU;
    int clippedX1;
    int clippedX2;
    t_sector *backSector;
    int sample_i;
    int sampleX;
    float t;
    float sampleZ;
    int midY;

    (void)unused;
    stackTop = 0;
    initOcclusionBuffer();
    stack[stackTop].sectorId = startSectorId;
    stack[stackTop].clipLeft = initialWindow.xStart;
    stack[stackTop].clipRight = initialWindow.xEnd;
    stack[stackTop].depth = 0;
    stack[stackTop].fromSectorId = -1;
    stackTop++;
    if (global.debugOverlay.enabled)
    {
        global.debugOverlay.clipWindowStart = initialWindow.xStart;
        global.debugOverlay.clipWindowEnd = initialWindow.xEnd;
    }
    while (stackTop > 0)
    {
        stackTop--;
        entry = stack[stackTop];
        if (entry.sectorId < 0 || entry.sectorId >= global.sectorCount)
            continue;
        if (entry.depth >= MAX_PORTAL_DEPTH)
            continue;
        if (entry.clipLeft > entry.clipRight)
            continue;
        if (isRangeFullyOccluded(entry.clipLeft, entry.clipRight))
            continue;
        if (global.debugOverlay.enabled)
        {
            global.debugOverlay.sectorsVisited++;
            if (entry.depth > global.debugOverlay.maxDepthThisFrame)
                global.debugOverlay.maxDepthThisFrame = entry.depth;
        }
        sector = &global.sectors[entry.sectorId];
        i = 0;
        while (i < sector->wallCount)
        {
            wall = &global.walls[sector->wallIds[i]];
            if (!projectWallToScreen(wall, &screenX1, &screenX2, &z1, &z2, &u1, &u2))
            {
                i++;
                continue;
            }
            if (screenX1 > screenX2)
            {
                tmpX = screenX1;
                screenX1 = screenX2;
                screenX2 = tmpX;
                tmpZ = z1;
                z1 = z2;
                z2 = tmpZ;
                tmpU = u1;
                u1 = u2;
                u2 = tmpU;
            }
            if (screenX2 < entry.clipLeft || screenX1 > entry.clipRight)
            {
                i++;
                continue;
            }
            clippedX1 = (screenX1 < entry.clipLeft) ? entry.clipLeft : screenX1;
            clippedX2 = (screenX2 > entry.clipRight) ? entry.clipRight : screenX2;
            if (isRangeFullyOccluded(clippedX1, clippedX2))
            {
                i++;
                continue;
            }
            if (wall->isPortal && wall->backSectorId >= 0)
            {
                float wallDx;
                float wallDy;
                float camDx;
                float camDy;
                float crossProduct;

                wallDx = wall->p2.x - wall->p1.x;
                wallDy = wall->p2.y - wall->p1.y;
                camDx = global.cam.camPos.x - wall->p1.x;
                camDy = global.cam.camPos.y - wall->p1.y;
                crossProduct = wallDx * camDy - wallDy * camDx;
                if (crossProduct < 0)
                {
                    i++;
                    continue;
                }
                if (wall->backSectorId == entry.fromSectorId)
                {
                    i++;
                    continue;
                }
                backSector = &global.sectors[wall->backSectorId];
                if (global.debugOverlay.enabled)
                    global.debugOverlay.portalTraversals++;
                if (sector->ceilingHeight > backSector->ceilingHeight)
                    drawWallSliceClipped(render, wall, clippedX1, clippedX2, z1, z2, screenX1, screenX2, backSector->ceilingHeight, sector->ceilingHeight, wall->upperTextureId, entry.sectorId, RENDER_CEILING | RENDER_WALL, u1, u2);
                else
                    drawWallSliceClipped(render, wall, clippedX1, clippedX2, z1, z2, screenX1, screenX2, sector->ceilingHeight, sector->ceilingHeight, -1, entry.sectorId, RENDER_CEILING, u1, u2);
                if (sector->floorHeight < backSector->floorHeight)
                    drawWallSliceClipped(render, wall, clippedX1, clippedX2, z1, z2, screenX1, screenX2, sector->floorHeight, backSector->floorHeight, wall->lowerTextureId, entry.sectorId, RENDER_WALL | RENDER_FLOOR, u1, u2);
                else
                    drawWallSliceClipped(render, wall, clippedX1, clippedX2, z1, z2, screenX1, screenX2, sector->floorHeight, sector->floorHeight, -1, entry.sectorId, RENDER_FLOOR, u1, u2);
                if (sector->ceilingHeight == backSector->ceilingHeight && sector->floorHeight == backSector->floorHeight)
                {
                    markColumnsOccluded(clippedX1, clippedX2);
                }
                else if (stackTop < MAX_PORTAL_DEPTH - 1)
                {
                    stack[stackTop].sectorId = wall->backSectorId;
                    stack[stackTop].clipLeft = clippedX1;
                    stack[stackTop].clipRight = clippedX2;
                    stack[stackTop].depth = entry.depth + 1;
                    stack[stackTop].fromSectorId = entry.sectorId;
                    stackTop++;
                }
            }
            else
            {
                drawWallSliceClipped(render, wall, clippedX1, clippedX2, z1, z2, screenX1, screenX2, sector->floorHeight, sector->ceilingHeight, wall->middleTextureId, entry.sectorId, RENDER_CEILING | RENDER_WALL | RENDER_FLOOR, u1, u2);
                if (global.debugOverlay.enabled)
                {
                    sample_i = 0;
                    while (sample_i < 3)
                    {
                        sampleX = global.debugOverlay.samples[sample_i].screenX;
                        if (sampleX >= clippedX1 && sampleX <= clippedX2)
                        {
                            t = (float)(sampleX - screenX1) / (float)(screenX2 - screenX1 + 1);
                            sampleZ = z1 + t * (z2 - z1);
                            if (global.debugOverlay.samples[sample_i].distance < 0 || sampleZ < global.debugOverlay.samples[sample_i].distance)
                            {
                                global.debugOverlay.samples[sample_i].distance = sampleZ;
                                global.debugOverlay.samples[sample_i].wallId = wall->id;
                                global.debugOverlay.samples[sample_i].sectorId = entry.sectorId;
                                midY = screenH / 2;
                                if (midY >= 0 && midY < screenH && sampleX >= 0 && sampleX < screenW)
                                    global.debugOverlay.samples[sample_i].zbuffer = depthBuff[midY][sampleX];
                            }
                        }
                        sample_i++;
                    }
                }
                markColumnsOccluded(clippedX1, clippedX2);
            }
            i++;
        }
    }
}

void renderSectorSimple(t_render *render, int sectorId)
{
    t_sector *sector;
    int i;
    t_wall *wall;
    int screenX1;
    int screenX2;
    float z1;
    float z2;
    float u1;
    float u2;
    t_sector *backSector;

    sector = &global.sectors[sectorId];
    i = 0;
    while (i < sector->wallCount)
    {
        wall = &global.walls[sector->wallIds[i]];
        if (!projectWallToScreen(wall, &screenX1, &screenX2, &z1, &z2, &u1, &u2))
        {
            i++;
            continue;
        }
        if (wall->isPortal && wall->backSectorId >= 0)
        {
            if (wall->backSectorId == global.currentSectorId)
            {
                i++;
                continue;
            }
            backSector = &global.sectors[wall->backSectorId];
            if (sector->ceilingHeight > backSector->ceilingHeight)
                drawWallSlice(render, wall, screenX1, screenX2, z1, z2, backSector->ceilingHeight, sector->ceilingHeight, wall->upperTextureId);
            if (sector->floorHeight < backSector->floorHeight)
                drawWallSlice(render, wall, screenX1, screenX2, z1, z2, sector->floorHeight, backSector->floorHeight, wall->lowerTextureId);
        }
        else
            drawWallSlice(render, wall, screenX1, screenX2, z1, z2, sector->floorHeight, sector->ceilingHeight, wall->middleTextureId);
        i++;
    }
}

void render_scene(t_render *render)
{
    t_render_window fullScreen;
    int x;
    int sectorId;
    int i;
    t_wall *wall;
    int screenX1;
    int screenX2;
    float z1;
    float z2;
    float u1;
    float u2;

    if (global.currentSectorId < 0)
        global.currentSectorId = findSectorContainingPoint(global.cam.camPos);
    precomputeRayDirections();
    initYBuffer();
    if (global.currentSectorId >= 0)
    {
        fullScreen.xStart = 0;
        fullScreen.xEnd = screenW - 1;
        renderSectorRecursive(render, global.currentSectorId, fullScreen, 0);
    }
    
    // Render all other sectors (for isolated geometry like pillars)
    sectorId = 0;
    while (sectorId < global.sectorCount)
    {
        if (sectorId == global.currentSectorId)
        {
            sectorId++;
            continue;
        }
        i = 0;
        while (i < global.sectors[sectorId].wallCount)
        {
            wall = &global.walls[global.sectors[sectorId].wallIds[i]];
            if (projectWallToScreen(wall, &screenX1, &screenX2, &z1, &z2, &u1, &u2))
            {
                if (!wall->isPortal)
                {
                    drawWallSliceClipped(render, wall, screenX1, screenX2, z1, z2, screenX1, screenX2,
                        global.sectors[sectorId].floorHeight, global.sectors[sectorId].ceilingHeight,
                        wall->middleTextureId, sectorId, RENDER_CEILING | RENDER_WALL | RENDER_FLOOR, u1, u2);
                }
            }
            i++;
        }
        sectorId++;
    }
    
    x = 0;
    while (x < screenW)
    {
        if (global.ybuffer.yTop[x] < screenH / 2)
        {
            int ceilSector = (global.ybuffer.ceilingSector[x] >= 0) ? global.ybuffer.ceilingSector[x] : global.currentSectorId;
            renderCeilingSlice(render, x, 0, global.ybuffer.yTop[x], ceilSector);
        }
        if (global.ybuffer.yBottom[x] > screenH / 2)
        {
            int floorSector = (global.ybuffer.floorSector[x] >= 0) ? global.ybuffer.floorSector[x] : global.currentSectorId;
            renderFloorSlice(render, x, global.ybuffer.yBottom[x], screenH, floorSector);
        }
        x++;
    }
}
