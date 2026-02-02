#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

// ============ MINIMAP OVERLAY ============

// Transformation monde → minimap (dans le coin)
static void world_to_minimap(t_env *env, double wx, double wy, int *sx, int *sy, 
                             int minimap_x, int minimap_y, int minimap_size, double zoom)
{
    // Centrer sur le joueur
    double rel_x = wx - env->player.pos.x;
    double rel_y = wy - env->player.pos.y;
    
    // Appliquer zoom et décaler dans le coin
    *sx = (int)(rel_x * zoom + minimap_x + minimap_size / 2);
    *sy = (int)(rel_y * zoom + minimap_y + minimap_size / 2);
}

// Dessiner la minimap en overlay (coin haut-droite)
void draw_minimap(t_env *env)
{
    if (!env->view_2d.show_minimap)
        return;
    
    // Paramètres minimap
    int minimap_size = 200;  // 200x200 pixels
    int minimap_x = env->w - minimap_size - 20;  // 20px du bord droit
    int minimap_y = 20;  // 20px du haut
    double minimap_zoom = 15.0;  // Zoom plus petit pour voir plus de map
    
    // Fond semi-transparent (noir avec alpha)
    t_rectangle bg = new_rectangle(0xDD000000, 0xFF444444, 1, 2);
    t_point bg_pos = new_point(minimap_x, minimap_y);
    t_point bg_size = new_point(minimap_size, minimap_size);
    draw_rectangle(env, bg, bg_pos, bg_size);
    
    // Dessiner les murs de la minimap
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (env->map.grid[y][x] != 0)
            {
                int sx1, sy1, sx2, sy2;
                world_to_minimap(env, x, y, &sx1, &sy1, minimap_x, minimap_y, 
                               minimap_size, minimap_zoom);
                world_to_minimap(env, x + 1, y + 1, &sx2, &sy2, minimap_x, minimap_y, 
                               minimap_size, minimap_zoom);
                
                // Clipper à la minimap
                if (sx1 < minimap_x) sx1 = minimap_x;
                if (sy1 < minimap_y) sy1 = minimap_y;
                if (sx2 > minimap_x + minimap_size) sx2 = minimap_x + minimap_size;
                if (sy2 > minimap_y + minimap_size) sy2 = minimap_y + minimap_size;
                
                if (sx2 > sx1 && sy2 > sy1)
                {
                    Uint32 wall_color = 0xFF666666;  // Gris
                    t_rectangle wall = new_rectangle(wall_color, wall_color, 1, 0);
                    t_point wall_pos = new_point(sx1, sy1);
                    t_point wall_size = new_point(sx2 - sx1, sy2 - sy1);
                    draw_rectangle(env, wall, wall_pos, wall_size);
                }
            }
        }
    }
    
    // Dessiner le joueur (petit cercle vert)
    int px, py;
    world_to_minimap(env, env->player.pos.x, env->player.pos.y, &px, &py, 
                    minimap_x, minimap_y, minimap_size, minimap_zoom);
    
    // Vérifier que le joueur est dans la minimap
    if (px >= minimap_x && px <= minimap_x + minimap_size &&
        py >= minimap_y && py <= minimap_y + minimap_size)
    {
        t_circle player = new_circle(0xFF00FF00, 0xFF00FF00, new_point(px, py), 3);
        draw_circle(player, env);
        
        // Direction du joueur (petite ligne)
        double dir_x = cos(env->player.angle);
        double dir_y = sin(env->player.angle);
        
        int dir_end_x, dir_end_y;
        world_to_minimap(env, 
                        env->player.pos.x + dir_x * 0.5,
                        env->player.pos.y + dir_y * 0.5,
                        &dir_end_x, &dir_end_y, minimap_x, minimap_y, 
                        minimap_size, minimap_zoom);
        
        t_point p1 = new_point(px, py);
        t_point p2 = new_point(dir_end_x, dir_end_y);
        draw_line(p1, p2, env, 0xFFFFFF00);  // Jaune
    }
    
    // Label "MINIMAP"
    draw_text(env, "MAP", minimap_x + 5, minimap_y + 5, 0xFFFFFFFF);
}
