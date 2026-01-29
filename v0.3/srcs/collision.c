#include "../header/collision.h"
#include "../header/render.h"
#include "../header/debug_log.h"


bool pointInSector(const t_engine *engine, const t_sector *sector, t_v2 point)
{
    if (!sector || sector->nWalls == 0)
        return (false);
    for (usize i = 0; i < sector->nWalls; i++) {
        const t_wall *w = &engine->walls[sector->firstWall + i];
        t_v2 a = v2i_2_v2(w->a);
        t_v2 b = v2i_2_v2(w->b);
        // After winding fix (swap a<->b), positive = inside, negative = outside
        if (point_side(point, a, b) <= 0.0f)
            return (false);
    }
    return (true);
}


int findSectorAt(const t_engine *engine, t_v2 p)
{
    for (int i = 1; i < (int)engine->nSectors; i++) {
        if (pointInSector(engine, &engine->sectors[i], p))
            return (i);
    }
    return (SECTOR_NONE);
}

bool tryMove(t_engine *engine, t_v2 *pos, t_v2 newPos, f32 radius)
{
    int currentSector = findSectorAt(engine, *pos);
    if (currentSector == SECTOR_NONE)
        return (false);
    if (pointInSector(engine, &engine->sectors[currentSector], newPos)) {
        *pos = newPos;
        return (true);
    }
    const t_sector *sec = &engine->sectors[currentSector];
    for (usize i = 0; i < sec->nWalls; i++) {
        const t_wall *wall = &engine->walls[sec->firstWall + i];

        if (wall->portal > SECTOR_NONE) {
            if (pointInSector(engine, &engine->sectors[wall->portal], newPos)) {
                    int oldSector = engine->camera.sector;
                    *pos = newPos;
                    engine->camera.sector = wall->portal;
                    // Log sector change
                    debug_log_sector_change(&engine->debugLog, engine->frameCount,
                                           engine->currentTime, oldSector, 
                                           wall->portal, (int)(sec->firstWall + i));
                    return (true);
            }
        }
    }
    for (usize i = 0; i < sec->nWalls; i++) {
        const t_wall *wall = &engine->walls[sec->firstWall + i];
        if (wall->portal > SECTOR_NONE)
            continue;
        t_v2 a = v2i_2_v2(wall->a);
        t_v2 b = v2i_2_v2(wall->b);

        t_v2 wallDir = { b.x - a.x, b.y - a.y };
        f32 wallLen = sqrtf(wallDir.x * wallDir.x + wallDir.y * wallDir.y);
        if (wallLen < 0.001f)
            continue;
        wallDir.x /= wallLen;
        wallDir.y /= wallLen;

        t_v2 wallNorm = { -wallDir.y, wallDir.x };
        
        t_v2 toPlayer = { newPos.x - a.x, newPos.y - a.y };
        
        f32 distToWall = toPlayer.x * wallNorm.x + toPlayer.y * wallNorm.y;
        
        if (distToWall < radius && distToWall > -radius) {
            t_v2 movement = { newPos.x - pos->x, newPos.y - pos->y };
            f32 slideAmount = movement.x * wallDir.x + movement.y * wallDir.y;
            
            t_v2 slidePos = {
                pos->x + wallDir.x * slideAmount,
                pos->y + wallDir.y * slideAmount
            };
            
            if (pointInSector(engine, sec, slidePos)) {
                *pos = slidePos;
                // Log collision with slide
                debug_log_collision(&engine->debugLog, engine->frameCount,
                                   engine->currentTime, wall, *pos, newPos, "slide");
                return (true);
            } else {
                // Log collision blocked
                debug_log_collision(&engine->debugLog, engine->frameCount,
                                   engine->currentTime, wall, *pos, newPos, "blocked");
            }
        }
    }
    return (false);
}