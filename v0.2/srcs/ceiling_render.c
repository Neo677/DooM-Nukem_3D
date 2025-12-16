#include "../header/ceiling.h"
#include "../header/rasterize.h"

static float g_rayDirX[SCREEN_W_MAX];
static float g_rayDirY[SCREEN_W_MAX];
static float g_rayAngle[SCREEN_W_MAX];
static int g_rayPrecomputed = 0;

void precomputeRayDirections(void)
{
    float tanHalfFOV;
    int i;
    float normalizedX;
    float rayAngleOffset;

    tanHalfFOV = tanf(FOV / 2.0f);
    i = 0;
    while (i < screenW && i < SCREEN_W_MAX)
    {
        normalizedX = ((float)i / screenW) - 0.5f;
        rayAngleOffset = atanf(normalizedX * 2.0f * tanHalfFOV);
        g_rayAngle[i] = global.cam.camAngle + rayAngleOffset;
        g_rayDirX[i] = cosf(g_rayAngle[i]);
        g_rayDirY[i] = sinf(g_rayAngle[i]);
        i++;
    }
    g_rayPrecomputed = 1;
}

int applyDistanceFog(int color, float distance)
{
    float fogFactor;
    int r;
    int g;
    int b;

    fogFactor = 1.0f - fminf(distance / 300.0f, 0.75f);
    if (fogFactor < 0.25f)
        fogFactor = 0.25f;
    r = (int)(((color >> 16) & 0xFF) * fogFactor);
    g = (int)(((color >> 8) & 0xFF) * fogFactor);
    b = (int)((color & 0xFF) * fogFactor);
    return ((r << 16) | (g << 8) | b);
}

int isSkyboxCeiling(int sectorId)
{
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (1);
    return (global.sectors[sectorId].ceilingTextureId == -1);
}

int isSkyboxFloor(int sectorId)
{
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (0);
    return (global.sectors[sectorId].floorTextureId == -1);
}

int getSkyPixelAt(int screenX, int screenY)
{
    float rayAngle;
    float horizon;
    float vRatio;
    int texU;
    int texV;

    if (!global.skybox.loaded)
        return (SLY_COLOR);
    if (!g_rayPrecomputed)
        precomputeRayDirections();
    rayAngle = g_rayAngle[screenX];
    while (rayAngle < 0)
        rayAngle += 2.0f * M_PI;
    while (rayAngle >= 2.0f * M_PI)
        rayAngle -= 2.0f * M_PI;
    texU = (int)(rayAngle / (2.0f * M_PI) * global.skybox.width);
    horizon = screenH / 2.0f + global.cam.camPitch * (screenH / 4.0f);
    if (screenY < horizon)
    {
        vRatio = (float)(horizon - screenY) / horizon;
        vRatio = 0.5f - vRatio * 0.5f;
    }
    else
        vRatio = 0.5f;
    texV = (int)(vRatio * global.skybox.height);
    texU = ((texU % global.skybox.width) + global.skybox.width) % global.skybox.width;
    texV = fmax(0, fmin(texV, global.skybox.height - 1));
    return (getSkyboxPixel(texU, texV));
}

int getCeilingPixelAt(int screenX, int screenY, int sectorId, float rowDist)
{
    t_sector *sector;
    float rayDirX;
    float rayDirY;
    float worldX;
    float worldY;
    int texId;
    t_texture *tex;
    float scale;
    int texX;
    int texY;
    int color;

    (void)screenY;
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (0x404040);
    sector = &global.sectors[sectorId];
    if (!g_rayPrecomputed)
        precomputeRayDirections();
    rayDirX = g_rayDirX[screenX];
    rayDirY = g_rayDirY[screenX];
    worldX = global.cam.camPos.x + rayDirX * rowDist;
    worldY = global.cam.camPos.y + rayDirY * rowDist;
    texId = sector->ceilingTextureId;
    if (texId < 0 || texId >= global.tex_manager.count)
        return (0x404040);
    tex = &global.tex_manager.textures[texId];
    if (!tex || !tex->loaded)
        return (0x404040);
    scale = FLOOR_TEXTURE_SCALE;
    texX = ((int)(worldX * scale) % tex->width + tex->width) % tex->width;
    texY = ((int)(worldY * scale) % tex->height + tex->height) % tex->height;
    color = getTexturePixel(texId, texX, texY);
    return (applyDistanceFog(color, rowDist));
}

int getFloorPixelAt(int screenX, int screenY, int sectorId, float rowDist)
{
    t_sector *sector;
    float rayDirX;
    float rayDirY;
    float worldX;
    float worldY;
    int texId;
    t_texture *tex;
    float scale;
    int texX;
    int texY;
    int color;

    (void)screenY;
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (0x202020);
    sector = &global.sectors[sectorId];
    if (!g_rayPrecomputed)
        precomputeRayDirections();
    rayDirX = g_rayDirX[screenX];
    rayDirY = g_rayDirY[screenX];
    worldX = global.cam.camPos.x + rayDirX * rowDist;
    worldY = global.cam.camPos.y + rayDirY * rowDist;
    texId = sector->floorTextureId;
    if (texId < 0 || texId >= global.tex_manager.count)
        return (0x202020);
    tex = &global.tex_manager.textures[texId];
    if (!tex || !tex->loaded)
        return (0x202020);
    scale = FLOOR_TEXTURE_SCALE;
    texX = ((int)(worldX * scale) % tex->width + tex->width) % tex->width;
    texY = ((int)(worldY * scale) % tex->height + tex->height) % tex->height;
    color = getTexturePixel(texId, texX, texY);
    return (applyDistanceFog(color, rowDist));
}

void renderCeilingSlice(t_render *render, int x, int yStart, int yEnd, int sectorId)
{
    t_sector *sector;
    float horizon;
    float waveOffset;
    int y;
    int color;
    float relativeHeight;
    float normalizedY;
    float rowDist;

    if (!g_rayPrecomputed)
        precomputeRayDirections();
    if (yStart >= yEnd || x < 0 || x >= screenW)
        return;
    if (yStart < 0)
        yStart = 0;
    if (yEnd > screenH)
        yEnd = screenH;
    sector = NULL;
    if (sectorId >= 0 && sectorId < global.sectorCount)
        sector = &global.sectors[sectorId];
    horizon = screenH / 2.0f + global.cam.camPitch * (screenH / 4.0f);
    waveOffset = WWAVE_MAG * sinf(global.cam.stepWave);
    horizon += waveOffset;
    y = yStart;
    while (y < yEnd)
    {
        if (isSkyboxCeiling(sectorId))
            color = getSkyPixelAt(x, y);
        else
        {
            relativeHeight = sector->ceilingHeight - global.cam.camZ;
            normalizedY = (horizon - y) / (screenH / 2.0f);
            if (fabsf(normalizedY) < 0.001f)
            {
                y++;
                continue;
            }
            rowDist = fabsf(relativeHeight / normalizedY);
            if (rowDist < 0.1f)
                rowDist = 0.1f;
            if (rowDist > 1000.0f)
                rowDist = 1000.0f;
            color = getCeilingPixelAt(x, y, sectorId, rowDist);
        }
        putPixel(render, x, y, color);
        y++;
    }
}

void renderFloorSlice(t_render *render, int x, int yStart, int yEnd, int sectorId)
{
    t_sector *sector;
    float horizon;
    float waveOffset;
    int y;
    int color;
    float relativeHeight;
    float normalizedY;
    float rowDist;
    float depth;
    int intensity;

    if (yStart >= yEnd || x < 0 || x >= screenW)
        return;
    if (yStart < 0)
        yStart = 0;
    if (yEnd > screenH)
        yEnd = screenH;
    sector = NULL;
    if (sectorId >= 0 && sectorId < global.sectorCount)
        sector = &global.sectors[sectorId];
    horizon = screenH / 2.0f + global.cam.camPitch * (screenH / 4.0f);
    waveOffset = WWAVE_MAG * sinf(global.cam.stepWave);
    horizon += waveOffset;
    y = yStart;
    while (y < yEnd)
    {
        if (isSkyboxFloor(sectorId))
            color = 0x332211;
        else if (sector && sector->floorTextureId >= 0)
        {
            relativeHeight = global.cam.camZ - sector->floorHeight;
            normalizedY = (y - horizon) / (screenH / 2.0f);
            if (fabsf(normalizedY) < 0.001f)
            {
                y++;
                continue;
            }
            rowDist = fabsf(relativeHeight / normalizedY);
            if (rowDist < 0.1f)
                rowDist = 0.1f;
            if (rowDist > 1000.0f)
                rowDist = 1000.0f;
            color = getFloorPixelAt(x, y, sectorId, rowDist);
        }
        else
        {
            depth = (float)(y - horizon) / (screenH - horizon);
            intensity = (int)(40 * (1.0f - depth * 0.5f));
            color = (intensity << 16) | (intensity << 8) | intensity;
        }
        putPixel(render, x, y, color);
        y++;
    }
}

void initYBuffer(void)
{
    int x;

    x = 0;
    while (x < screenW && x < SCREEN_W_MAX)
    {
        global.ybuffer.yTop[x] = 0;
        global.ybuffer.yBottom[x] = screenH;
        global.ybuffer.ceilingSector[x] = -1;
        global.ybuffer.floorSector[x] = -1;
        x++;
    }
}
