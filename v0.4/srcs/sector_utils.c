#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int init_sectors(t_env *env)
{
    // Initialisation basique (vide pour l'instant)
    env->sector_map.sectors = NULL;
    env->sector_map.vertices = NULL;
    env->sector_map.nb_sectors = 0;
    env->sector_map.nb_vertices = 0;
    
    printf("Sector system initialized (Empty)\n");
    return (0);
}

void free_sectors(t_env *env)
{
    if (env->sector_map.sectors)
    {
        for (int i = 0; i < env->sector_map.nb_sectors; i++)
        {
            t_sector *s = &env->sector_map.sectors[i];
            if (s->vertices) free(s->vertices);
            if (s->neighbors) free(s->neighbors);
            if (s->wall_textures) free(s->wall_textures);
        }
        free(env->sector_map.sectors);
        env->sector_map.sectors = NULL;
    }
    
    if (env->sector_map.vertices)
    {
        free(env->sector_map.vertices);
        env->sector_map.vertices = NULL;
    }
    
    env->sector_map.nb_sectors = 0;
    env->sector_map.nb_vertices = 0;
    
    DEBUG_LOG("Sector system freed\n");
}

void print_sector_info(t_sector *s)
{
    if (!s) return;
    
    printf("Sector Info:\n");
    printf("  Vertices: %d\n", s->nb_vertices);
    printf("  Floor Height: %.2f\n", s->floor_height);
    printf("  Ceiling Height: %.2f\n", s->ceiling_height);
}

int find_sector(t_env *env, double x, double y)
{
    t_v2 p = {x, y};
    
    for (int i = 0; i < env->sector_map.nb_sectors; i++)
    {
        t_sector *s = &env->sector_map.sectors[i];
        
        // Constuire tableau temporaire de vertices pour collision.c
        // Attention: malloc dans boucle = lent, mais ok pour initialization ou changement rare
        // Optimisation: stack array si max vertices connu, ou check bounding box d'abord
        
        t_v2 *poly = malloc(sizeof(t_v2) * s->nb_vertices);
        if (!poly) continue;
        
        for (int v = 0; v < s->nb_vertices; v++)
        {
            t_vertex vert = env->sector_map.vertices[s->vertices[v]];
            poly[v].x = vert.x;
            poly[v].y = vert.y;
        }
        
        int inside = point_in_polygon(p, poly, s->nb_vertices);
        free(poly);
        
        if (inside) return (i);
    }
    return (-1);
}

// Calculate floor height at a specific position (for Slopes)
double get_sector_floor_height(t_env *env, int sector_id, double x, double y)
{
    if (sector_id < 0 || sector_id >= env->sector_map.nb_sectors)
        return 0.0;
        
    t_sector *sect = &env->sector_map.sectors[sector_id];
    
    // Flat sector
    if (fabs(sect->floor_slope) < 0.01)
        return sect->floor_height;
        
    // Sloped sector
    // We need the vertices of the reference wall (pivot)
    int i = sect->floor_slope_ref_wall;
    if (i < 0 || i >= sect->nb_vertices) i = 0; // fallback
    
    // Get pivot vertex (start of slope)
    t_vertex v0 = env->sector_map.vertices[sect->vertices[i]];
    
    // Get next vertex to form the wall vector
    int j = (i + 1) % sect->nb_vertices;
    t_vertex v1 = env->sector_map.vertices[sect->vertices[j]];
    
    // Calculate wall normal (2D) pointing INWARD or OUTWARD?
    // Wall vector
    double wx = v1.x - v0.x;
    double wy = v1.y - v0.y;
    double len = sqrt(wx*wx + wy*wy);
    if (len == 0) return sect->floor_height;
    
    // Normal (rotate -90 or +90)
    // Assuming standard winding (CCW?), normal inward is (-dy, dx)
    // Let's assume slope increases along this normal.
    double nx = -wy / len;
    double ny = wx / len;
    
    // Project vector (Pos - v0) onto Normal
    double dx = x - v0.x;
    double dy = y - v0.y;
    
    double dist = dx * nx + dy * ny;
    
    // H = Base + Dist * Slope
    return sect->floor_height + dist * sect->floor_slope;
}
