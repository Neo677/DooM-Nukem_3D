#include "../header/render.h"
#include "../header/math.h"
#include "../header/collision.h"

float point_side(t_v2 point, t_v2 a, t_v2 b)
{
    return ((b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x));
}

void verLine(t_engine *engine, int x, int y0, int y1, u32 color)
{
    if (x < 0 || x >= SCREENW)
        return;
    if (y0 > y1) {
        int tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    if (y0 < 0)
        y0 = 0;
    if (y1 >= SCREENH)
        y1 = SCREENH - 1;
    for (int y = y0; y <= y1; y++) {
        engine->render.pixels[y * SCREENW + x] = color;
    }
}

int clipToNear(t_v2 *a, t_v2 *b)
{
    if (a->x >= ZNEAR && b->x >= ZNEAR)
        return (1);
    if (a->x <= ZNEAR && b->x <= ZNEAR)
        return (0);
    
    float t = (ZNEAR - a->x) / (b->x - a->x);

    t = fmaxf(0.0f, fminf(1.0f, t));
    if (a->x < ZNEAR) {
        a->x += (b->x - a->x) * t;
        a->y += (b->y - a->y) * t;
    } else {
        b->x += (a->x - b->x) * t;
        b->y += (b->y - b->y) * t;
    }
    return (1);
}

void clearClip(t_engine *engine)
{
    for (int x = 0; x < SCREENW; x++) {
        engine->yLo[x] = 0;
        engine->yHi[x] = SCREENH - 1;
    }
}

void render(t_engine *engine)
{
    if (!engine || !engine->render.pixels)
        return;
    
    engine->camera.cosA = cosf(engine->camera.angle);
    engine->camera.sinA = sinf(engine->camera.angle);

    for (int y = 0; y < SCREENH; ++y) {
        u32 col = (y < SCREENH / 2) ? 0xFF303030 : 0xFF181818;
        
        for (int x = 0; x < SCREENW; ++x) {
            engine->render.pixels[y * SCREENW + x] = col;
        }
    }
    clearClip(engine);

    int startSector = engine->camera.sector;
    if (startSector <= SECTOR_NONE || startSector >= (int)engine->nSectors)
        startSector = findSectorAt(engine, engine->camera.pos);
    if (startSector <= SECTOR_NONE)
        return;

    float fovScaleX = SCREENW / (2.0f * tanf(HFOV * 0.5f));
    float fovScaleY = SCREENH * VFOV;

    bool visited[SECTOR_MAX] = {false};
    int queue[SECTOR_MAX];
    int qh = 0, qt = 0;
    queue[qt++] = startSector;
    visited[startSector] = true;

    while (qh != qt) {
        const t_sector *sec = &engine->sectors[queue[qh++]];
        float ceilH = sec->zCeil - EYE_Z;
        float floorH = sec->zFloor - EYE_Z;

        for (usize wi = 0; wi < sec->nWalls; ++wi) {
            const t_wall *w = &engine->walls[sec->firstWall + wi];
            t_v2 a = worldPosToCamera(engine, v2i_2_v2(w->a));
            t_v2 b = worldPosToCamera(engine, v2i_2_v2(w->b));

            if (!clipToNear(&a, &b))
                continue;
            if (a.x <= 0.0f && b.x <= 0.0f)
                continue;
            
            int sx0 = (int)(SCREENW * 0.5f - a.y * fovScaleX / a.x);
            int sx1 = (int)(SCREENW * 0.5f - a.y * fovScaleX / b.x);
            if (sx0 == sx1)
                continue;
            
            float top0 = SCREENH * 0.5f - ceilH * fovScaleY / a.x;
            float top1 = SCREENH * 0.5f - ceilH * fovScaleY / b.x;
            float bot0 = SCREENW * 0.5f - floorH * fovScaleY / a.x;
            float bot1 = SCREENW * 0.5f - floorH * fovScaleY / b.x;

            const t_sector *nbr = NULL;
            float nt0 = 0, nt1 = 0, nb0 = 0, nb1 = 1;
            bool isPortal = (w->portal > SECTOR_NONE && w->portal < (int)engine->nSectors);
            if (isPortal) {
                nbr = &engine->sectors[w->portal];
                float nCeil = nbr->zCeil - EYE_Z;
                float nFloor = nbr->zFloor - EYE_Z;

                nt0 = SCREENH * 0.5f - nCeil * fovScaleY / a.x;
                nt1 = SCREENH * 0.5f - nCeil * fovScaleY / b.x;
                nb0 = SCREENH * 0.5f - nFloor * fovScaleY / a.x;
                nb1 = SCREENH * 0.5f - nFloor * fovScaleY / b.x;
            }

            if (sx0 > sx1) {
                int t = sx0;
                sx0 = sx1;
                sx1 = t;
                float f;
                f = top0; top0 = top1; top1 = f;
                f = bot0; bot0 = bot1; bot1 = f;
                f = nt0; nt0 = nt1; nt1 = f;
                f = nb0; nb0 = nb1; nb1 = f;
            }

            int xStart = sx0 < 0 ? 0 : sx0;
            int xEnd = sx1 >= SCREENW ? SCREENW - 1 : sx1;
            float invDx = 1.0f / (float)(sx1 - sx0);

            for (int x = xStart; x <= xEnd; ++x) {
                float t = (x - sx0) * invDx;
                float yTop = top0 + (top1 - top0) * t;
                float yBot = bot0 + (bot1 - bot0) * t;
                int ya = (int)ceilf(yTop);
                int yb = (int)floorf(yBot);
                ya = ya < engine->yLo[x] ? engine->yLo[x] : ya;
                yb = yb > engine->yHi[x] ? engine->yHi[x] : yb;
                if (ya > yb)
                    continue;
                
                if (isPortal) {
                    float nyTop = nt0 + (nt1 - nt0) * t;
                    float nyBot = nb0 + (nb1 - nb0) * t;

                    int upperEnd = (int)floorf(fminf(yb, nyTop));
                    if (upperEnd >= ya)
                        verLine(engine, x, ya, upperEnd, sec->ceilCol);

                    int lowerStart = (int)ceilf(fmaxf(nyBot, ya));
                    if (yb >= lowerStart) 
                        verLine(engine, x, lowerStart, yb, sec->floorCol);

                    int portalLo = (int)ceilf(nyTop);
                    int portalHi = (int)floorf(nyBot);

                    portalLo = portalLo < engine->yLo[x] ? engine->yLo[x] : portalLo;
                    portalHi = portalHi < engine->yHi[x] ? engine->yHi[x] : portalHi;
                    if (portalLo <= portalHi) {
                        engine->yLo[x] = portalLo;
                        engine->yHi[x] = portalHi;
                    }
                } else {
                    verLine(engine, x, ya, yb, w->color ? w->color : 0xFFFFFFFF);
                }
            }
            if (isPortal && nbr && !visited[w->portal]) {
                visited[w->portal] = true;
                if (qt < SECTOR_MAX)
                    queue[qt++] = w->portal;
            }
        }
    }

}
