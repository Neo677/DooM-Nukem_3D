#include "env.h"

void    init_player(t_env *env)
{
    env->player.pos = new_v2(3.5, 3.5);     // Centre de la map
    env->player.angle = 0.0;                 // Regarde vers l'est
    env->player.height = 0.5;                // Hauteur des yeux (mi-mur)
}