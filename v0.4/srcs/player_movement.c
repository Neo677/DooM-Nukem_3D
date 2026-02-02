#include "env.h"
#include <stdio.h>

// Vérifie et applique le déplacement du joueur dans les secteurs
void player_move(t_env *env, double dx, double dy)
{
    // Si pas de secteurs, fallback old system (ou rien)
    if (env->sector_map.nb_sectors == 0) return;
    
    double new_x = env->player.pos.x + dx;
    double new_y = env->player.pos.y + dy;
    
    int current = env->player.current_sector;
    if (current < 0 || current >= env->sector_map.nb_sectors) return;
    
    t_sector *sect = &env->sector_map.sectors[current];
    t_v2 desired_pos = {new_x, new_y};
    
    // 1. Est-ce qu'on est toujours dans le même secteur ?
    // Optimisation: check bounding box first? Non, juste point_in_polygon
    
    // Reconstitution polygone (TODO: stocker polygone dans t_sector pour eviter malloc à chaque frame)
    t_v2 *poly = malloc(sizeof(t_v2) * sect->nb_vertices);
    for(int i=0; i<sect->nb_vertices; i++) {
        t_vertex v = env->sector_map.vertices[sect->vertices[i]];
        poly[i].x = v.x;
        poly[i].y = v.y;
    }
    
    if (point_in_polygon(desired_pos, poly, sect->nb_vertices))
    {
        env->player.pos.x = new_x;
        env->player.pos.y = new_y;
        free(poly);
        return;
    }
    free(poly);
    
    // 2. Si non, est-ce qu'on a traversé un portail ?
    // On regarde les voisins
    for (int i = 0; i < sect->nb_vertices; i++)
    {
        int neighbor = sect->neighbors[i];
        if (neighbor >= 0)
        {
            t_sector *n_sect = &env->sector_map.sectors[neighbor];
            // Check si on est dans ce voisin
            t_v2 *n_poly = malloc(sizeof(t_v2) * n_sect->nb_vertices);
            for(int k=0; k<n_sect->nb_vertices; k++) {
                t_vertex v = env->sector_map.vertices[n_sect->vertices[k]];
                n_poly[k].x = v.x;
                n_poly[k].y = v.y;
            }
            
            if (point_in_polygon(desired_pos, n_poly, n_sect->nb_vertices))
            {
                // Transition réussie !
                env->player.pos.x = new_x;
                env->player.pos.y = new_y;
                env->player.current_sector = neighbor;
                printf("Entrée dans le secteur %d\n", neighbor);
                free(n_poly);
                return;
            }
            free(n_poly);
        }
    }
    
    // 3. Sinon, c'est un mur -> On bloque (Slide collision TODO)
    // Pour l'instant stop net
}
