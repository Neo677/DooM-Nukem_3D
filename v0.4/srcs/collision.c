#include "collision.h"

#define EPSILON 1e-6

double norm_vector(double x, double y)
{
    return (sqrt(x * x + y * y));
}


t_v2 parallel_movement(t_v2 move, t_v2 p1, t_v2 p2)
{
    double  norm_move;
    double  norm_wall;
    double  scalar;
    double  wall_x;
    double  wall_y;

    wall_x = p2.x - p1.x;
    wall_y = p2.y - p1.y;

    norm_move = norm_vector(move.x, move.y);
    norm_wall = norm_vector(wall_x, wall_y);

    if (norm_wall < EPSILON || norm_move < EPSILON)
        return ((t_v2){0, 0});

    
    scalar = (wall_x * move.x + wall_y * move.y) / (norm_wall * norm_wall);
    
    
    t_v2 slide;
    slide.x = wall_x * scalar;
    slide.y = wall_y * scalar;

    return (slide);
}



int intersect_segments(t_v2 p1, t_v2 p2, t_v2 p3, t_v2 p4, t_v2 *intersection)
{
    t_v2    p = p1;
    t_v2    r = v2_sub(p2, p1);
    t_v2    q = p3;
    t_v2    s = v2_sub(p4, p3);
    
    double r_cross_s = v2_cross(r, s);
    t_v2 q_minus_p = v2_sub(q, p);
    double qp_cross_r = v2_cross(q_minus_p, r);
    
    
    if (fabs(r_cross_s) < EPSILON)
    {
        
        if (fabs(qp_cross_r) < EPSILON)
        {
            
            return (0); 
        }
        return (0); 
    }
    
    double t = v2_cross(q_minus_p, s) / r_cross_s;
    double u = v2_cross(q_minus_p, r) / r_cross_s;
    
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        if (intersection)
            *intersection = v2_add(p, v2_mul(r, t));
        return (1);
    }
    
    return (0);
}

int point_in_box(t_v2 p, t_v2 min, t_v2 max)
{
    return (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y);
}

int box_intersect(t_v2 min1, t_v2 max1, t_v2 min2, t_v2 max2)
{
    return (min1.x <= max2.x && max1.x >= min2.x &&
            min1.y <= max2.y && max1.y >= min2.y);
}




int point_in_polygon(t_v2 p, t_v2 *vertices, int nb_vertices)
{
    int i, j, c = 0;
    double y_diff;
    
    if (!vertices || nb_vertices < 3)
        return (0);
    
    for (i = 0, j = nb_vertices - 1; i < nb_vertices; j = i++)
    {
        y_diff = vertices[j].y - vertices[i].y;
        if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
            fabs(y_diff) > EPSILON &&
            (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / 
            y_diff + vertices[i].x))
            c = !c;
    }
    return c;
}


int is_convex(t_v2 *vertices, int nb_vertices)
{
    if (!vertices || nb_vertices < 3)
        return (0);
    
    int neg = 0;
    int pos = 0;
    
    for (int i = 0; i < nb_vertices; i++)
    {
        t_v2 a = vertices[i];
        t_v2 b = vertices[(i + 1) % nb_vertices];
        t_v2 c = vertices[(i + 2) % nb_vertices];
        
        t_v2 ab = v2_sub(b, a);
        t_v2 bc = v2_sub(c, b);
        
        double cross = v2_cross(ab, bc);
        
        if (cross < -EPSILON) neg++;
        else if (cross > EPSILON) pos++;
        
        if (neg && pos) return (0); 
    }
    return (1);
}


double distance_point_to_segment(t_v2 p, t_v2 a, t_v2 b)
{
    t_v2 ab = v2_sub(b, a);
    t_v2 ap = v2_sub(p, a);
    
    double ab_len_sq = v2_dot(ab, ab);
    
    
    if (ab_len_sq < EPSILON)
        return sqrt(v2_dot(ap, ap));
    
    
    double t = v2_dot(ap, ab) / ab_len_sq;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    
    
    t_v2 closest = v2_add(a, v2_mul(ab, t));
    t_v2 diff = v2_sub(p, closest);
    
    return sqrt(v2_dot(diff, diff));
}
