#include "env.h"

// Forward to geometric implementation
// The old raycasting code (lines 4-203) is replaced by this wrapper.
void draw_skybox_column(t_env *env, int x, int y1, int y2, double ray_angle)
{
    draw_skybox_geometric(env, x, y1, y2, ray_angle);
}
