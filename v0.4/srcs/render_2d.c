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
    for (int x = 0; x <= MAP_WIDTH; x++)
    {
        int sx1, sy1, sx2, sy2;
        world_to_screen(env, x, 0, &sx1, &sy1);
        world_to_screen(env, x, MAP_HEIGHT, &sx2, &sy2);
        
        t_point p1 = new_point(sx1, sy1);
        t_point p2 = new_point(sx2, sy2);
        draw_line(p1, p2, env, grid_color);
    }
    
    // Lignes horizontales
    for (int y = 0; y <= MAP_HEIGHT; y++)
    {
        int sx1, sy1, sx2, sy2;
        world_to_screen(env, 0, y, &sx1, &sy1);
        world_to_screen(env, MAP_WIDTH, y, &sx2, &sy2);
        
        t_point p1 = new_point(sx1, sy1);
        t_point p2 = new_point(sx2, sy2);
        draw_line(p1, p2, env, grid_color);
    }
}

// ============ MURS ============

static void draw_walls(t_env *env)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (env->map.grid[y][x] != 0)
            {
                // Choisir couleur selon type de mur
                Uint32 wall_color;
                if (env->map.grid[y][x] == 1)
                    wall_color = 0xFF888888;  // Gris clair
                else
                    wall_color = 0xFF666666;  // Gris moyen
                
                // Dessiner un carré rempli pour chaque mur
                int sx1, sy1, sx2, sy2;
                world_to_screen(env, x, y, &sx1, &sy1);
                world_to_screen(env, x + 1, y + 1, &sx2, &sy2);
                
                // Rectangle plein
                t_rectangle r = new_rectangle(wall_color, 0xFF444444, 1, 1);
                t_point pos = new_point(sx1, sy1);
                t_point size = new_point(sx2 - sx1, sy2 - sy1);
                draw_rectangle(env, r, pos, size);
            }
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
