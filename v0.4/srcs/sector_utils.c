#include "env.h"
#include <stdio.h>
#include <stdlib.h>

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
    }
    
    if (env->sector_map.vertices)
        free(env->sector_map.vertices);
        
    printf("Sector system freed\n");
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
