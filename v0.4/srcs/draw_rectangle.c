#include "env.h"

// Helper pour créer un rectangle
t_rectangle new_rectangle(Uint32 inside_color, Uint32 line_color, int filled, int line_size)
{
    t_rectangle r;
    r.inside_color = inside_color;
    r.line_color = line_color;
    r.filled = filled;
    r.line_size = (line_size < 0) ? 0 : line_size;
    return r;
}

// Fonction helper pour max
static inline int ft_max(int a, int b)
{
    return (a > b) ? a : b;
}

// Dessiner un rectangle (rempli ou vide avec bordure)
void    draw_rectangle(t_env *env, t_rectangle r, t_point pos, t_point size)
{
    int x, y;
    
    // Parcourir toutes les pixels du rectangle
    y = ft_max(0, pos.y);
    while (y <= pos.y + size.y && y < env->h)
    {
        x = ft_max(0, pos.x);
        while (x <= pos.x + size.x && x < env->w)
        {
            // Déterminer si on est sur la bordure
            int on_border = (y < pos.y + r.line_size ||
                           y > pos.y + size.y - r.line_size ||
                           x < pos.x + r.line_size ||
                           x > pos.x + size.x - r.line_size);
            
            if (on_border)
                env->sdl.texture_pixels[x + y * env->w] = r.line_color;
            else if (r.filled)
                env->sdl.texture_pixels[x + y * env->w] = r.inside_color;
            
            x++;
        }
        y++;
    }
}
