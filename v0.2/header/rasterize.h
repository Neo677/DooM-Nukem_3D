#ifndef RASTERIZE_H
# define RASTERIZE_H

# include "game.h"
# include "types.h"
# include "texture.h"

void clearDepthBuffer(void);
void clearRasterBuffer(void);
void Rasterize(t_render *render);
void putPixel(t_render *render, int x, int y, int color);
int pointInPoly(int nvert, float *vertx, float *verty, float testx, float testy);
color_t getColorBydistance(float dist);

#endif
