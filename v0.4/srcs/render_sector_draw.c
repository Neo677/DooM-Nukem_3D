#include "render_sector.h"
#include <math.h>


Uint32 apply_fog(Uint32 color, double dist)
{
    if (dist < 0.0) dist = 0.0;
    
    double intensity = 1.0 - (dist / FOG_MAX_DISTANCE);
    if (intensity < FOG_MIN_INTENSITY) intensity = FOG_MIN_INTENSITY;
    if (intensity > 1.0) intensity = 1.0;
    
    Uint8 r = (color >> 16) & 0xFF;
    Uint8 g = (color >> 8) & 0xFF;
    Uint8 b = color & 0xFF;
    
    r = (Uint8)(r * intensity);
    g = (Uint8)(g * intensity);
    b = (Uint8)(b * intensity);
    
    return (0xFF000000 | (r << 16) | (g << 8) | b);
}


static Uint32 sample_texture_bilinear(t_texture *tex, double u, double v)
{
    
    if (u < 0.0) u = 0.0;
    if (v < 0.0) v = 0.0;
    if (u >= tex->width - 1) u = tex->width - 1.001;
    if (v >= tex->height - 1) v = tex->height - 1.001;
    
    
    int u0 = (int)u;
    int v0 = (int)v;
    int u1 = u0 + 1;
    int v1 = v0 + 1;
    
    double fu = u - u0;
    double fv = v - v0;
    
    
    Uint32 c00 = tex->pixels[v0 * tex->width + u0];
    Uint32 c10 = tex->pixels[v0 * tex->width + u1];
    Uint32 c01 = tex->pixels[v1 * tex->width + u0];
    Uint32 c11 = tex->pixels[v1 * tex->width + u1];
    
    
    Uint8 r00 = (c00 >> 16) & 0xFF, g00 = (c00 >> 8) & 0xFF, b00 = c00 & 0xFF;
    Uint8 r10 = (c10 >> 16) & 0xFF, g10 = (c10 >> 8) & 0xFF, b10 = c10 & 0xFF;
    Uint8 r01 = (c01 >> 16) & 0xFF, g01 = (c01 >> 8) & 0xFF, b01 = c01 & 0xFF;
    Uint8 r11 = (c11 >> 16) & 0xFF, g11 = (c11 >> 8) & 0xFF, b11 = c11 & 0xFF;
    
    
    double r0 = r00 * (1.0 - fu) + r10 * fu;
    double r1 = r01 * (1.0 - fu) + r11 * fu;
    double r = r0 * (1.0 - fv) + r1 * fv;
    
    double g0 = g00 * (1.0 - fu) + g10 * fu;
    double g1 = g01 * (1.0 - fu) + g11 * fu;
    double g = g0 * (1.0 - fv) + g1 * fv;
    
    double b0 = b00 * (1.0 - fu) + b10 * fu;
    double b1 = b01 * (1.0 - fu) + b11 * fu;
    double b = b0 * (1.0 - fv) + b1 * fv;
    
    return (0xFF000000 | ((Uint8)r << 16) | ((Uint8)g << 8) | (Uint8)b);
}


void vline(t_env *env, int x, int y1, int y2, Uint32 color, int *ytop, int *ybottom)
{
    
    y1 = clamp(y1, ytop[x], ybottom[x]);
    y2 = clamp(y2, ytop[x], ybottom[x]);
    
    if (y2 < y1) return;
    
    
    for (int y = y1; y <= y2; y++)
    {
        env->sdl.texture_pixels[y * env->w + x] = color;
    }
}


void vline_textured(t_env *env, int x, int y1, int y2, int y_ceil_unclipped, int y_floor_unclipped, double u, t_texture *tex, int *ytop, int *ybottom, double depth)
{
    
    int y_start = clamp(y1, ytop[x], ybottom[x]);
    int y_end = clamp(y2, ytop[x], ybottom[x]);
    
    if (y_end < y_start) return;
    
    double tex_h = (double)tex->height;
    double tex_w = (double)tex->width;
    
    
    
    double u_wrapped = fmod(u, 1.0);
    if (u_wrapped < 0) u_wrapped += 1.0;
    if (isnan(u_wrapped) || isinf(u_wrapped)) u_wrapped = 0.0;
        
        int u_pix = (int)(u_wrapped * tex_w);
        
        
        if (u_pix < 0) u_pix = 0;
        if (u_pix >= tex->width) u_pix = tex->width - 1;
        
        double wall_h = (double)(y_floor_unclipped - y_ceil_unclipped);
        if (fabs(wall_h) < 0.1) return;
        
        for (int y = y_start; y <= y_end; y++)
        {
            double v = ((double)y - y_ceil_unclipped) / wall_h;
            
            
            if (v < 0.0) v = 0.0;
            if (v > 1.0) v = 1.0;
            
            if (isnan(v)) v = 0.0;
            
            
            double u_tex = u_wrapped * tex_w;
            double v_tex = v * tex_h;
            
            Uint32 color = sample_texture_bilinear(tex, u_tex, v_tex);
            env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, depth);
        }
}


void draw_floor_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect)
{
    (void)ray_angle;
    if (y2 < y1) return;
    
    t_texture *tex = &env->floor_texture;
    int has_texture = (tex->pixels != NULL);
    Uint32 floor_color = 0xFF222222;

    double cam_z = env->player.height; 
    
    
    double floor_h_flat = sect->floor_height;
    double rel_h_flat = cam_z - floor_h_flat; 
    if (rel_h_flat <= Z_NEAR_PLANE && fabs(sect->floor_slope) < COLLISION_EPSILON)
        rel_h_flat = Z_NEAR_PLANE;

    double cos_a = env->cos_table[x];
    double sin_a = env->sin_table[x];
    double beta = env->player.angle - ray_angle;
    double cos_beta = cos(beta);

    double screen_dist_const = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0);

    
    double slope = sect->floor_slope;
    double num = 0, slope_term = 0;
    int is_sloped = (fabs(slope) > 0.001);

    if (is_sloped)
    {
        
        int i = sect->floor_slope_ref_wall;
        if (i < 0 || i >= sect->nb_vertices) i = 0;
        
        t_vertex v0 = env->sector_map.vertices[sect->vertices[i]];
        t_vertex v1 = env->sector_map.vertices[sect->vertices[(i + 1) % sect->nb_vertices]];
        
        double wx = v1.x - v0.x;
        double wy = v1.y - v0.y;
        double len = sqrt(wx*wx + wy*wy);
        
        if (len > 0.0001) {
            double nx = -wy / len;
            double ny = wx / len;
            
            double dx = env->player.pos.x - v0.x;
            double dy = env->player.pos.y - v0.y;
            
            double k0 = dx * nx + dy * ny;
            double k1 = cos_a * nx + sin_a * ny;
            
            num = sect->floor_height + slope * k0 - cam_z;
            slope_term = slope * k1;
        } else {
            is_sloped = 0; 
        }
    }

    for (int y = y1; y <= y2; y++)
    {
        double dist = 1000.0; 
        
        if (is_sloped)
        {
            double p_val = (double)((int)env->player.horizon - y) / screen_dist_const;
            
            double denom = p_val - slope_term;
            
            if (fabs(denom) < 0.0001) dist = 1000.0; 
            else dist = num / denom;
            
            
            if (dist < 0.1) dist = 0.1; 
        }
        else
        {
            int p = y - (int)env->player.horizon;
            if (p == 0) p = 1;
            dist = (rel_h_flat * screen_dist_const) / (double)p;
            
            dist /= cos_beta;
        }

        if (isnan(dist) || isinf(dist)) dist = 1000.0;

        double wx = env->player.pos.x + dist * cos_a;
        double wy = env->player.pos.y + dist * sin_a;
        
        Uint32 color;
        if (has_texture)
        {
            double u_tex = wx * tex->width;
            double v_tex = wy * tex->height;
            
            
            if (isnan(u_tex)) u_tex = 0;
            if (isnan(v_tex)) v_tex = 0;

            
            u_tex = fmod(u_tex, (double)tex->width);
            v_tex = fmod(v_tex, (double)tex->height);
            if (u_tex < 0) u_tex += tex->width;
            if (v_tex < 0) v_tex += tex->height;
            
            color = sample_texture_bilinear(tex, u_tex, v_tex);
        }
        else
        {
            color = floor_color;
        }
        
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, dist);
    }
}


void draw_ceiling_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect)
{
    if (y2 < y1) return;
    
    
    if (env->skybox_enabled && env->skybox_computed)
    {
        
        
        draw_skybox_column(env, x, y1, y2, ray_angle);
        return;
    }

    t_texture *tex = &env->ceiling_texture;
    int has_texture = (tex->pixels != NULL);
    Uint32 ceiling_color = 0xFF333333;

    double cam_z = env->player.height; 
    
    
    double ceil_h_flat = sect->ceiling_height;
    double rel_h_flat = ceil_h_flat - cam_z; 
    if (rel_h_flat <= Z_NEAR_PLANE && fabs(sect->ceiling_slope) < COLLISION_EPSILON)
        rel_h_flat = Z_NEAR_PLANE;

    double cos_a = env->cos_table[x];
    double sin_a = env->sin_table[x];
    double beta = env->player.angle - ray_angle;
    double cos_beta = cos(beta);
    
    
    
    
    
    
    double screen_dist_const = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0);

    
    double slope = sect->ceiling_slope;
    double num = 0, slope_term = 0;
    int is_sloped = (fabs(slope) > 0.001);

    if (is_sloped)
    {
        int i = sect->ceiling_slope_ref_wall;
        if (i < 0 || i >= sect->nb_vertices) i = 0;
        
        t_vertex v0 = env->sector_map.vertices[sect->vertices[i]];
        t_vertex v1 = env->sector_map.vertices[sect->vertices[(i + 1) % sect->nb_vertices]];
        
        double wx = v1.x - v0.x;
        double wy = v1.y - v0.y;
        double len = sqrt(wx*wx + wy*wy);
        
        if (len > 0.0001) {
            double nx = -wy / len;
            double ny = wx / len;
            
            double dx = env->player.pos.x - v0.x;
            double dy = env->player.pos.y - v0.y;
            
            double k0 = dx * nx + dy * ny;
            double k1 = cos_a * nx + sin_a * ny;
            
            num = sect->ceiling_height + slope * k0 - cam_z;
            slope_term = slope * k1;
        } else {
            is_sloped = 0;
        }
    }

    for (int y = y1; y <= y2; y++)
    {
        double dist;
        
        if (is_sloped)
        {
            
            
            
            double p_val = (double)((int)env->player.horizon - y) / screen_dist_const;
            double denom = p_val - slope_term;
            
            if (fabs(denom) < 0.0001) dist = 1000.0; 
            else dist = num / denom;
            
            if (dist < 0.1) dist = 0.1;
        }
        else
        {
            int p = (int)env->player.horizon - y;
            if (p <= 0) p = 1;
            dist = (rel_h_flat * screen_dist_const) / (double)p;
            dist /= cos_beta; 
        }

        double wx = env->player.pos.x + dist * cos_a;
        double wy = env->player.pos.y + dist * sin_a;
        
        Uint32 color;
        if (has_texture)
        {
            double u_tex = wx * tex->width;
            double v_tex = wy * tex->height;
            
            
            u_tex = fmod(u_tex, (double)tex->width);
            v_tex = fmod(v_tex, (double)tex->height);
            if (u_tex < 0) u_tex += tex->width;
            if (v_tex < 0) v_tex += tex->height;
            
            color = sample_texture_bilinear(tex, u_tex, v_tex);
        }
        else
        {
            color = ceiling_color;
        }
        
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, dist);
    }
}
