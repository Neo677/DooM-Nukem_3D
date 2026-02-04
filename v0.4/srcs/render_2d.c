#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846
#define EPSILON 1e-6

// ============ TRANSFORMATION MONDE → eCRAN ============

static void world_to_screen(t_env *env, double wx, double wy, int *sx, int *sy)
{
    // Centrer sur le joueur
    double rel_x = wx - env->player.pos.x;
    double rel_y = wy - env->player.pos.y;
    
    // zoom et offset
    *sx = (int)(rel_x * env->view_2d.zoom + env->w / 2 + env->view_2d.offset.x);
    *sy = (int)(rel_y * env->view_2d.zoom + env->h / 2 + env->view_2d.offset.y);
}

// ============ MURS ============
static void draw_walls(t_env *env)
{
    // Grid walls removed

    // PHASE 3 SECTORS
    if (env->sector_map.nb_sectors <= 0 || !env->sector_map.sectors || !env->sector_map.vertices)
        return;
    
    for (int i = 0; i < env->sector_map.nb_sectors; i++)
    {
        t_sector *sect = &env->sector_map.sectors[i];
        
        if (!sect->vertices || sect->nb_vertices <= 0)
            continue;
        
        Uint32 wall_color = (i == env->player.current_sector) ? 0xFF00FF00 : 0xFFAAAAAA;
        
        for (int v = 0; v < sect->nb_vertices; v++)
        {
            int idx1 = sect->vertices[v];
            int idx2 = sect->vertices[(v + 1) % sect->nb_vertices];
            
            // Vérifier que les indices sont valides
            if (idx1 < 0 || idx1 >= env->sector_map.nb_vertices ||
                idx2 < 0 || idx2 >= env->sector_map.nb_vertices)
                continue;
            
            t_vertex v1 = env->sector_map.vertices[idx1];
            t_vertex v2 = env->sector_map.vertices[idx2];
            
            int sx1, sy1, sx2, sy2;
            world_to_screen(env, v1.x, v1.y, &sx1, &sy1);
            world_to_screen(env, v2.x, v2.y, &sx2, &sy2);
            
            Uint32 color = wall_color;
            if (sect->neighbors && sect->neighbors[v] >= 0)
                color = 0xFFFF0000; // Portail rouge
            
            draw_line(new_point(sx1, sy1), new_point(sx2, sy2), env, color);
            
            // Draw vertex
            draw_circle(new_circle(0xFFFFFFFF, 0xFFFFFFFF, new_point(sx1, sy1), 2), env);
        }
        
        // Draw Sector ID center (approx)
        // Calculer centroid seulement si nb_vertices > 0
        if (sect->nb_vertices > 0)
        {
            double cx = 0, cy = 0;
            int valid_vertices = 0;
            
            for (int k = 0; k < sect->nb_vertices; k++)
            {
                int idx = sect->vertices[k];
                if (idx >= 0 && idx < env->sector_map.nb_vertices)
                {
                    cx += env->sector_map.vertices[idx].x;
                    cy += env->sector_map.vertices[idx].y;
                    valid_vertices++;
                }
            }
            
            if (valid_vertices > 0)
            {
                cx /= valid_vertices;
                cy /= valid_vertices;
                int scx, scy;
                world_to_screen(env, cx, cy, &scx, &scy);
                char buf[16];
                snprintf(buf, 16, "S%d", i);
                draw_text(env, buf, scx, scy, 0xFF00FFFF);
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

static double intersect_ray_segment(double ox, double oy, double dx, double dy, 
                                  double x1, double y1, double x2, double y2)
{
    // Ray: O + t*D
    // Seg: A + u*(B-A)
    
    double seg_dx = x2 - x1;
    double seg_dy = y2 - y1;
    
    // Produit vectoriel simplifié: dx * seg_dy - dy * seg_dx
    double det = dx * seg_dy - dy * seg_dx;
    if (fabs(det) < EPSILON)
        return -1;
    
    double t = ((x1 - ox) * seg_dy - (y1 - oy) * seg_dx) / det;
    double u = (dx * (y1 - oy) - dy * (x1 - ox)) / det;
    
    if (t > 0 && u >= 0 && u <= 1)
        return t;
    
    return -1;
}

static void draw_rays(t_env *env)
{
    if (!env->view_2d.show_rays)
        return;
    
    if (!env->sector_map.sectors || !env->sector_map.vertices)
        return;
    
    double fov_rad = (60.0 * PI) / 180.0;
    int num_rays = 50;
    
    for (int i = 0; i < num_rays; i++)
    {
        double ray_angle = env->player.angle - fov_rad / 2.0 + 
                          (i / (double)num_rays) * fov_rad;
        
        double dx = cos(ray_angle);
        double dy = sin(ray_angle);
        
        double closest_dist = 1000.0; // Max draw dist
        int hit_something = 0;
        
        // Check ALL sector walls (Simple brute force for debug view)
        for (int s = 0; s < env->sector_map.nb_sectors; s++)
        {
            t_sector *sect = &env->sector_map.sectors[s];
            
            if (!sect->vertices)
                continue;
            
            for (int v = 0; v < sect->nb_vertices; v++)
            {
                if (sect->neighbors && sect->neighbors[v] >= 0)
                    continue; // Skip Portals (transparent for rays)
                
                int idx1 = sect->vertices[v];
                int idx2 = sect->vertices[(v + 1) % sect->nb_vertices];
                
                // Vérifier validité des indices
                if (idx1 < 0 || idx1 >= env->sector_map.nb_vertices ||
                    idx2 < 0 || idx2 >= env->sector_map.nb_vertices)
                    continue;
                
                t_vertex v1 = env->sector_map.vertices[idx1];
                t_vertex v2 = env->sector_map.vertices[idx2];
                
                double dist = intersect_ray_segment(env->player.pos.x, env->player.pos.y, 
                                                   dx, dy, v1.x, v1.y, v2.x, v2.y);
                
                if (dist > 0 && dist < closest_dist)
                {
                    closest_dist = dist;
                    hit_something = 1;
                }
            }
        }
        
        // Draw Ray
        double end_x = env->player.pos.x + dx * closest_dist;
        double end_y = env->player.pos.y + dy * closest_dist;
        
        int px, py, ex, ey;
        world_to_screen(env, env->player.pos.x, env->player.pos.y, &px, &py);
        world_to_screen(env, end_x, end_y, &ex, &ey);
        
        Uint32 ray_color = (i == num_rays / 2) ? 0xFFFF0000 : 0xFF660000;
        draw_line(new_point(px, py), new_point(ex, ey), env, ray_color);
        
        if (hit_something)
        {
            draw_circle(new_circle(0xFFAA0000, 0xFFFF0000, new_point(ex, ey), 3), env);
        }
    }
}

// ============ FONCTION PRINCIPALE ============
void render_2d(t_env *env)
{
    // Fond noir
    clear_image(env, 0xFF000000);
    
    // Ordre de rendu (du fond au premier plan)
    // draw_grid(env); // Removed

    draw_walls(env);
    draw_rays(env);      // Rayons en dessous du joueur
    draw_player(env);
    
    // FPS (en haut a gauche)
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