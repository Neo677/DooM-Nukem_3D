#include "../header/rasterize.h"
#include "../header/game.h"
#include "../header/types.h"
#include "../header/texture.h"
#include "../header/collision_vector.h"

float depthBuff[screenH][screenW];
static int lastSectorPerColumn[screenW] __attribute__((unused));

#define LIGHT_AMBIENT_MIN   0.08f   
#define LIGHT_DISTANCE_MAX  150.0f  
#define LIGHT_FALLOFF_EXP   1.5f    

void clearDepthBuffer(void)
{
    for (int y = 0; y < screenH; y++) {
        for (int x = 0; x < screenW; x++) {
            depthBuff[y][x] = 999999.0f;
        }
    }
}

void clearRasterBuffer(void) {
    memset(global.screenSpacePoly, 0, sizeof(global.screenSpacePoly));
}

int pointInPoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
    int i, j, isPointInside = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        int isSameCoordn = 0;
        if ((verty[i] > testy) == (verty[j] > testy))
            isSameCoordn = 1;

        float denom = verty[j] - verty[i];
        if (isSameCoordn == 0 && denom != 0.0f) {
            if (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / denom + vertx[i])
                isPointInside = !isPointInside;
        }
    }
    return (isPointInside);
}

color_t getColorBydistance(float dist)
{
    if (dist <= 0.1)
        dist = 0.1;
    float pixelShader = 500.0 / dist;
    if (pixelShader > 1.0)
        pixelShader = 1.0;
    else if (pixelShader < 0.15)
        pixelShader = 0.15;
    color_t clr;
    clr.R = (unsigned char)(0xFF * pixelShader);
    clr.G = (unsigned char)(0x64 * pixelShader);
    clr.B = (unsigned char)(0x00 * pixelShader);
    return (clr);
}

float calculateLightFactor(int lightLevel, float distance)
{
    float sectorLight = (float)lightLevel / 255.0f;
    
    float distanceFactor;
    if (distance <= 0.1f)
        distance = 0.1f;
    
    distanceFactor = 1.0f - powf(distance / LIGHT_DISTANCE_MAX, 1.0f / LIGHT_FALLOFF_EXP);
    if (distanceFactor < 0.0f)
        distanceFactor = 0.0f;
    if (distanceFactor > 1.0f)
        distanceFactor = 1.0f;
    
    float combined = sectorLight * distanceFactor;
    
    if (combined < LIGHT_AMBIENT_MIN)
        combined = LIGHT_AMBIENT_MIN;
    
    return combined;
}

int applyLighting(int color, float lightFactor)
{
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    
    r = (int)(r * lightFactor);
    g = (int)(g * lightFactor);
    b = (int)(b * lightFactor);
    
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    
    return (r << 16) | (g << 8) | b;
}

int applyLightingWithTint(int color, float lightFactor)
{
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    
    float coldTint = (1.0f - lightFactor) * 0.15f;
    
    r = (int)(r * lightFactor * (1.0f - coldTint * 0.5f));
    g = (int)(g * lightFactor * (1.0f - coldTint * 0.3f));
    b = (int)(b * lightFactor * (1.0f + coldTint * 0.2f));
    
    if (r > 255)
        r = 255;
    if (r < 0)
        r = 0;
    if (g > 255)
        g = 255;
    if (g < 0)
        g = 0;
    if (b > 255)
        b = 255;
    if (b < 0)
        b = 0;
    
    return (r << 16) | (g << 8) | b;
}

void putPixel(t_render *render, int x, int y, int color)
{
    if (x >= screenW || y >= screenH || x < 0 || y < 0)
        return;
    char *dst = render->addr + (y * render->line_len + x * (render->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

void Rasterize(t_render *render)
{
    float vx[MAX_VERTS];
    float vy[MAX_VERTS];

    for (int polyIdx = 0; polyIdx < global.screenSpaceVisiblePlanes; polyIdx++) {
        for (int segIdx = 0; segIdx < MAX_VERTS; segIdx++) {
            int vertCnt = global.screenSpacePoly[polyIdx][segIdx].vertCnt;

            if (vertCnt == 0)
                continue;
            float dist = global.screenSpacePoly[polyIdx][segIdx].distFromCamera;
            if (dist <= 0)
                continue;
            int textureId = -1;
            if (polyIdx < MAX_POLYS) 
                textureId = global.polys[global.polyOrder[polyIdx]].textureId;
            
            float minX = screenW, maxX = 0;
            float minY = screenH, maxY = 0;

            for (int nextv = 0; nextv < vertCnt; nextv++) {
                vx[nextv] = global.screenSpacePoly[polyIdx][segIdx].vert[nextv].x;
                vy[nextv] = global.screenSpacePoly[polyIdx][segIdx].vert[nextv].y;
                
                if (vx[nextv] < minX)
                    minX = vx[nextv];
                if (vx[nextv] > maxX)
                    maxX = vx[nextv];
                if (vy[nextv] < minY)
                    minY = vy[nextv];
                if (vy[nextv] > maxY)
                    maxY = vy[nextv];
            }
            if (minX < 0)
                minX = 0;
            if (maxX >= screenW)
                maxX = screenW - 1;
            if (minY < 0)
                minY = 0;
            if (maxY >= screenH)
                maxY = screenH - 1;

            if (minX >= screenW || maxX < 0 || minY >= screenH || maxY < 0)
                continue;
            if (maxX < minX || maxY < minY)
                continue;
            t_texture *tex = NULL;
            if (textureId >= 0 && textureId < global.tex_manager.count)
                tex = &global.tex_manager.textures[textureId];
            static int debug_once = 0;
            if (tex && tex->loaded && debug_once == 0) {
                fflush(stdout);
                debug_once = 1;
            }

            for (int y = (int)minY; y <= (int)maxY; y++) {
                for (int x = (int)minX; x <= (int)maxX; x++) {
                    if (dist < depthBuff[y][x]) {
                        if (pointInPoly(vertCnt, vx, vy, x, y) == 1) {
                            int color;
                            if (tex && tex->loaded) {
                                float u = ((float)(x - minX) / (maxX - minX + 1)) * tex->width;
                                float v = ((float)(y - minY) / (maxY - minY + 1)) * tex->height;
                                float scale = dist / 50.0f;

                                u = fmod(u * scale, tex->width);
                                v = fmod(v * scale, tex->height);

                                color = getTexturePixel(textureId, (int)u, (int)v);
                                color_t c = getColorBydistance(dist);

                                int r = ((color >> 16) & 0xFF) * c.R / 255;
                                int g = ((color >> 8) & 0xFF) * c.G / 255;
                                int b = (color & 0xFF) * c.B / 255;
                                color = (r << 16) | (g << 8) | b;
                            } else {
                                color_t c = getColorBydistance(dist);
                                color = (c.R << 16) | (c.G << 8) | c.B;
                            }
                            putPixel(render, x, y, color);
                            depthBuff[y][x] = dist;
                        }
                    }
                }
            }
        }
    }
}


void renderFloorPerSector(t_render *render)
{
    float waveVal = WWAVE_MAG * sinf(global.cam.stepWave);
    int horizon = screenH / 2 + (int)(waveVal + global.cam.camPitch * screenH * 0.3f);
    
    if (horizon < 0)
        horizon = 0;
    if (horizon >= screenH)
        horizon = screenH - 1;

    float dirX = cosf(global.cam.camAngle);
    float dirY = sinf(global.cam.camAngle);
    
    float fov = 0.66f;
    float planeX = -dirY * fov;
    float planeY = dirX * fov;
    
    float rayDirX0 = dirX - planeX;
    float rayDirY0 = dirY - planeY;
    float rayDirX1 = dirX + planeX;
    float rayDirY1 = dirY + planeY;

    for (int x = 0; x < screenW; x++)
        lastSectorPerColumn[x] = global.currentSectorId;

    for (int y = horizon; y < screenH; y++)
    {
        int p = y - horizon;
        if (p <= 0)
            continue;

        float cameraZ = EYE_HEIGHT + global.cam.camZ;

        float rowDistance = cameraZ / (float)p;

        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenW;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenW;

        float floorX = global.cam.camPos.x + rowDistance * rayDirX0;
        float floorY = global.cam.camPos.y + rowDistance * rayDirY0;

        for (int x = 0; x < screenW; x++)
        {
            Vec2_t worldPos = {floorX, floorY};
            int sectorId = lastSectorPerColumn[x];

            if (sectorId < 0 || sectorId >= global.sectorCount ||
                !pointInSector(&global.sectors[sectorId], worldPos))
            {
                sectorId = findSectorContainingPoint(worldPos);
            }
            lastSectorPerColumn[x] = sectorId;

            int color;
            if (sectorId >= 0)
            {
                t_sector *sector = &global.sectors[sectorId];
                int texId = sector->floorTextureId;

                if (texId >= 0 && texId < global.tex_manager.count)
                {
                    t_texture *tex = &global.tex_manager.textures[texId];
                    if (tex->loaded)
                    {
                        int texX = (int)(floorX * FLOOR_TEXTURE_SCALE) % tex->width;
                        int texY = (int)(floorY * FLOOR_TEXTURE_SCALE) % tex->height;
                        
                        if (texX < 0)
                            texX += tex->width;
                        if (texY < 0)
                            texY += tex->height;

                        color = getTexturePixel(texId, texX, texY);

                        float shade = 1.0f - (rowDistance / 400.0f);
                        if (shade < 0.15f)
                            shade = 0.15f;
                        if (shade > 1.0f)
                            shade = 1.0f;

                        shade *= (float)sector->lightLevel / 255.0f;

                        int r = (int)(((color >> 16) & 0xFF) * shade);
                        int g = (int)(((color >> 8) & 0xFF) * shade);
                        int b = (int)((color & 0xFF) * shade);
                        color = (r << 16) | (g << 8) | b;
                    }
                    else
                    {
                        if (sectorId < MAX_POLYS)
                            color = global.polys[sectorId].color;
                        else
                            color = 0x404040;
                    }
                }
                else
                {
                    if (sectorId < MAX_POLYS)
                        color = global.polys[sectorId].color;
                    else
                        color = 0x404040;

                    float shade = 1.0f - (rowDistance / 400.0f);
                    if (shade < 0.15f)
                        shade = 0.15f;

                    int r = (int)(((color >> 16) & 0xFF) * shade);
                    int g = (int)(((color >> 8) & 0xFF) * shade);
                    int b = (int)((color & 0xFF) * shade);
                    color = (r << 16) | (g << 8) | b;
                }
            }
            else
            {
                float shade = 1.0f - (rowDistance / 400.0f);
                if (shade < 0.1f)
                    shade = 0.1f;
                int intensity = (int)(30 * shade);
                color = (intensity << 16) | (intensity << 8) | intensity;
            }

            if (rowDistance < depthBuff[y][x])
            {
                putPixel(render, x, y, color);
                depthBuff[y][x] = rowDistance;
            }

            floorX += floorStepX;
            floorY += floorStepY;
        }
    }
}

