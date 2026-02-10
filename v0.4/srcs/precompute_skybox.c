#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846


static t_v2 get_corner_position(int i)
{
    t_v2 pos;
    
    
    switch (i)
    {
        case 0:  
            pos.x = 0.0;
            pos.y = 0.0;
            break;
        case 1:  
            pos.x = 10.0;
            pos.y = 0.0;
            break;
        case 2:  
            pos.x = 10.0;
            pos.y = 10.0;
            break;
        case 3:  
            pos.x = 0.0;
            pos.y = 10.0;
            break;
        default:
            pos.x = 0.0;
            pos.y = 0.0;
    }
    
    return pos;
}


static void rotate_vertex(t_env *env, int i)
{
    t_v2 pos = get_corner_position(i);
    double angle = env->player.angle;
    
    
    double tx = pos.x - 5.0;
    double ty = pos.y - 5.0;
    
    
    env->skybox_vertices[i].vx = tx * sin(angle) - ty * cos(angle);
    env->skybox_vertices[i].vz = tx * cos(angle) + ty * sin(angle);
}


static t_v2 get_intersection_near_plane(t_v2 p1, t_v2 p2, double near_z)
{
    t_v2 inter;
    
    
    double t = (near_z - p1.y) / (p2.y - p1.y);
    inter.x = p1.x + t * (p2.x - p1.x);
    inter.y = near_z;
    
    return inter;
}


static void clip_vertex(t_env *env, int i)
{
    t_skybox_vertex *v1 = &env->skybox_vertices[i];
    t_skybox_vertex *v2 = &env->skybox_vertices[i + 1];
    
    double near_z = Z_NEAR_PLANE;
    
    
    if (v1->vz < near_z && v2->vz < near_z)
    {
        v1->draw = 0;
        return;
    }
    
    
    if (v1->vz >= near_z && v2->vz >= near_z)
    {
        v1->draw = 1;
        v1->clipped_vx1 = v1->vx;
        v1->clipped_vz1 = v1->vz;
        v1->clipped_vx2 = v2->vx;
        v1->clipped_vz2 = v2->vz;
        return;
    }
    
    
    v1->draw = 1;
    
    t_v2 p1, p2, inter;
    p1.x = v1->vx;
    p1.y = v1->vz;
    p2.x = v2->vx;
    p2.y = v2->vz;
    
    inter = get_intersection_near_plane(p1, p2, near_z);
    
    
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


static void project_vertex(t_env *env, int i)
{
    t_skybox_vertex *v = &env->skybox_vertices[i];
    
    if (!v->draw) return;
    
    
    double fov_scale = (env->w / 2.0) / tan(30.0 * PI / 180.0);
    
    v->scale1 = fov_scale / v->clipped_vz1;
    v->scale2 = fov_scale / v->clipped_vz2;
    
    
    v->clipped_x1 = (env->w / 2.0) - v->clipped_vx1 * v->scale1;
    v->clipped_x2 = (env->w / 2.0) - v->clipped_vx2 * v->scale2;
    
    
    v->x = (env->w / 2.0) - v->vx * fov_scale / v->vz;
    
    
    double box_bottom = -5.0;
    double box_top = 5.0;
    
    
    v->c1 = (env->h / 2.0) - (box_top - env->player.height) * v->scale1;
    v->c2 = (env->h / 2.0) - (box_top - env->player.height) * v->scale2;
    
    
    v->f1 = (env->h / 2.0) - (box_bottom - env->player.height) * v->scale1;
    v->f2 = (env->h / 2.0) - (box_bottom - env->player.height) * v->scale2;
}


static void precompute_ranges(t_env *env, int i)
{
    t_skybox_vertex *v = &env->skybox_vertices[i];
    t_skybox_vertex *next = &env->skybox_vertices[i + 1];
    
    if (!v->draw) return;
    
    t_v2 pos1 = get_corner_position(i);
    t_v2 pos2 = get_corner_position(i + 1);
    
    
    v->xrange = next->x - v->x;
    v->floor_range = v->f2 - v->f1;
    v->ceiling_range = v->c2 - v->c1;
    v->zrange = v->vz - next->vz;
    v->zcomb = v->vz * next->vz;
    
    
    v->x0z1 = pos1.x * next->vz;
    v->x1z0 = pos2.x * v->vz;
    v->xzrange = v->x1z0 - v->x0z1;
    
    v->y0z1 = pos1.y * next->vz;
    v->y1z0 = pos2.y * v->vz;
    v->yzrange = v->y1z0 - v->y0z1;
}


void precompute_skybox(t_env *env)
{
    if (!env->skybox_enabled) return;
    
    
    for (int i = 0; i < 4; i++)
        rotate_vertex(env, i);
    
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    
    for (int i = 0; i < 4; i++)
        clip_vertex(env, i);
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    
    for (int i = 0; i < 4; i++)
        project_vertex(env, i);
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    
    for (int i = 0; i < 4; i++)
    {
        if (env->skybox_vertices[i].draw)
            precompute_ranges(env, i);
    }
    
    env->skybox_vertices[4] = env->skybox_vertices[0];
    
    
    env->skybox_computed = 1;
}
