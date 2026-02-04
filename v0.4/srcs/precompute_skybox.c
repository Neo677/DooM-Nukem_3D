#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

// Helper: Définir les coordonnées des 4 coins de la box
static t_v2 get_corner_position(int i)
{
    t_v2 pos;
    
    // Coins de la box (coordonnées world, centrées autour de (5,5))
    switch (i)
    {
        case 0:  // Coin bas-gauche
            pos.x = 0.0;
            pos.y = 0.0;
            break;
        case 1:  // Coin bas-droit
            pos.x = 10.0;
            pos.y = 0.0;
            break;
        case 2:  // Coin haut-droit
            pos.x = 10.0;
            pos.y = 10.0;
            break;
        case 3:  // Coin haut-gauche
            pos.x = 0.0;
            pos.y = 10.0;
            break;
        default:
            pos.x = 0.0;
            pos.y = 0.0;
    }
    
    return pos;
}

// Étape 1: Rotation des sommets (world → camera space)
static void rotate_vertex(t_env *env, int i)
{
    t_v2 pos = get_corner_position(i);
    double angle = env->player.angle;
    
    // Translation pour centrer autour de (5, 5)
    double tx = pos.x - 5.0;
    double ty = pos.y - 5.0;
    
    // Rotation (formule doom_expl)
    env->skybox_vertices[i].vx = tx * sin(angle) - ty * cos(angle);
    env->skybox_vertices[i].vz = tx * cos(angle) + ty * sin(angle);
}

// Helper: Calcul de l'intersection avec le plan Z-near
static t_v2 get_intersection_near_plane(t_v2 p1, t_v2 p2, double near_z)
{
    t_v2 inter;
    
    // Intersection ligne (p1-p2) avec plan z = near_z
    double t = (near_z - p1.y) / (p2.y - p1.y);
    inter.x = p1.x + t * (p2.x - p1.x);
    inter.y = near_z;
    
    return inter;
}

// Étape 2: Clipping contre le plan Z-near
static void clip_vertex(t_env *env, int i)
{
    t_skybox_vertex *v1 = &env->skybox_vertices[i];
    t_skybox_vertex *v2 = &env->skybox_vertices[i + 1];
    
    double near_z = Z_NEAR_PLANE;
    
    // Vérifier si completement derrière
    if (v1->vz < near_z && v2->vz < near_z)
    {
        v1->draw = 0;
        return;
    }
    
    // Vérifier si completement devant (pas de clipping nécessaire)
    if (v1->vz >= near_z && v2->vz >= near_z)
    {
        v1->draw = 1;
        v1->clipped_vx1 = v1->vx;
        v1->clipped_vz1 = v1->vz;
        v1->clipped_vx2 = v2->vx;
        v1->clipped_vz2 = v2->vz;
        return;
    }
    
    // Clipping partiel nécessaire
    v1->draw = 1;
    
    t_v2 p1, p2, inter;
    p1.x = v1->vx;
    p1.y = v1->vz;
    p2.x = v2->vx;
    p2.y = v2->vz;
    
    inter = get_intersection_near_plane(p1, p2, near_z);
    
    // Déterminer quel vertex clipper
    if (v1->vz < near_z)
    {
        v1->clipped_vx1 = inter.x;
        v1->clipped_vz1 = inter.y;
    }
    else
    {
        v1->clipped_vx1 = v1->vx;
        v1->clipped_vz1 = v1->vz;
    }
    
    if (v2->vz < near_z)
    {
        v1->clipped_vx2 = inter.x;
        v1->clipped_vz2 = inter.y;
    }
    else
    {
        v1->clipped_vx2 = v2->vx;
        v1->clipped_vz2 = v2->vz;
    }
}

// Étape 3: Projection écran
static void project_vertex(t_env *env, int i)
{
    t_skybox_vertex *v = &env->skybox_vertices[i];
    
    if (!v->draw) return;
    
    // Calcul du scale (perspective division)
    double fov_scale = (env->w / 2.0) / tan(30.0 * PI / 180.0);
    
    v->scale1 = fov_scale / v->clipped_vz1;
    v->scale2 = fov_scale / v->clipped_vz2;
    
    // Projection X écran
    v->clipped_x1 = (env->w / 2.0) - v->clipped_vx1 * v->scale1;
    v->clipped_x2 = (env->w / 2.0) - v->clipped_vx2 * v->scale2;
    
    // Position X center (pour sorting/debug)
    v->x = (env->w / 2.0) - v->vx * fov_scale / v->vz;
    
    // Hauteurs plafond/sol (box fixe de -5 à +5 en hauteur)
    double box_bottom = -5.0;
    double box_top = 5.0;
    
    // Projection Y pour le plafond
    v->c1 = (env->h / 2.0) - (box_top - env->player.height) * v->scale1;
    v->c2 = (env->h / 2.0) - (box_top - env->player.height) * v->scale2;
    
    // Projection Y pour le sol
    v->f1 = (env->h / 2.0) - (box_bottom - env->player.height) * v->scale1;
    v->f2 = (env->h / 2.0) - (box_bottom - env->player.height) * v->scale2;
}

// Étape 4: Précalcul des ranges pour interpolation
static void precompute_ranges(t_env *env, int i)
{
    t_skybox_vertex *v = &env->skybox_vertices[i];
    t_skybox_vertex *next = &env->skybox_vertices[i + 1];
    
    if (!v->draw) return;
    
    t_v2 pos1 = get_corner_position(i);
    t_v2 pos2 = get_corner_position(i + 1);
    
    // Ranges pour interpolation
    v->xrange = next->x - v->x;
    v->floor_range = v->f2 - v->f1;
    v->ceiling_range = v->c2 - v->c1;
    v->zrange = v->vz - next->vz;
    v->zcomb = v->vz * next->vz;
    
    // Texture mapping perspective-correct helpers
    v->x0z1 = pos1.x * next->vz;
    v->x1z0 = pos2.x * v->vz;
    v->xzrange = v->x1z0 - v->x0z1;
    
    v->y0z1 = pos1.y * next->vz;
    v->y1z0 = pos2.y * v->vz;
    v->yzrange = v->y1z0 - v->y0z1;
}

// Fonction principale: Precompute skybox
void precompute_skybox(t_env *env)
{
    if (!env->skybox_enabled) return;
    
    // Étape 1: Rotation des 4 coins
    for (int i = 0; i < 4; i++)
        rotate_vertex(env, i);
    
    // Wrap: copier le premier vertex à la position 4
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    // Étape 2: Clipping Z-near
    for (int i = 0; i < 4; i++)
        clip_vertex(env, i);
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    // Étape 3: Projection écran
    for (int i = 0; i < 4; i++)
        project_vertex(env, i);
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    // Étape 4: Precalcul ranges
    for (int i = 0; i < 4; i++)
    {
        if (env->skybox_vertices[i].draw)
            precompute_ranges(env, i);
    }
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    // Marquer comme precompute
    env->skybox_computed = 1;
}
