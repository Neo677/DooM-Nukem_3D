#include "env.h"
#include <stdio.h>

// Helper to check if step is possible
static int can_step(t_env *env, int s1, int s2)
{
    t_sector *sect1 = &env->sector_map.sectors[s1];
    t_sector *sect2 = &env->sector_map.sectors[s2];
    double diff = sect2->floor_height - sect1->floor_height;
    return (diff <= 0.6); // Max step height
}

void player_move(t_env *env, double dx, double dy)
{
    if (env->sector_map.nb_sectors == 0) return;
    
    // Vars
    t_v2 pos = env->player.pos;
    t_v2 dir = {dx, dy};
    t_v2 next = v2_add(pos, dir);
    int current_id = env->player.current_sector;
    t_sector *sect = &env->sector_map.sectors[current_id];
    
    // 1. Check if simple move is valid (still inside current)
    // Build poly
    // Note: optimization possible avoiding malloc every frame
    t_v2 *poly = malloc(sizeof(t_v2) * sect->nb_vertices);
    for(int i=0; i<sect->nb_vertices; i++) {
        t_vertex v = env->sector_map.vertices[sect->vertices[i]];
        poly[i] = (t_v2){v.x, v.y};
    }
    
    if (point_in_polygon(next, poly, sect->nb_vertices))
    {
        env->player.pos = next;
        free(poly);
        return;
    }
    
    // 2. Collision detected! Find which wall checks.
    // We extend the ray slightly to ensure intersection if we barely crossed
    int wall_idx = -1;
    double min_dist = 1e9;
    
    // Check intersection with all walls
    for (int i = 0; i < sect->nb_vertices; i++)
    {
        int j = (i + 1) % sect->nb_vertices;
        t_v2 p1 = poly[i];
        t_v2 p2 = poly[j];
        
        t_v2 hit;
        // Use segment intersection
        // We use 'next' which is outside, so Segment(pos, next) should intersect boundary
        if (intersect_segments(pos, v2_add(next, v2_mul(dir, 10.0)), p1, p2, &hit))
        {
            double d = v2_dist_sq(pos, hit);
            if (d < min_dist) {
                min_dist = d;
                wall_idx = i;
            }
        }
    }
    
    // If no intersection found but point_in_poly was false, we might be scraping a vertex or robust error.
    // Try checking portals via previous logic as fallback?
    // For now, if no wall identified, we block.
    if (wall_idx == -1) {
        free(poly);
        return;
    }
    
    // 3. Handle Wall Collision
    int neighbor = sect->neighbors[wall_idx];
    if (neighbor >= 0)
    {
        // PORTAL
        if (can_step(env, current_id, neighbor))
        {
            // Enter neighbor
            // Need to verify if 'next' is inside neighbor? 
            // Usually yes, unless we cross TWO portals (corner).
            // Simplification: Update pos and sector
            
            // Check z-step (Visual fix)
            t_sector *n_sect = &env->sector_map.sectors[neighbor];
            double diff = n_sect->floor_height - sect->floor_height;
            if (diff > 0) env->player.height = n_sect->floor_height + 0.5;
            
            env->player.pos = next;
            env->player.current_sector = neighbor;
            printf("Transition -> Sector %d\n", neighbor);
        }
        else // Blocked by Step Height -> Treated as Wall
        {
            // Slide along the portal wall
            goto SLIDE; // dirty goto but effective here to share logic
        }
    }
    else
    {
        // WALL
        SLIDE:;
        t_v2 p1 = poly[wall_idx];
        t_v2 p2 = poly[(wall_idx + 1) % sect->nb_vertices];
        
        // Calculate wall direction
        t_v2 wall_dir = v2_normalize(v2_sub(p2, p1));
        
        // Project movement onto wall
        double dot = v2_dot(dir, wall_dir);
        if (dot > 0) // Only if moving somewhat along the wall
        {
            t_v2 slide = v2_mul(wall_dir, dot);
            // Apply half slide to reduce jitter? or full? 
            // Full slide is mathematically correct for friction-less
            t_v2 new_next = v2_add(pos, slide);
            
            // Verify if new position is valid (inside current)
            // It should be, unless the wall corner is acute.
            // Simple check:
            if (point_in_polygon(new_next, poly, sect->nb_vertices))
            {
                env->player.pos = new_next;
            }
        }
    }
    
    free(poly);
}

void update_player_physics(t_env *env)
{
    if (env->sector_map.nb_sectors == 0) return;
    int s_id = env->player.current_sector;
    if (s_id < 0 || s_id >= env->sector_map.nb_sectors) return;

    t_sector *sect = &env->sector_map.sectors[s_id];
    
    // Calcul de la hauteur du sol à la position exacte (Pentes)
    double current_floor = get_sector_floor_height(env, s_id, env->player.pos.x, env->player.pos.y);
    double eye_level = current_floor + 0.5;

    // Gravity simple
    // Attention: frame rate independent physics needed ideally (dt)
    // Ici on assume ~60 FPS fixed step for simplicity or use frame_timer
    double gravity = 0.05 * (60.0 / (env->fps ? env->fps : 60)); 
    if (gravity > 0.1) gravity = 0.1; // clamp

    env->player.velocity_z -= 0.002; // Constante petite pour test
    
    env->player.height += env->player.velocity_z;

    // Collision Sol
    if (env->player.height < eye_level)
    {
        env->player.height = eye_level;
        env->player.velocity_z = 0;
        env->player.is_falling = 0;
    }
    else
    {
        env->player.is_falling = 1;
    }
    
    // Collision Plafond
    if (env->player.height > sect->ceiling_height - 0.1)
    {
        env->player.height = sect->ceiling_height - 0.1;
        env->player.velocity_z = 0;
    }
}
