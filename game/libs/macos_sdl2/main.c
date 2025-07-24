
#include <stdio.h>
#include "player.h"
#include "game_state.h"
#include "window.h"
#include "renderer.h"
#include "keyboard.h"

#define SCRNW 1024
#define SCRNH 768
#define FPS 120

void ft_gameLoop(game_state_t *game_state, player_t *player)
{
     while (game_state->is_running)
     {
        G_FrameStart();

        handleEvents(game_state, player);
        R_Render(player, game_state);

        G_FrameEnd(game_state);
     }
}

int main()
{
    // init Engine
    // keybord i/o
    // events handler
    // renderer
    // player
    // rendering context (window)

    game_state_t game_state = G_init(SCRNW, SCRNH, FPS);
    player_t player = P_init(40, 40, SCRNH * 10, M_PI / 2);
    init_keymap();
    w_init(SCRNW, SCRNH);
    

    ft_gameLoop(&game_state, &player);

    return (0);
}