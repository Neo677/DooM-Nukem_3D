#include "env.h"

void    clear_image(t_env *env, Uint32 color)
{
    int i;
    
    i = 0;
    while (i < env->w * env->h)
    {
        env->sdl.texture_pixels[i] = color;
        i++;
    }
}