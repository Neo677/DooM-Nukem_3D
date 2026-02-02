#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

// ============ TRANSFORMATION MONDE → ÉCRAN ============

static void world_to_screen(t_env *env, double wx, double wy, int *sx, int *sy)
{
    // Centrer sur le joueur
    double rel_x = wx - env->player.pos.x;
    double rel_y = wy - env->player.pos.y;
    
    // Appliquer zoom et offset
    *sx = (int)(rel_x * env->view_2d.zoom + env->w / 2 + env->view_2d.offset.x);
    *sy = (int)(rel_y * env->view_2d.zoom + env->h / 2 + env->view_2d.offset.y);
}

// ============ GRILLE ============

static void draw_grid(t_env *env)
{
    if (!env->view_2d.show_grid)
        return;
    
    Uint32 grid_color = 0xFF333333;  // Gris très foncé
    
    // Lignes verticales
    for (int x = 0; x <= env->map.width; x++)
    {
        int sx1, sy1, sx2, sy2;
        world_to_screen(env, x, 0, &sx1, &sy1);
        world_to_screen(env, x, env->map.height, &sx2, &sy2);
        
        t_point p1 = new_point(sx1, sy1);
        t_point p2 = new_point(sx2, sy2);
        draw_line(p1, p2, env, grid_color);
    }
    
    // Lignes horizontales
    for (int y = 0; y <= env->map.height; y++)
    {
        int sx1, sy1, sx2, sy2;
        world_to_screen(env, 0, y, &sx1, &sy1);
        world_to_screen(env, env->map.width, y, &sx2, &sy2);
        
        t_point p1 = new_point(sx1, sy1);
        t_point p2 = new_point(sx2, sy2);
        draw_line(p1, p2, env, grid_color);
    }
}

// ============ MURS ============

static void draw_walls(t_env *env)
{
    // PHASE 2 GRID
    if (env->map.width > 0 && env->map.height > 0) {
        for (int y = 0; y < env->map.height; y++)
        {
             // ... Phase 2 logic skipped for brevity if needed inside checks ...
             // Keeping original loop logic wrapped
             for (int x = 0; x < env->map.width; x++) {
                 if (env->map.grid[y][x] != 0) {
                    Uint32 wall_color = (env->map.grid[y][x] == 1) ? 0xFF888888 : 0xFF666666;
                    int sx1, sy1, sx2, sy2;
                    world_to_screen(env, x, y, &sx1, &sy1);
                    world_to_screen(env, x + 1, y + 1, &sx2, &sy2);
                    draw_rectangle(env, new_rectangle(wall_color, 0xFF444444, 1, 1), new_point(sx1, sy1), new_point(sx2 - sx1, sy2 - sy1));
                 }
             }
        }
    }

    // PHASE 3 SECTORS
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
                world_to_screen(env, v1.x, v1.y, &sx1, &sy1);
                world_to_screen(env, v2.x, v2.y, &sx2, &sy2);
                
                Uint32 color = wall_color;
                if (sect->neighbors[v] >= 0) color = 0xFFFF0000; // Portail rouge
                
                draw_line(new_point(sx1, sy1), new_point(sx2, sy2), env, color);
                
                // Draw vertex
                draw_circle(new_circle(0xFFFFFFFF, 0xFFFFFFFF, new_point(sx1, sy1), 2), env);
            }
            
            // Draw Sector ID center (approx)
            // Calculer centroid
            double cx=0, cy=0;
            for(int k=0; k<sect->nb_vertices; k++) {
                 cx += env->sector_map.vertices[sect->vertices[k]].x;
                 cy += env->sector_map.vertices[sect->vertices[k]].y;
            }
            cx /= sect->nb_vertices;
            cy /= sect->nb_vertices;
            int scx, scy;
            world_to_screen(env, cx, cy, &scx, &scy);
            char buf[16];
            snprintf(buf, 16, "S%d", i);
            draw_text(env, buf, scx, scy, 0xFF00FFFF);
        }
    }
}

// ============ JOUEUR ============

static void draw_player(t_env *env)
{
    int px, py;
    world_to_screen(env, env->player.pos.x, env->player.pos.y, &px, &py);
    
    // Cercle pour le joueur
    int radius = (int)(0.3 * env->view_2d.zoom);
    if (radius < 4) radius = 4;
    
    t_circle player_circle = new_circle(0xFF00AA00, 0xFF00FF00, new_point(px, py), radius);
    draw_circle(player_circle, env);
    
    // Direction (ligne jaune)
    double dir_x = cos(env->player.angle);
    double dir_y = sin(env->player.angle);
    
    int dir_end_x, dir_end_y;
    world_to_screen(env, 
                    env->player.pos.x + dir_x * 0.5,
                    env->player.pos.y + dir_y * 0.5,
                    &dir_end_x, &dir_end_y);
    
    t_point p1 = new_point(px, py);
    t_point p2 = new_point(dir_end_x, dir_end_y);
    draw_line(p1, p2, env, 0xFFFFFF00);
}

// ============ RAYONS (DEBUG) ============

static void draw_rays(t_env *env)
{
    if (!env->view_2d.show_rays)
        return;
    
    double fov_rad = (FOV * PI) / 180.0;
    int num_rays = 30;  // Moins de rayons pour lisibilité
    
    for (int i = 0; i < num_rays; i++)
    {
        double ray_angle = env->player.angle - fov_rad / 2.0 + 
                          (i / (double)num_rays) * fov_rad;
        
        // Lancer le rayon
        t_ray_hit hit = cast_ray_dda(env, ray_angle);
        
        // Calculer point de fin
        double end_x = env->player.pos.x + cos(ray_angle) * hit.distance;
        double end_y = env->player.pos.y + sin(ray_angle) * hit.distance;
        
        // Convertir en screen
        int px, py, ex, ey;
        world_to_screen(env, env->player.pos.x, env->player.pos.y, &px, &py);
        world_to_screen(env, end_x, end_y, &ex, &ey);
        
        // Rayon central plus visible
        Uint32 ray_color = (i == num_rays / 2) ? 0xFFFF0000 : 0xFF660000;
        t_point p1 = new_point(px, py);
        t_point p2 = new_point(ex, ey);
        draw_line(p1, p2, env, ray_color);
        
        // Point de hit
        if (hit.wall_type != 0)
        {
            t_circle hit_point = new_circle(0xFFAA0000, 0xFFFF0000, new_point(ex, ey), 3);
            draw_circle(hit_point, env);
        }
    }
}

// ============ FONCTION PRINCIPALE ============

void render_2d(t_env *env)
{
    // Fond noir
    clear_image(env, 0xFF000000);
    
    // Ordre de rendu (du fond au premier plan)
    draw_grid(env);
    draw_walls(env);
    draw_rays(env);      // Rayons en dessous du joueur
    draw_player(env);
    
    // FPS (en haut à gauche)
    draw_fps_on_screen(env);
    
    // Texte debug
    char zoom_text[64];
    snprintf(zoom_text, sizeof(zoom_text), "Zoom: %.0fx", env->view_2d.zoom);
    draw_text(env, zoom_text, 10, 30, 0xFFAAAAAA);
    
    draw_text(env, "Mode: 2D Top-Down", 10, 50, 0xFFFFFFFF);
    
    // Contrôles
    draw_text(env, "TAB: Switch to 3D", 10, 80, 0xFF888888);
    draw_text(env, "+/-: Zoom", 10, 100, 0xFF888888);
    draw_text(env, "R: Toggle Rays", 10, 120, 0xFF888888);
    draw_text(env, "G: Toggle Grid", 10, 140, 0xFF888888);
}
