#include "collision.h"

// Intersection de segments [p1,p2] et [p3,p4]
// Retourne 1 si intersection, et remplit *intersection si non NULL
int intersect_segments(t_v2 p1, t_v2 p2, t_v2 p3, t_v2 p4, t_v2 *intersection)
{
    t_v2    p = p1;
    t_v2    r = v2_sub(p2, p1);
    t_v2    q = p3;
    t_v2    s = v2_sub(p4, p3);
    
    double r_cross_s = v2_cross(r, s);
    t_v2 q_minus_p = v2_sub(q, p);
    double qp_cross_r = v2_cross(q_minus_p, r);
    
    // Si r x s = 0, lignes paralleles
    if (fabs(r_cross_s) < 1e-6)
    {
        // Colineaires si (q-p) x r = 0
        if (fabs(qp_cross_r) < 1e-6)
        {
            // Verifier chevauchement (plus complexe, ignore pour l'instant)
            return (0); 
        }
        return (0); // Paralleles non colineaires
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

// Algorithme Ray-Casting (Even-Odd rule) ou Winding Number
// Pour polygones simples, Check cross product sides si convexe
// Ici : methode generique pour tout polygone simple
int point_in_polygon(t_v2 p, t_v2 *vertices, int nb_vertices)
{
    int i, j, c = 0;
    for (i = 0, j = nb_vertices - 1; i < nb_vertices; j = i++)
    {
        if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
            (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / 
            (vertices[j].y - vertices[i].y) + vertices[i].x))
            c = !c;
    }
    return c;
}

// Verifie si un polygone est convexe (tous les cross products de même signe)
int is_convex(t_v2 *vertices, int nb_vertices)
{
    if (nb_vertices < 3) return (0);
    
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
        
        if (cross < 0) neg++;
        else if (cross > 0) pos++;
        
        if (neg && pos) return (0); // Signes mixtes => concave
    }
    return (1);
}
