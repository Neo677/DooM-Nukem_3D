#ifndef COLLISION_H
# define COLLISION_H

# include "geometry.h"

// Basic checks
int     intersect_segments(t_v2 p1, t_v2 p2, t_v2 p3, t_v2 p4, t_v2 *intersection);
int     point_in_box(t_v2 p, t_v2 min, t_v2 max);
int     box_intersect(t_v2 min1, t_v2 max1, t_v2 min2, t_v2 max2);

// Polygon checks (Sector utils)
int     point_in_polygon(t_v2 p, t_v2 *vertices, int nb_vertices);
int     is_convex(t_v2 *vertices, int nb_vertices);

#endif
