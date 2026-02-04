#include "env.h"
#include <stdio.h>

#define PLAYER_MIN_HEIGHT 0.6
#define SLIDE_LIMIT 3

// Helper to check if step is possible
static int can_step(t_env *env, int s1, int s2)
{
    t_sector *sect1 = &env->sector_map.sectors[s1];
    t_sector *sect2 = &env->sector_map.sectors[s2];
    double diff = sect2->floor_height - sect1->floor_height;
    return (diff <= MAX_STEP_HEIGHT);
}

// Helper to check if sector has enough room
static int can_enter_sector(t_env *env, int s_id)
{
    t_sector *sect = &env->sector_map.sectors[s_id];
    return ((sect->ceiling_height - sect->floor_height) >= PLAYER_MIN_HEIGHT);
}

// Helper: Apply wall slide
static void apply_wall_slide(t_env *env, t_v2 pos, t_v2 dir, t_v2 *poly, 
                              int nb_verts, int wall_idx)
{
    t_v2 p1 = poly[wall_idx];
    t_v2 p2 = poly[(wall_idx + 1) % nb_verts];
    
    // Use the robust parallel_movement from collision.c
    t_v2 slide = parallel_movement(dir, p1, p2);
    
    // Apply slide if meaningful
    if (v2_dot(slide, slide) > 1e-6)
    {
        t_v2 new_next = v2_add(pos, slide);
        
        // Only verify checks if we are still inside the sector
        if (point_in_polygon(new_next, poly, nb_verts))
            env->player.pos = new_next;
    }
}

void player_move(t_env *env, double dx, double dy)
{
    if (env->sector_map.nb_sectors == 0) return;
    
    t_v2 pos = env->player.pos;
    t_v2 dir = {dx, dy};
    t_v2 next = v2_add(pos, dir);
    int current_id = env->player.current_sector;
    t_sector *sect = &env->sector_map.sectors[current_id];
    
    // Use pre-allocated buffer instead of malloc
    if (sect->nb_vertices > env->collision_buffer_size)
    {
        DEBUG_LOG("Sector has too many vertices (%d > %d)\n", 
                  sect->nb_vertices, env->collision_buffer_size);
        return;
    }
    
    t_v2 *poly = env->collision_buffer;
    for(int i = 0; i < sect->nb_vertices; i++)
    {
        t_vertex v = env->sector_map.vertices[sect->vertices[i]];
        poly[i] = (t_v2){v.x, v.y};
    }
    
    // Check if next position is inside polygon
    if (!point_in_polygon(next, poly, sect->nb_vertices))
    {
        goto handle_collision;
    }
    
    // Check minimum distance to all walls (player radius collision)
    // Only block movement if we're getting CLOSER to a wall
    int too_close = 0;
    for (int i = 0; i < sect->nb_vertices; i++)
    {
        int j = (i + 1) % sect->nb_vertices;
        double dist_next = distance_point_to_segment(next, poly[i], poly[j]);
        
        if (dist_next < PLAYER_RADIUS)
        {
            // Check if we're getting closer or moving away
            double dist_current = distance_point_to_segment(pos, poly[i], poly[j]);
            
            // Only block if getting closer to the wall
            if (dist_next < dist_current)
            {
                too_close = 1;
                // Treat radius violation as collision
                break;
            }
        }
    }
    
    if (!too_close)
    {
        env->player.pos = next;
        return;
    }
    
handle_collision:
    
    // 2. Collision detected! Find which wall checks.
    int wall_idx = -1;
    double min_dist = 1e9;
    
    // Check intersection with all walls
    for (int i = 0; i < sect->nb_vertices; i++)
    {
        int j = (i + 1) % sect->nb_vertices;
        t_v2 p1 = poly[i];
        t_v2 p2 = poly[j];
        
        t_v2 hit;
        // Extended ray check corresponding to movement + buffer
        if (intersect_segments(pos, v2_add(next, v2_mul(dir, 5.0)), p1, p2, &hit))
        {
            double d = v2_dist_sq(pos, hit);
            if (d < min_dist) {
                min_dist = d;
                wall_idx = i;
            }
        }
    }
    
    // If no intersection found but point_in_poly was false, try to find closest wall
    if (wall_idx == -1)
    {
         for (int i = 0; i < sect->nb_vertices; i++)
        {
            int j = (i + 1) % sect->nb_vertices;
            double d = distance_point_to_segment(next, poly[i], poly[j]);
            if (d < min_dist)
            {
                min_dist = d;
                wall_idx = i;
            }
        }
    }
    
    if (wall_idx == -1) return; // Should not happen ideally
    
    // 3. Handle Wall Collision
    int neighbor = sect->neighbors[wall_idx];
    if (neighbor >= 0)
    {
        // PORTAL
        // Check 1: Step Height
        // Check 2: Sector Room Height (Ceiling - Floor)
        if (can_step(env, current_id, neighbor) && can_enter_sector(env, neighbor))
        {
            t_sector *n_sect = &env->sector_map.sectors[neighbor];
            double diff = n_sect->floor_height - sect->floor_height;
            if (diff > 0)
                env->player.height = n_sect->floor_height + PLAYER_EYE_HEIGHT;
            
            env->player.pos = next;
            env->player.current_sector = neighbor;
            DEBUG_LOG("Transition -> Sector %d\n", neighbor);
        }
        else
        {
            // Blocked by step or height - apply slide
            apply_wall_slide(env, pos, dir, poly, sect->nb_vertices, wall_idx);
        }
    }
    else
    {
        // Solid wall - apply slide
        apply_wall_slide(env, pos, dir, poly, sect->nb_vertices, wall_idx);
    }
}

void update_player_physics(t_env *env)
{
    if (env->sector_map.nb_sectors == 0) return;
    int s_id = env->player.current_sector;
    if (s_id < 0 || s_id >= env->sector_map.nb_sectors) return;

    t_sector *sect = &env->sector_map.sectors[s_id];
    
    // Calcul de la hauteur du sol a la position exacte (Pentes)
    double current_floor = get_sector_floor_height(env, s_id, env->player.pos.x, env->player.pos.y);
    double eye_level = current_floor + PLAYER_EYE_HEIGHT;

    // Frame-rate independent gravity
    double gravity_scale = GRAVITY_FRAME_SCALE * (60.0 / (env->fps ? env->fps : 60)); 
    if (gravity_scale > 0.1) gravity_scale = 0.1;

    env->player.velocity_z -= GRAVITY_CONSTANT;
    
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
