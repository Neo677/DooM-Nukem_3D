#include "env.h"

// Helper pour remplir une ligne horizontale
static void fill_hline(Uint32 color, t_point p1, t_point p2, t_env *env)
{
    while (p1.x <= p2.x)
    {
        if (p1.x >= 0 && p1.x < env->w && p1.y >= 0 && p1.y < env->h)
            env->sdl.texture_pixels[p1.x + p1.y * env->w] = color;
        p1.x++;
    }
}

// Dessiner le contour (Bresenham circle midpoint algorithm)
static void draw_outline(t_circle circle, int x, int y, t_env *env)
{
    // 8 points de symétrie
    int points[8][2] = {
        {circle.center.x + x, circle.center.y + y},
        {circle.center.x - x, circle.center.y + y},
        {circle.center.x + x, circle.center.y - y},
        {circle.center.x - x, circle.center.y - y},
        {circle.center.x + y, circle.center.y + x},
        {circle.center.x - y, circle.center.y + x},
        {circle.center.x + y, circle.center.y - x},
        {circle.center.x - y, circle.center.y - x}
    };
    
    for (int i = 0; i < 8; i++)
    {
        int px = points[i][0];
        int py = points[i][1];
        if (px >= 0 && px < env->w && py >= 0 && py < env->h)
            env->sdl.texture_pixels[px + py * env->w] = circle.line_color;
    }
}

// Algorithme de Bresenham pour cercle rempli
void    draw_circle(t_circle circle, t_env *env)
{
    int x = 0;
    int y = circle.radius;
    int p = 3 - 2 * circle.radius;
    
    while (x <= y)
    {
        // Remplir 4 lignes horizontales (symétrie)
        fill_hline(circle.color, 
                   new_point(circle.center.x - x, circle.center.y + y), 
                   new_point(circle.center.x + x, circle.center.y + y), env);
        fill_hline(circle.color, 
                   new_point(circle.center.x - x, circle.center.y - y), 
                   new_point(circle.center.x + x, circle.center.y - y), env);
        fill_hline(circle.color, 
                   new_point(circle.center.x - y, circle.center.y - x), 
                   new_point(circle.center.x + y, circle.center.y - x), env);
        fill_hline(circle.color, 
                   new_point(circle.center.x - y, circle.center.y + x), 
                   new_point(circle.center.x + y, circle.center.y + x), env);
        
        // Dessiner le contour
        draw_outline(circle, x, y, env);
        
        // Prochaine itération
        if (p < 0)
            p += 4 * x++ + 6;
        else
            p += 4 * (x++ - y--) + 10;
    }
}
