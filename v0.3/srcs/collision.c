#include "../header/collision.h"

bool pointInSector(const t_engine *engine, const t_sector *sector, t_v2 point)
{
    if (!sector || sector->nWalls == 0)
        return (false);
    for (usize i = 0; i < sector->nWalls; i++) {
        const t_wall *w = &engine->walls[sector->firstWall + i];
        t_v2 a = v2i_2_v2(w->a);
        t_v2 b = v2i_2_v2(w->b);
        if (point_side(point, a, b) > 0.0f)
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

