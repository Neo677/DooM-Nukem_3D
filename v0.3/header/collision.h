#ifndef COLLISION_H
# define COLLISION_H

#include "../header/global.h"

int findSectorAt(const t_engine *engine, t_v2 p);
bool pointInSector(const t_engine *engine, const t_sector *sector, t_v2 point);


#endif