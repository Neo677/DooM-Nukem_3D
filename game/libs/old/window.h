#ifndef WINDOW_H
# define WINDOW_H

# ifdef _WIN64
    #include "libs/win_sdl2/include/SDL2/SDL.h"
# elif __APPLE__
    #include "SDL2/SDL.h"
#endif

void w_init(const unsigned int winw, const unsigned int winh);
void w_shutdown();
SDL_Window* w_get();

#endif