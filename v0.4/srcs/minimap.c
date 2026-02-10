#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846




static void world_to_minimap(t_env *env, double wx, double wy, int *sx, int *sy, 
                             int minimap_x, int minimap_y, int minimap_size, double zoom)
{
    
    double rel_x = wx - env->player.pos.x;
    double rel_y = wy - env->player.pos.y;
    
    
    *sx = (int)(rel_x * zoom + minimap_x + minimap_size / 2);
    *sy = (int)(rel_y * zoom + minimap_y + minimap_size / 2);
}


void draw_minimap(t_env *env)
{
    if (!env->view_2d.show_minimap)
        return;
    
    
    int minimap_size = 200;  
    int minimap_x = env->w - minimap_size - 20;  
    int minimap_y = 20;  
    double minimap_zoom = 15.0;  
    
    
    t_rectangle bg = new_rectangle(0xDD000000, 0xFF444444, 1, 2);
    t_point bg_pos = new_point(minimap_x, minimap_y);
    t_point bg_size = new_point(minimap_size, minimap_size);
    draw_rectangle(env, bg, bg_pos, bg_size);
    


    
    if (env->sector_map.nb_sectors > 0)
    {
        for (int i = 0; i < env->sector_map.nb_sectors; i++)
        {
            t_sector *sect = &env->sector_map.sectors[i];
            
            
            Uint32 wall_color = (i == env->player.current_sector) ? 0xFF00FF00 : 0xFFAAAAAA;
            
            for (int v = 0; v < sect->nb_vertices; v++)
            {
                int idx1 = sect->vertices[v];
                int idx2 = sect->vertices[(v + 1) % sect->nb_vertices];
                
                t_vertex v1 = env->sector_map.vertices[idx1];
                t_vertex v2 = env->sector_map.vertices[idx2];
                
                int sx1, sy1, sx2, sy2;
                world_to_minimap(env, v1.x, v1.y, &sx1, &sy1, minimap_x, minimap_y, minimap_size, minimap_zoom);
                world_to_minimap(env, v2.x, v2.y, &sx2, &sy2, minimap_x, minimap_y, minimap_size, minimap_zoom);
                
                
                Uint32 color = wall_color;
                if (sect->neighbors[v] >= 0) color = 0xFFFF0000; 
                
                
                
                if (sx1 >= minimap_x && sx1 <= minimap_x + minimap_size &&
                    sy1 >= minimap_y && sy1 <= minimap_y + minimap_size)
                        draw_line(new_point(sx1, sy1), new_point(sx2, sy2), env, color);
            }
        }
    }
    
    
    int px, py;
    world_to_minimap(env, env->player.pos.x, env->player.pos.y, &px, &py, 
                    minimap_x, minimap_y, minimap_size, minimap_zoom);
    
    
    if (px >= minimap_x && px <= minimap_x + minimap_size &&
        py >= minimap_y && py <= minimap_y + minimap_size)
    {
        t_circle player = new_circle(0xFF00FF00, 0xFF00FF00, new_point(px, py), 3);
        draw_circle(player, env);
        
        
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
        draw_line(p1, p2, env, 0xFFFFFF00);  
    }
    
    
    draw_text(env, "MAP", minimap_x + 5, minimap_y + 5, 0xFFFFFFFF);
}
