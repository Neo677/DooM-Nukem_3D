#ifndef COLLISION_H
# define COLLISION_H

#include "global.h"

int findSectorAt(const t_engine *engine, t_v2 p);
bool pointInSector(const t_engine *engine, const t_sector *sector, t_v2 point);
bool tryMove(t_engine *engine, t_v2 *pos, t_v2 newPos, f32 radius);

#endif