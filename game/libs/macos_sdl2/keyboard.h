#ifndef KEYBOARD_H
# define KEYBOARD_H

# ifdef _WIN64
    #include "libs/win_sdl2/include/SDL2/SDL.h"
# elif __APPLE__
    #include "SDL2/SDL.h"
#endif

# include "game_state.h"
# include "player.h"
# include <stdio.h>

typedef struct keymap_s
{
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode forward;
    SDL_Scancode backward;
    SDL_Scancode starfe_left;
    SDL_Scancode starfe_right;
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode quit;
    SDL_Scancode toggle_map;
    SDL_Scancode debug_mode;
} keymap_t;

typedef struct key_states_s
{
    bool left;
    bool right;
    bool forward;
    bool backward;
    bool starfe_left;
    bool starfe_right;
    bool up;
    bool down;
    bool quit;
    bool map_state;
    bool debug_mode;
} key_states_t;

enum KBD_KEY_STATE
{
    KEY_STATE_UP,
    KEY_STATE_DOWN
};

void init_keymap();
void handleEvents(game_state_t *game_state, player_t *player);
void processKeyStates(player_t *player, double deltatime);
void handleRealTimeKeys(SDL_Scancode key_scancode, enum KBD_KEY_STATE state);



#endif