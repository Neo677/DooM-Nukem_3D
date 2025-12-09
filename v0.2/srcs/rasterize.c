#include "../header/rasterize.h"
#include "../header/game.h"
#include "../header/types.h"
#include "../header/texture.h"

float depthBuff[screenH][screenW];

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
    float pixelShader = 100.0 / dist;
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
                // printf("DEBUG: texture [%d]: %s (%dx%d)\n", textureId, tex->name, tex->width, tex->height);
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

