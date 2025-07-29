#ifndef PLAYER_H
# define PLAYER_H

# include "typedefs.h"

typedef struct _player
{
    vec2_t position;
    double z;
    double dir_angles;

} player_t;

player_t P_init(double x, double y, double z, double angle);


#endif