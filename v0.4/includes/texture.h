#ifndef TEXTURE_H
# define TEXTURE_H

# include <stdlib.h>
# include "SDL2/SDL.h"

typedef struct s_texture {
    Uint32  *pixels;    // Pixels ARGB8888
    int     width;
    int     height;
}   t_texture;

// Mipmapping (LOD)
typedef struct s_texture_mipmap {
    t_texture levels[4];  // 4 niveaux de LOD
    int num_levels;
}   t_texture_mipmap;

// Fonctions
void generate_mipmaps(t_texture *base, t_texture_mipmap *mip);
t_texture* select_mip_level(t_texture_mipmap *mip, double distance);
void free_texture(t_texture *tex);

#endif
