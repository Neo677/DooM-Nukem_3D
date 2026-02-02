#ifndef GEOMETRY_H
# define GEOMETRY_H

# include "utils.h"
# include <math.h>

// Vector operations
t_v2    v2_add(t_v2 a, t_v2 b);
t_v2    v2_sub(t_v2 a, t_v2 b);
t_v2    v2_mul(t_v2 a, double scalar);
t_v2    v2_div(t_v2 a, double scalar);
double  v2_dot(t_v2 a, t_v2 b);
double  v2_cross(t_v2 a, t_v2 b);
double  v2_length(t_v2 a);
t_v2    v2_normalize(t_v2 a);
double  v2_dist(t_v2 a, t_v2 b);
double  v2_dist_sq(t_v2 a, t_v2 b);

#endif
