#include "render_sector.h"
#include <math.h>

// Basic fog shading
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

// Bilinear texture sampling for smoother textures
static Uint32 sample_texture_bilinear(t_texture *tex, double u, double v)
{
    // Clamp coordinates to texture bounds
    if (u < 0.0) u = 0.0;
    if (v < 0.0) v = 0.0;
    if (u >= tex->width - 1) u = tex->width - 1.001;
    if (v >= tex->height - 1) v = tex->height - 1.001;
    
    // Get integer and fractional parts
    int u0 = (int)u;
    int v0 = (int)v;
    int u1 = u0 + 1;
    int v1 = v0 + 1;
    
    double fu = u - u0;
    double fv = v - v0;
    
    // Sample 4 neighboring pixels
    Uint32 c00 = tex->pixels[v0 * tex->width + u0];
    Uint32 c10 = tex->pixels[v0 * tex->width + u1];
    Uint32 c01 = tex->pixels[v1 * tex->width + u0];
    Uint32 c11 = tex->pixels[v1 * tex->width + u1];
    
    // Extract RGB components
    Uint8 r00 = (c00 >> 16) & 0xFF, g00 = (c00 >> 8) & 0xFF, b00 = c00 & 0xFF;
    Uint8 r10 = (c10 >> 16) & 0xFF, g10 = (c10 >> 8) & 0xFF, b10 = c10 & 0xFF;
    Uint8 r01 = (c01 >> 16) & 0xFF, g01 = (c01 >> 8) & 0xFF, b01 = c01 & 0xFF;
    Uint8 r11 = (c11 >> 16) & 0xFF, g11 = (c11 >> 8) & 0xFF, b11 = c11 & 0xFF;
    
    // Bilinear interpolation
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

// Fonction de dessin verticale simple (remplace draw_line pour le rasterizer)
void vline(t_env *env, int x, int y1, int y2, Uint32 color, int *ytop, int *ybottom)
{
    // Apply global clipping
    y1 = clamp(y1, ytop[x], ybottom[x]);
    y2 = clamp(y2, ytop[x], ybottom[x]);
    
    if (y2 < y1) return;
    
    // Draw pixels
    for (int y = y1; y <= y2; y++)
    {
        env->sdl.texture_pixels[y * env->w + x] = color;
    }
}

// Dessiner une colonne texturee mur
void vline_textured(t_env *env, int x, int y1, int y2, int y_ceil_unclipped, int y_floor_unclipped, double u, t_texture *tex, int *ytop, int *ybottom, double depth)
{
    // Apply global clipping
    int y_start = clamp(y1, ytop[x], ybottom[x]);
    int y_end = clamp(y2, ytop[x], ybottom[x]);
    
    if (y_end < y_start) return;
    
    double tex_h = (double)tex->height;
    double tex_w = (double)tex->width;
    
    // Wrap U coordinate safely using fmod
    // u is in world units (0.0 to 1.0 = one texture repeat)
    double u_wrapped = fmod(u, 1.0);
    if (u_wrapped < 0) u_wrapped += 1.0;
    
    int u_pix = (int)(u_wrapped * tex_w);
    
    // Safety clamp (just in case)
    if (u_pix < 0) u_pix = 0;
    if (u_pix >= tex->width) u_pix = tex->width - 1;
    
    double wall_h = (double)(y_floor_unclipped - y_ceil_unclipped);
    if (fabs(wall_h) < 0.1) return;
    
    for (int y = y_start; y <= y_end; y++)
    {
        double v = ((double)y - y_ceil_unclipped) / wall_h;
        
        // Safety clamp for v (walls don't wrap vertically)
        if (v < 0.0) v = 0.0;
        if (v > 1.0) v = 1.0;
        
        // Use bilinear filtering for smoother textures
        double u_tex = u_wrapped * tex_w;
        double v_tex = v * tex_h;
        
        Uint32 color = sample_texture_bilinear(tex, u_tex, v_tex);
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, depth);
    }
}

// Dessiner une colonne de sol texturee (Raycasting vertical avec Pentes)
void draw_floor_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect)
{
    (void)ray_angle;
    if (y2 < y1) return;
    
    t_texture *tex = &env->floor_texture;
    int has_texture = (tex->pixels != NULL);
    Uint32 floor_color = 0xFF222222;

    double cam_z = env->player.height; 
    
    // Constants for flat floor
    double floor_h_flat = sect->floor_height;
    double rel_h_flat = cam_z - floor_h_flat; 
    if (rel_h_flat <= Z_NEAR_PLANE && fabs(sect->floor_slope) < COLLISION_EPSILON)
        rel_h_flat = Z_NEAR_PLANE;

    double cos_a = env->cos_table[x];
    double sin_a = env->sin_table[x];
    double screen_dist_const = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0);

    // Slope parameters
    double slope = sect->floor_slope;
    double num = 0, slope_term = 0;
    int is_sloped = (fabs(slope) > 0.001);

    if (is_sloped)
    {
        // Calculate K0, K1
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
            is_sloped = 0; // Fallback
        }
    }

    for (int y = y1; y <= y2; y++)
    {
        double dist;
        
        if (is_sloped)
        {
            double p_val = (double)((env->h / 2) - y) / screen_dist_const;
            // Denom = p_val - slope_term
            double denom = p_val - slope_term;
            
            if (fabs(denom) < 0.0001) dist = 1000.0; // Horizon/Singularity
            else dist = num / denom;
            
            // Fix dist behind player
            if (dist < 0.1) dist = 0.1; // Clamp or skip
        }
        else
        {
            int p = y - env->h / 2;
            if (p == 0) p = 1;
            dist = (rel_h_flat * screen_dist_const) / (double)p;
        }

        double wx = env->player.pos.x + dist * cos_a;
        double wy = env->player.pos.y + dist * sin_a;
        
        Uint32 color;
        if (has_texture)
        {
            double u_tex = wx * tex->width;
            double v_tex = wy * tex->height;
            
            // Wrap coordinates
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

// Dessiner une colonne de plafond texturee
void draw_ceiling_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect)
{
    if (y2 < y1) return;
    
    // NOUVEAU: Skybox override (utilise le nouveau système 3D box)
    if (env->skybox_enabled && env->skybox_computed)
    {
        // Pour l'instant, on active skybox sur tous les plafonds
        // TODO: Ajouter un flag sect->has_skybox pour contrôler par secteur
        draw_skybox_column(env, x, y1, y2, ray_angle);
        return;
    }

    t_texture *tex = &env->ceiling_texture;
    int has_texture = (tex->pixels != NULL);
    Uint32 ceiling_color = 0xFF333333;

    double cam_z = env->player.height; 
    
    // Constants for flat ceiling
    double ceil_h_flat = sect->ceiling_height;
    double rel_h_flat = ceil_h_flat - cam_z; 
    if (rel_h_flat <= Z_NEAR_PLANE && fabs(sect->ceiling_slope) < COLLISION_EPSILON)
        rel_h_flat = Z_NEAR_PLANE;

    double cos_a = env->cos_table[x];
    double sin_a = env->sin_table[x];
    double beta = env->player.angle - ray_angle;
    double cos_beta = cos(beta);
    
    // Optimization: if beta is small, cos(beta) ~ 1. 
    // We could precompute cos_beta too but it depends on ray_angle vs player_angle which is constant per column.
    // Actually, cos_beta IS constant per column X relative to center.
    // But for now let's just use the table for world-space ray vector.
    
    double screen_dist_const = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0);

    // Slope parameters
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
            // y is screen coordinate.
            // (H/2 - y). For ceiling, y < H/2 usually. (H/2 - y) is positive.
            double p_val = (double)((env->h / 2) - y) / screen_dist_const;
            double denom = p_val - slope_term;
            
            if (fabs(denom) < 0.0001) dist = 1000.0; 
            else dist = num / denom;
            
            if (dist < 0.1) dist = 0.1;
        }
        else
        {
            int p = (env->h / 2) - y;
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
            
            // Wrap coordinates
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
