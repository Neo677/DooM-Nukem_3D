#include "env.h"

void init_player(t_env *env)
{
    env->player.pos.x = 4.5;
    env->player.pos.y = 4.5;
    env->player.angle = 0.0;
    env->player.height = 1.7;
    env->player.velocity_z = 0.0;
    env->player.is_falling = 0;
    env->player.current_sector = -1;
    
    // NOUVEAU: Initialiser le pitch (vue centrée)
    env->player.pitch = 0.0;
    env->player.pitch_cos = 1.0;
    env->player.pitch_sin = 0.0;
    env->player.horizon = env->h / 2.0;  // Centre de l'écran
    
    // NOUVEAU: Initialiser stats
    env->player.health = 100;
    env->player.armor = 0;
    env->player.ammo = 50;
}