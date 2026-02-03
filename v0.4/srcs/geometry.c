#include "geometry.h"

t_v2    v2_add(t_v2 a, t_v2 b)
{
    return ((t_v2){a.x + b.x, a.y + b.y});
}

t_v2    v2_sub(t_v2 a, t_v2 b)
{
    return ((t_v2){a.x - b.x, a.y - b.y});
}

t_v2    v2_mul(t_v2 a, double scalar)
{
    return ((t_v2){a.x * scalar, a.y * scalar});
}

t_v2    v2_div(t_v2 a, double scalar)
{
    if (scalar == 0)
        return ((t_v2){0, 0});
    return ((t_v2){a.x / scalar, a.y / scalar});
}

double  v2_dot(t_v2 a, t_v2 b)
{
    return (a.x * b.x + a.y * b.y);
}

double  v2_cross(t_v2 a, t_v2 b)
{
    return (a.x * b.y - a.y * b.x);
}

double  v2_length(t_v2 a)
{
    return (sqrt(a.x * a.x + a.y * a.y));
}

t_v2    v2_normalize(t_v2 a)
{
    double len = v2_length(a);
    if (len == 0)
        return ((t_v2){0, 0});
    return (v2_div(a, len));
}

double  v2_dist(t_v2 a, t_v2 b)
{
    return (v2_length(v2_sub(b, a)));
}

double  v2_dist_sq(t_v2 a, t_v2 b)
{
    t_v2 diff = v2_sub(b, a);
    return (v2_dot(diff, diff));
}

// Returns 1 if intersection, 0 otherwise. Stores intersection in `out`
// MOVED TO COLLISION.C to avoid duplicate

