#include "../header/game.h"

int ft_key_quit(int keycode, t_render *render)
{
    (void)render;
    
    if (keycode == ESC)
        return (1);
    return (0);
}
