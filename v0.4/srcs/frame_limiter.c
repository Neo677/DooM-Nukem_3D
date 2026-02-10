#include "env.h"

void    draw_fps(t_env *env)
{
    
    Uint32 current_time = SDL_GetTicks();
    
    if (current_time - env->frame_timer >= 1000)  
    {
        env->fps = env->fps_count;
        env->fps_count = 0;
        env->frame_timer = current_time;
    }
    env->fps_count++;
    
    
    draw_fps_on_screen(env);
}

void    limit_fps(t_env *env, Uint32 frame_start)
{
    (void)env;  
    const int TARGET_FPS = 60;
    const int FRAME_TIME = 1000 / TARGET_FPS;  
    
    Uint32 frame_duration = SDL_GetTicks() - frame_start;
    
    if (frame_duration < FRAME_TIME)
        SDL_Delay(FRAME_TIME - frame_duration);
}
