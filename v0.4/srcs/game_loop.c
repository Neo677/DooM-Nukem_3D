#include "env.h"

void    game_loop(t_env *env)
{
    SDL_Event event;
    Uint32    frame_start;
    env->running = 1;
    SDL_SetRelativeMouseMode(SDL_TRUE);  // Capturer souris
    while (env->running)
    {
        frame_start = SDL_GetTicks();
        
        // Events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                env->running = 0;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                env->running = 0;
        }
        // Input
        handle_input(env);
        // Render 3D
        render_3d(env);
        // FPS
        draw_fps(env);
        // Update
        SDL_UpdateTexture(env->sdl.texture, NULL, 
                          env->sdl.texture_pixels, 
                          env->w * sizeof(Uint32));
        SDL_RenderCopy(env->sdl.renderer, env->sdl.texture, NULL, NULL);
        SDL_RenderPresent(env->sdl.renderer);
        
        limit_fps(env, frame_start);
    }
}