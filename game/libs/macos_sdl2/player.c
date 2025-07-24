#include "player.h"

player_t P_init(double x, double y, double z, double angle)
{
    player_t player;
    player.dir_angles = angle;
    player.position.x = x;
    player.position.y = y;
    player.z = z;
    
    return (player);
}