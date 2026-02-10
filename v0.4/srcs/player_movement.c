#include "env.h"
#include "env.h"
#include <stdio.h>
#include <math.h>

#define PLAYER_MIN_HEIGHT 0.6
#define SLIDE_LIMIT 3


static int can_step(t_env *env, int s1, int s2)
{
    t_sector *sect1 = &env->sector_map.sectors[s1];
    t_sector *sect2 = &env->sector_map.sectors[s2];
    double diff = sect2->floor_height - sect1->floor_height;
    
    return (diff <= MAX_STEP_HEIGHT);
}


static int can_enter_sector(t_env *env, int s_id)
{
    t_sector *sect = &env->sector_map.sectors[s_id];
    return ((sect->ceiling_height - sect->floor_height) >= PLAYER_MIN_HEIGHT);
}


static void apply_wall_slide(t_env *env, t_v2 pos, t_v2 dir, t_v2 *poly, 
                              int nb_verts, int wall_idx)
{
    t_v2 p1 = poly[wall_idx];
    t_v2 p2 = poly[(wall_idx + 1) % nb_verts];
    
    
    t_v2 slide = parallel_movement(dir, p1, p2);
    
    
    if (v2_dot(slide, slide) > 1e-6)
    {
        t_v2 new_next = v2_add(pos, slide);
        
        
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
    if (current_id < 0 || current_id >= env->sector_map.nb_sectors) return;

    t_sector *sect = &env->sector_map.sectors[current_id];
    
    
    if (sect->nb_vertices > env->collision_buffer_size) return;
    
    t_v2 *poly = env->collision_buffer;
    for(int i = 0; i < sect->nb_vertices; i++)
    {
        t_vertex v = env->sector_map.vertices[sect->vertices[i]];
        poly[i] = (t_v2){v.x, v.y};
    }
    
    
    if (point_in_polygon(next, poly, sect->nb_vertices))
    {
        
        for (int i = 0; i < sect->nb_vertices; i++)
        {
            int j = (i + 1) % sect->nb_vertices;
            
            
            int is_passable_portal = 0;
            if (sect->neighbors[i] >= 0)
            {
                int neighbor = sect->neighbors[i];
                int step_ok = can_step(env, current_id, neighbor);
                
                
                if (!step_ok) {
                    t_sector *n_sect = &env->sector_map.sectors[neighbor];
                    double barrier_h = n_sect->floor_height + PLAYER_EYE_HEIGHT - 0.1;
                    if (env->player.height >= barrier_h) step_ok = 1;
                }
                
                if (step_ok && can_enter_sector(env, neighbor)) {
                    is_passable_portal = 1;
                }
            }

            if (distance_point_to_segment(next, poly[i], poly[j]) < PLAYER_RADIUS)
            {
                 
                 if (distance_point_to_segment(next, poly[i], poly[j]) < 
                     distance_point_to_segment(pos, poly[i], poly[j]))
                 {
                     if (is_passable_portal) {
                         
                         
                         continue; 
                     }
                     
                     
                     DEBUG_LOG("[RADIUS] Slide against Wall %d (Passable=%d)\n", i, is_passable_portal);
                     apply_wall_slide(env, pos, dir, poly, sect->nb_vertices, i);
                     return;
                 }
            }
        }
        env->player.pos = next;
        return;
    }
    
    
    int wall_idx = -1;
    double min_dist = 1e9;

    for (int i = 0; i < sect->nb_vertices; i++)
    {
        int j = (i + 1) % sect->nb_vertices;
        t_v2 p1 = poly[i];
        t_v2 p2 = poly[j];
        
        t_v2 hit;
        
        
        if (intersect_segments(pos, v2_add(pos, v2_mul(dir, 1.1)), p1, p2, &hit))
        {
            double d = v2_dist_sq(pos, hit);
            if (d < min_dist) {
                min_dist = d;
                wall_idx = i;
                DEBUG_LOG("[HIT] Wall %d (Neigh: %d) DistSq: %.4f\n", i, sect->neighbors[i], d);
            }
        }
    }
    
    
    
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
        if (wall_idx != -1) DEBUG_LOG("[FALLBACK] Closest Wall %d Dist: %.4f\n", wall_idx, min_dist);
    }

    if (wall_idx == -1) return; 

    
    int neighbor = sect->neighbors[wall_idx];
    
    if (neighbor >= 0)
    {
        
        
        
        int step_ok = can_step(env, current_id, neighbor);
        
        
        t_sector *n_sect = &env->sector_map.sectors[neighbor];
        if (!step_ok)
        {
            double barrier_h = n_sect->floor_height + PLAYER_EYE_HEIGHT - 0.1;
            if (env->player.height >= barrier_h)
            {
                step_ok = 1;
                DEBUG_LOG("[JUMP OVER] Z=%.2f Barrier=%.2f\n", env->player.height, barrier_h);
            }
        }

        int can_enter = can_enter_sector(env, neighbor);
        if (step_ok && can_enter)
        {
             
             env->player.pos = next; 
             env->player.current_sector = neighbor;
             
             
             double diff = n_sect->floor_height - sect->floor_height;
             
             
             if (diff > 0 && diff <= MAX_STEP_HEIGHT)
                env->player.height += diff; 
                
             DEBUG_LOG("[TRANSITION] Sector %d -> %d\n", current_id, neighbor);
        }
        else
        {
            
            DEBUG_LOG("[BLOCK] Portal %d->%d: StepOk=%d CanEnter=%d (Z=%.2f FloorNext=%.2f)\n", 
                current_id, neighbor, step_ok, can_enter, env->player.height, n_sect->floor_height);
            apply_wall_slide(env, pos, dir, poly, sect->nb_vertices, wall_idx);
        }
    }
    else
    {
        
        DEBUG_LOG("[BLOCK] Solid Wall %d (Neigh: %d)\n", wall_idx, neighbor);
        apply_wall_slide(env, pos, dir, poly, sect->nb_vertices, wall_idx);
    }
}

void update_player_physics(t_env *env)
{
    if (env->sector_map.nb_sectors == 0) return;
    int s_id = env->player.current_sector;
    if (s_id < 0 || s_id >= env->sector_map.nb_sectors) return;

    t_sector *sect = &env->sector_map.sectors[s_id];
    
    
    double current_floor = get_sector_floor_height(env, s_id, env->player.pos.x, env->player.pos.y);
    double eye_level = current_floor + PLAYER_EYE_HEIGHT;

    
    if (env->single_step_mode && !env->step_trigger) return;
    env->step_trigger = 0;
    
     double gravity = GRAVITY_CONSTANT;
    if (env->slow_motion) gravity *= 0.1;

    
    env->player.velocity_z -= gravity;
    env->player.height += env->player.velocity_z;

    
    if (isnan(env->player.height) || isinf(env->player.height)) {
        printf("[NAN RESET] Height was NaN! Resetting to default.\n");
        env->player.height = eye_level;
        env->player.velocity_z = 0;
    }

    
    double gap = env->player.height - eye_level;
    
    if (env->player.height < eye_level)
    {
        env->player.height = eye_level;
        env->player.velocity_z = 0;
        env->player.is_falling = 0; 
        if (env->debug_physics) printf("[GROUND HIT] Snapped. Z=%.3f Floor=%.3f\n", env->player.height, current_floor);
    }
    else
    {
        
        
        
        if (gap < 0.02 && env->player.velocity_z <= 0)
        {
             env->player.height = eye_level;
             env->player.velocity_z = 0;
             env->player.is_falling = 0;
             if (env->debug_physics) printf("[GROUND SNAP] Gap=%.4f (EPS)\n", gap);
        }
        else
        {
             env->player.is_falling = 1; 
        }
    }
    
    
    double ceil_limit = sect->ceiling_height - HEAD_MARGIN;
    if (env->player.height > ceil_limit)
    {
        env->player.height = ceil_limit;
        if (env->player.velocity_z > 0) {
            env->player.velocity_z = 0; 
            if (env->debug_physics) printf("[CEILING HIT] Z=%.3f Ceil=%.3f\n", env->player.height, sect->ceiling_height);
        }
    }

    
    if (env->debug_physics) {
        
        static int f = 0; f++;
        if (f % 10 == 0 || env->single_step_mode) {
             printf("PHYS: z=%.3f vz=%.4f sect=%d fall=%d floor=%.2f ceil=%.2f gap=%.3f\n", 
               env->player.height, env->player.velocity_z, s_id, 
               env->player.is_falling, eye_level - PLAYER_EYE_HEIGHT, sect->ceiling_height, gap);
        }
    }
}

