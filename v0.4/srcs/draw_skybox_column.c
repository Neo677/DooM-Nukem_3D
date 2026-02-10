#include "env.h"

void draw_skybox_column(t_env *env, int x, int y1, int y2, double ray_angle)
{
    draw_skybox_geometric(env, x, y1, y2, ray_angle);
}
