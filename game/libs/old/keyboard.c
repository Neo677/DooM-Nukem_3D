#include "keyboard.h"

keymap_t  keymap;
key_states_t  keystates;
const double MOV_SPEED = 50.0;
const double ELEVATION_SPEED = 200;
const double ROT_SPEED = 2;

void init_keymap()
{
    keymap.forward = SDL_SCANCODE_W;
    keymap.backward = SDL_SCANCODE_S;
    keymap.left = SDL_SCANCODE_A;
    keymap.right = SDL_SCANCODE_D;
    keymap.quit = SDL_SCANCODE_ESCAPE;
    keymap.starfe_left = SDL_SCANCODE_Q;
    keymap.starfe_right = SDL_SCANCODE_E;
    keymap.up = SDL_SCANCODE_UP;
    keymap.down = SDL_SCANCODE_DOWN;
    keymap.toggle_map = SDL_SCANCODE_M;
    keymap.debug_mode = SDL_SCANCODE_O;

    keystates.left = false;
    keystates.right = false;
    keystates.forward = false;
    keystates.backward = false;
    keystates.starfe_left = false;
    keystates.starfe_right = false;
    keystates.up = false;
    keystates.down = false;
    keystates.quit = false;
    keystates.map_state = false;
    keystates.debug_mode = false;
}

void handleEvents(game_state_t *game_state, player_t *player)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_KEYDOWN:
            handleRealTimeKeys(event.key.keysym.scancode, KEY_STATE_DOWN);
            game_state->state_show_map = keystates.map_state;

            if (event.key.keysym.scancode == keymap.quit)
                game_state->is_running = false;

            if (event.key.keysym.scancode == keymap.debug_mode)
                game_state->is_debug_mode = !game_state->is_debug_mode;
            break;
        case SDL_KEYUP:
            handleRealTimeKeys(event.key.keysym.scancode, KEY_STATE_UP);
            break;
        case SDL_QUIT:
            game_state->is_running = false;
        default:
            break;
    }
    processKeyStates(player, game_state->delta_time);
}

void processKeyStates(player_t *player, double deltatime)
{
    if (keystates.forward)
    {
        player->position.x += MOV_SPEED * cos(player->dir_angles) * deltatime;  
        player->position.y += MOV_SPEED * sin(player->dir_angles) * deltatime;
    }
    else if (keystates.backward)
    {
        player->position.x -= MOV_SPEED * cos(player->dir_angles) * deltatime;
        player->position.y -= MOV_SPEED * sin(player->dir_angles) * deltatime;
    }
    if (keystates.left)
        player->dir_angles += ROT_SPEED * deltatime;
    else if (keystates.right)
        player->dir_angles -= ROT_SPEED * deltatime;

    if (keystates.starfe_left)
    {
        player->position.x += MOV_SPEED * cos(player->dir_angles + M_PI / 2) * deltatime;
        player->position.y += MOV_SPEED * sin(player->dir_angles + M_PI / 2) * deltatime;
    }
    if (keystates.starfe_right)
    {
        player->position.x -= MOV_SPEED * cos(player->dir_angles + M_PI / 2) * deltatime;
        player->position.y -= MOV_SPEED * sin(player->dir_angles + M_PI / 2) * deltatime;
    }
    if (keystates.up)
    {
        double old_z = player->z;
        player->z += ELEVATION_SPEED * deltatime;
        printf("Moving UP - Z: %.2f -> %.2f (delta: %.4f)\n", old_z, player->z, deltatime);
    }
    else if (keystates.down)
    {
        double old_z = player->z;
        player->z -= ELEVATION_SPEED * deltatime;
        printf("Moving DOWN - Z: %.2f -> %.2f (delta: %.4f)\n", old_z, player->z, deltatime);
    }
}

void handleRealTimeKeys(SDL_Scancode key_scancode, enum KBD_KEY_STATE state)
{
    if (key_scancode == keymap.forward)
        keystates.forward = state;
    else if (key_scancode == keymap.backward)
        keystates.backward = state;
    
    if (key_scancode == keymap.left)
        keystates.left = state;
    else if (key_scancode == keymap.right)
        keystates.right = state;
    
    if (key_scancode == keymap.starfe_left)
        keystates.starfe_left = state;
    else if (key_scancode == keymap.starfe_right)
        keystates.starfe_right = state;

    if (key_scancode == keymap.up)
    {
        keystates.up = state;
        if (state == KEY_STATE_DOWN)
            printf("UP key pressed\n");
    }
    else if (key_scancode == keymap.down)
    {
        keystates.down = state;
        if (state == KEY_STATE_DOWN)
            printf("DOWN key pressed\n");
    }

    if (key_scancode == keymap.toggle_map && state == true)
        keystates.map_state = !keystates.map_state;
    
}
