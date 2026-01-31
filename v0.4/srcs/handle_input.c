#include "env.h"

void    handle_input(t_env *env)
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    t_v2        new_pos;
    double      dx, dy;
    // WASD movement
    if (keys[SDL_SCANCODE_W])  // Avancer
    {
        dx = cos(env->player.angle) * MOVE_SPEED;
        dy = sin(env->player.angle) * MOVE_SPEED;
        new_pos = new_v2(env->player.pos.x + dx, env->player.pos.y + dy);
        
        // Vérifier collision
        if (env->map.grid[(int)new_pos.y][(int)new_pos.x] == 0)
            env->player.pos = new_pos;
    }
    
    if (keys[SDL_SCANCODE_S])  // Reculer
    {
        dx = cos(env->player.angle) * MOVE_SPEED;
        dy = sin(env->player.angle) * MOVE_SPEED;
        new_pos = new_v2(env->player.pos.x - dx, env->player.pos.y - dy);
        
        if (env->map.grid[(int)new_pos.y][(int)new_pos.x] == 0)
            env->player.pos = new_pos;
    }
    // Rotation A/D
    if (keys[SDL_SCANCODE_A])  // Tourner gauche
        env->player.angle -= ROT_SPEED;
    
    if (keys[SDL_SCANCODE_D])  // Tourner droite
        env->player.angle += ROT_SPEED;
}