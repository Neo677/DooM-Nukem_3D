#include "game_state.h"

int frame_state = 0;

game_state_t G_init(const unsigned int scrnw, const unsigned int scrnh, int target_fps)
{
    game_state_t game_state;
    game_state.is_debug_mode = false;
    game_state.is_running = true;
    game_state.scrn_h = scrnh;
    game_state.scrn_w = scrnw;
    game_state.target_fps = target_fps;
    game_state.target_frame_time = 1.0 / (double) game_state.target_fps;
    /* 
        ⬆️​⬆️​ cette operation vise a calculer le nombre de FPS cible, 
        ex : si target_fps = 60 alors on va attribuer a target_frame_time cette value
        Cette opération calcule le temps que chaque image doit prendre pour atteindre le FPS cible
        ex : si target_fps est 60, alors le temps par image est 1.0 / 60, soit environ 0.0167 secondes (16,7 millisecondes).
        La conversion permet éviter une division entière. Cela garantit que le résultat est un nombre à virgule flottante (précis). */
    game_state.is_fds_capped = false;
    game_state.delta_time = game_state.target_frame_time;
    game_state.is_debug_mode = false;

    return (game_state);
}

void G_FrameStart()
{
    frame_state = SDL_GetTicks();

}

void G_FrameEnd(game_state_t *state)
{
    state->delta_time = (SDL_GetTicks() - frame_state) / 1000.0;
    /*

    */

    if (state->delta_time < state->target_frame_time) 
    {
        SDL_Delay((state->target_frame_time - state->delta_time) * 1000.0);
        state->delta_time = state->target_frame_time;
        
    }
}
