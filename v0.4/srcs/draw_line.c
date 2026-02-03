#include "env.h"
static void fill_img(t_point c, t_env *env, Uint32 color)
{
    // Verifier les limites de l'ecran
    if (c.x >= 0 && c.x < env->w && c.y >= 0 && c.y < env->h)
        env->sdl.texture_pixels[c.x + c.y * env->w] = color;
}
static void draw_line_low(t_point c1, t_point c2, t_env *env, Uint32 color)
{
    int dx = c2.x - c1.x;
    int dy = c2.y - c1.y;
    int yi = (dy < 0) ? -1 : 1;
    int e;
    dy = abs(dy);
    e = 2 * dy - dx;
    
    while (c1.x <= c2.x)
    {
        fill_img(c1, env, color);
        if (e > 0)
        {
            c1.y += yi;
            e -= 2 * dx;
        }
        e += 2 * dy;
        c1.x++;
    }
}
static void draw_line_high(t_point c1, t_point c2, t_env *env, Uint32 color)
{
    int dx = c2.x - c1.x;
    int dy = c2.y - c1.y;
    int xi = (dx < 0) ? -1 : 1;
    int e;
    dx = abs(dx);
    e = 2 * dx - dy;
    
    while (c1.y <= c2.y)
    {
        fill_img(c1, env, color);
        if (e > 0)
        {
            c1.x += xi;
            e -= 2 * dy;
        }
        e += 2 * dx;
        c1.y++;
    }
}
void    draw_line(t_point c1, t_point c2, t_env *env, Uint32 color)
{
    if (abs(c2.y - c1.y) < abs(c2.x - c1.x))
    {
        if (c1.x > c2.x)
            draw_line_low(c2, c1, env, color);
        else
            draw_line_low(c1, c2, env, color);
    }
    else
    {
        if (c1.y > c2.y)
            draw_line_high(c2, c1, env, color);
        else
            draw_line_high(c1, c2, env, color);
    }
}
