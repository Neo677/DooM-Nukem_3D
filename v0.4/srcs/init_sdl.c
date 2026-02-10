#include "env.h"

int  set_sdl(t_env *env)
{
    
    env->sdl.window = SDL_CreateWindow("Doom-Nukem v0.4",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        env->w, env->h,
        SDL_WINDOW_SHOWN);
    if (!env->sdl.window)
    {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return (1);
    }
    
    env->sdl.renderer = SDL_CreateRenderer(env->sdl.window, -1,
        SDL_RENDERER_TARGETTEXTURE);
    if (!env->sdl.renderer)
    {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
        return (1);
    }
    
    env->sdl.texture = SDL_CreateTexture(env->sdl.renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        env->w, env->h);
    if (!env->sdl.texture)
    {
        printf("SDL_CreateTexture error: %s\n", SDL_GetError());
        return (1);
    }
    
    env->sdl.texture_pixels = (Uint32 *)malloc(sizeof(Uint32) * env->w * env->h);
    if (!env->sdl.texture_pixels)
    {
        printf("Could not malloc texture pixels\n");
        return (1);
    }
    clear_image(env, 0xFF000000);
    return (0);
}

int init_sdl(t_env *env)
{
    printf("Initializing SDL..\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return (1);
    } if (set_sdl(env)) {
        printf("Could not set SDL\n");
        return (1);
    }
    return (0);
}