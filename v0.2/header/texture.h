#ifndef TEXTURE_H
# define TEXTURE_H

# include "game.h"

t_texture_type detectTextureType(const char *filename);
int loadTexture(t_render *render, const char *path, const char *name);
int findTextureByName(const char *name);
int getTexturePixel(int texId, int x, int y);
void initTextureManager(t_render *render);
void assignTexturesToPolygons(void);

#endif

