#ifndef RENDER_H
# define RENDER_H

#include "../header/global.h"

float point_side(t_v2 point, t_v2 a, t_v2 b);
void verLine(t_engine *engine, int x, int y0, int y1, u32 color);
void render(t_engine *engine);

#endif