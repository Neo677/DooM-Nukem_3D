#include "window.h"

SDL_Window *sdl_window = NULL;

void w_init(const unsigned int winw, const unsigned int winh)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    sdl_window = SDL_CreateWindow(
        "engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        winw, 
        winh,
        SDL_WINDOW_SHOWN
    );
}

void w_shutdown()
{
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

SDL_Window* w_get()
{
    return (sdl_window);
}
