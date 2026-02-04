#include "env.h"
#include "entities.h"

void    game_loop(t_env *env)
{
    SDL_Event event;
    Uint32    frame_start;
    env->running = 1;
    // Capture souris desactivee par defaut - utiliser touche C pour l'activer
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
        
        // NOUVEAU: Capturer les mouvements de souris pour le pitch
        if (env->mouse_captured)
        {
            SDL_GetRelativeMouseState(&env->sdl.mouse_x, &env->sdl.mouse_y);
        }
        else
        {
            env->sdl.mouse_x = 0;
            env->sdl.mouse_y = 0;
        }
        
        // Input
        handle_input(env);
        // Physics
        update_player_physics(env);
        // Update enemy AI
        update_all_enemies_ai(env);
        // Render 3D
        // Render 3D
        render_3d(env);
        // HUD
        draw_hud(env);
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