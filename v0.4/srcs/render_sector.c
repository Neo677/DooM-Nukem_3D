#include "render_sector.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

// Structure pour le clipping (interne)
typedef struct {
    int x1, x2;     // Screen coordinates
    int y1a, y2a;   // Ceiling heights (screen)
    int y1b, y2b;   // Floor heights (screen)
    double z1, z2;  // Depths (for perspective correctness)
    double u1, u2;  // Texture coords (linear for now)
} t_wall_segment;

// Helper: Clip value
static int clamp(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// Projection simple et robuste (avec gestion Z near)
// Returns 0 if fully behind, 1 if visible
static int transform_vertex(t_env *env, t_vertex v, double *rx, double *rz)
{
    double tx = v.x - env->player.pos.x;
    double ty = v.y - env->player.pos.y;
    
    double cs = cos(env->player.angle);
    double sn = sin(env->player.angle);
    
    *rz = tx * cs + ty * sn;
    *rx = tx * sn - ty * cs;
    
    // Z-Near clipping tolerance
    return (*rz > Z_NEAR_PLANE);
}

// Fonction de dessin verticale simple (remplace draw_line pour le rasterizer)
static void vline(t_env *env, int x, int y1, int y2, Uint32 color, int *ytop, int *ybottom)
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

// Dessiner une colonne texturee



// Basic fog shading
static Uint32 apply_fog(Uint32 color, double dist)
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

// Dessiner une colonne de sol texturee (Raycasting vertical avec Pentes)
static void draw_floor_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect)
{
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

    double cos_a = cos(ray_angle);
    double sin_a = sin(ray_angle);
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
            
            // Formula: dist = Num / ( (H/2 - y)/Scale - SlopeTerm )
            // Num = BaseH + Slope*K0 - CamZ
            // SlopeTerm = Slope * K1
            // Note: H(dist) = BaseH + Slope*(K0 + dist*K1)
            // y = H/2 - (H(dist) - CamZ)/dist * Scale
            // ...
            // Derivation check:
            // y_screen = (H/2) - (Z_world / dist) * Scale
            // Z_world = H(dist) - CamZ (reversed? No, Z is depth usually, here Z is height difference)
            // Wait, standard projection: y = H/2 - (HeightDiff / Depth) * Scale
            // HeightDiff = H(dist) - CamZ.
            // So: (H/2 - y)/Scale = (H(dist) - CamZ) / dist
            // LHS = (H(dist) - CamZ) / dist
            // LHS = (Base + S*K0 + S*K1*dist - CamZ) / dist
            // LHS = (Base + S*K0 - CamZ)/dist + S*K1
            // LHS - S*K1 = (Base + S*K0 - CamZ)/dist
            // dist = (Base + S*K0 - CamZ) / (LHS - S*K1)
            
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

        // double corrected_dist = dist * inv_cos_beta; // Correct fisheye for texture mapping? - Unused
        
        // However, for texture mapping we might want it?
        // If dist calculated above is perpendicular distance, we need true distance for wX/wY.
        // Actually, ray_angle is absolute. dist above is likely "distance along the ray" if derivation used RayDir.
        // K1 uses cos_a (RayDir). So dist IS distance along ray.
        // But for flat floor: dist = rel_h / pixel_y_ratio. This assumes dist is perpendicular?
        // No, standard Doom: distance = (h * screen_dist) / (y - horiz) / cos(beta).
        // My flat code has `dist /= cos_beta`.
        // My slope code derived `dist` along the ray directly?
        // y = ... (H(dist) - CamZ)/dist. Here dist is along the ray because K1/K0 are world space.
        // So `dist` from slope formula is TRUE distance. No need to divide by cos_beta for geometry.
        
        // However, for texture mapping we might want it?
        
        double wx = env->player.pos.x + dist * cos_a;
        double wy = env->player.pos.y + dist * sin_a;
        
        Uint32 color;
        if (has_texture)
        {
            int tx = (int)(wx * tex->width) % tex->width;
            int ty = (int)(wy * tex->height) % tex->height;
            if (tx < 0) tx += tex->width;
            if (ty < 0) ty += tex->height;
            color = tex->pixels[ty * tex->width + tx];
        }
        else
        {
            color = floor_color;
        }
        
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, dist);
    }
}

// Dessiner une colonne de plafond texturee
static void draw_ceiling_vertical(t_env *env, int x, int y1, int y2, double ray_angle, t_sector *sect)
{
    if (y2 < y1) return;
    
    // NOUVEAU: Skybox override (utilise le nouveau système 3D box)
    if (env->skybox.enabled && env->skybox.computed)
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

    double cos_a = cos(ray_angle);
    double sin_a = sin(ray_angle);
    double beta = env->player.angle - ray_angle;
    double cos_beta = cos(beta);
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
            
            // Formula same as floor but H(dist) is ceiling:
            // (Base + S*K0 + S*K1*dist - CamZ) / dist = (H/2 - y)/Scale
            // Num / dist + SlopeTerm = (H/2 - y)/Scale
            // Num / dist = (H/2 - y)/Scale - SlopeTerm
            // dist = Num / ( (H/2 - y)/Scale - SlopeTerm )
            
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
            int tx = (int)(wx * tex->width) % tex->width;
            int ty = (int)(wy * tex->height) % tex->height;
            if (tx < 0) tx += tex->width;
            if (ty < 0) ty += tex->height;
            color = tex->pixels[ty * tex->width + tx];
        }
        else
        {
            color = ceiling_color;
        }
        
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, dist);
    }
}

// Dessiner une colonne texturee mur
static void vline_textured(t_env *env, int x, int y1, int y2, int y_ceil_unclipped, int y_floor_unclipped, double u, t_texture *tex, int *ytop, int *ybottom, double depth)
{
    // Apply global clipping
    int y_start = clamp(y1, ytop[x], ybottom[x]);
    int y_end = clamp(y2, ytop[x], ybottom[x]);
    
    if (y_end < y_start) return;
    
    double tex_h = (double)tex->height;
    double tex_w = (double)tex->width;
    
    int u_pix = (int)(u * tex_w) % tex->width;
    if (u_pix < 0) u_pix += tex->width;
    
    double wall_h = (double)(y_floor_unclipped - y_ceil_unclipped);
    if (fabs(wall_h) < 0.1) return;
    
    for (int y = y_start; y <= y_end; y++)
    {
        double v = ((double)y - y_ceil_unclipped) / wall_h;
        int v_pix = (int)(v * tex_h) % tex->height;
        if (v_pix < 0) v_pix += tex->height;
        
        Uint32 color = tex->pixels[v_pix * tex->width + u_pix];
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, depth);
    }
}



// Initialise les tableaux de clipping
void init_portal_renderer(t_env *env)
{
    (void)env;
    // Cette fonction pourrait être utilisee pour allouer des buffers si besoin
    // Pour l'instant on passera les buffers sur la stack ou alloues dans render_3d
}

// Cœur du rendu recursif
void render_sectors_recursive(t_env *env, int sector_id, int xmin, int xmax, int *ytop, int *ybottom, int depth)
{
    if (sector_id < 0 || sector_id >= env->sector_map.nb_sectors) return;
    if (depth > MAX_RECURSION_DEPTH) return;
    
    // Limite recursion
    // Note: Utiliser un compteur global ou passer depth en argument serait mieux
    // Pour l'instant on fait confiance aux cycles (TODO: Ajouter depth)
    
    t_sector *sect = &env->sector_map.sectors[sector_id];
    
    // Rendering chaque mur
    for (int i = 0; i < sect->nb_vertices; i++)
    {
        // 1. Recuperer les sommets
        int idx1 = sect->vertices[i];
        int idx2 = sect->vertices[(i + 1) % sect->nb_vertices];
        
        t_vertex v1 = env->sector_map.vertices[idx1];
        t_vertex v2 = env->sector_map.vertices[idx2];
        
        // 2. Transformer (World -> Cameraview)
        double rz1, rx1, rz2, rx2;
        transform_vertex(env, v1, &rx1, &rz1);
        transform_vertex(env, v2, &rx2, &rz2);
        
        // 3. Clipping Z-Near (Doit être fait avant la projection ecran !)
        if (rz1 <= Z_NEAR_PLANE && rz2 <= Z_NEAR_PLANE) continue; // Mur entierement derriere
        
        // Clipping 'Near Plane' basique (point derriere -> intersecte a z=near)
        if (rz1 <= Z_NEAR_PLANE || rz2 <= Z_NEAR_PLANE)
        {
            double alpha = (Z_NEAR_PLANE - rz1) / (rz2 - rz1);
            double int_rx = rx1 + (rx2 - rx1) * alpha;
            
            if (rz1 <= Z_NEAR_PLANE) { rz1 = Z_NEAR_PLANE; rx1 = int_rx; }
            else                     { rz2 = Z_NEAR_PLANE; rx2 = int_rx; }
        }
        
        // 4. Projection ecran
        double scale1 = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0); // FOVH
        double scale2 = scale1;
        
        int sx1 = (int)((env->w / 2) - (rx1 / rz1) * scale1);
        int sx2 = (int)((env->w / 2) - (rx2 / rz2) * scale2);
        
        // 5. Backface culling ou validation horizontale
        if (sx1 >= sx2 || sx2 <= xmin || sx1 >= xmax) continue;
        
        // 6. Calcul des hauteurs plafond/sol (Projected heights)
        // Utilisation de la hauteur relative joueur
        double ceil_h = sect->ceiling_height - env->player.height;
        double floor_h = sect->floor_height - env->player.height;
        
        // Projection Y (similaire a Doom)
        // Yscreen = (H / 2) - (Height / Z) * Scale
        int sy1_ceil = (env->h / 2) - (int)(ceil_h / rz1 * scale1);
        int sy1_floor = (env->h / 2) - (int)(floor_h / rz1 * scale1);
        int sy2_ceil = (env->h / 2) - (int)(ceil_h / rz2 * scale2);
        int sy2_floor = (env->h / 2) - (int)(floor_h / rz2 * scale2);
        
        // Hauteurs pour le voisin (si portail)
        int n_sy1_ceil = 0, n_sy1_floor = 0, n_sy2_ceil = 0, n_sy2_floor = 0;
        int neighbor = sect->neighbors[i];
        if (neighbor >= 0)
        {
            double n_ceil_h = env->sector_map.sectors[neighbor].ceiling_height - env->player.height;
            double n_floor_h = env->sector_map.sectors[neighbor].floor_height - env->player.height;
            n_sy1_ceil = (env->h / 2) - (int)(n_ceil_h / rz1 * scale1);
            n_sy1_floor = (env->h / 2) - (int)(n_floor_h / rz1 * scale1);
            n_sy2_ceil = (env->h / 2) - (int)(n_ceil_h / rz2 * scale2);
            n_sy2_floor = (env->h / 2) - (int)(n_floor_h / rz2 * scale2);
        }
        
        // 7. Rasterization colonne par colonne (X Loop)
        // Clamp X range
        int begin_x = clamp(sx1, xmin, xmax);
        int end_x = clamp(sx2, xmin, xmax);
        
        for (int x = begin_x; x < end_x; x++)
        {
            // Interpolation lineaire (Simple mais pas perspective-correct pour textures, ok pour geometrie)
            // Pour être precis, il faut interpoler 1/z
            double t = (double)(x - sx1) / (sx2 - sx1);
            
            // Hauteur plafond/sol actuels
            int y_ceil = sy1_ceil + (sy2_ceil - sy1_ceil) * t;
            int y_floor = sy1_floor + (sy2_floor - sy1_floor) * t;

// ... inside render_sectors_recursive loop ...

            // Clip against current viewport y-buffers
            int c_y_ceil = clamp(y_ceil, ytop[x], ybottom[x]);
            int c_y_floor = clamp(y_floor, ytop[x], ybottom[x]);
            
            // Texture lookup
            t_texture *wall_tex = NULL;
            if (env->num_textures > 0 && sect->wall_textures)
            {
                int tex_id = sect->wall_textures[i];
                if (tex_id >= 0 && tex_id < env->num_textures)
                    wall_tex = &env->wall_textures[tex_id];
            }
            
            // Perspective correct U mapping
            double wall_len = hypot(v2.x - v1.x, v2.y - v1.y);
            double tex_scale = 1.0; 
            
            double iz1 = 1.0 / rz1;
            double iz2 = 1.0 / rz2;
            double uz1 = 0.0 * iz1; 
            double uz2 = (wall_len * tex_scale) * iz2; 
            
            double iz = iz1 + (iz2 - iz1) * t;
            double uz = uz1 + (uz2 - uz1) * t;
            double u_current = uz / iz;
            
            // Depth for shading
            double z_current = 1.0 / iz;

            // Angle Rayon pour Sol/Plafond
            double fov_rad_col = 60.0 * 3.14159 / 180.0;
            double ray_angle_col = env->player.angle - (fov_rad_col / 2.0) + ((double)x / env->w) * fov_rad_col;
            
            // -- Rendu --
            
            // Plafond Texture
            if (c_y_ceil > ytop[x])
                draw_ceiling_vertical(env, x, ytop[x], c_y_ceil, ray_angle_col, sect);

            // Sol Texture
            if (c_y_floor < ybottom[x])
                draw_floor_vertical(env, x, c_y_floor, ybottom[x], ray_angle_col, sect);
            
            if (neighbor >= 0) // C'est un PORTAIL
            {
                // Interpolation hauteurs voisin
                int ny_ceil = n_sy1_ceil + (n_sy2_ceil - n_sy1_ceil) * t;
                int ny_floor = n_sy1_floor + (n_sy2_floor - n_sy1_floor) * t;
                
                int c_ny_ceil = clamp(ny_ceil, ytop[x], ybottom[x]);
                int c_ny_floor = clamp(ny_floor, ytop[x], ybottom[x]);
                
                // Dessin du mur du haut ("Upper Wall")
                if (c_ny_ceil > c_y_ceil) {
                     if (wall_tex) vline_textured(env, x, c_y_ceil, c_ny_ceil, y_ceil, y_floor, u_current, wall_tex, ytop, ybottom, z_current);
                     else vline(env, x, c_y_ceil, c_ny_ceil, apply_fog(0xFF888888, z_current), ytop, ybottom);
                }
                
                // Dessin du mur du bas ("Lower Wall")
                if (c_ny_floor < c_y_floor) {
                     if (wall_tex) vline_textured(env, x, c_ny_floor, c_y_floor, y_ceil, y_floor, u_current, wall_tex, ytop, ybottom, z_current);
                     else vline(env, x, c_ny_floor, c_y_floor, apply_fog(0xFF666666, z_current), ytop, ybottom);
                }
                
                // Ne PAS mettre a jour ytop/ybottom ici directement pour la suite de CE secteur
                // car cela affecterait les murs suivants.
                // Par contre, pour la RECURSION, on doit calculer la nouvelle fenêtre.
            }
            else // C'est un MUR SOLIDE
            {
                if (wall_tex)
                    vline_textured(env, x, c_y_ceil, c_y_floor, y_ceil, y_floor, u_current, wall_tex, ytop, ybottom, z_current);
                else
                    vline(env, x, c_y_ceil, c_y_floor, apply_fog(0xFFAAAAAA, z_current), ytop, ybottom);
            }
        }
        
        // RECURSION SAFE
        if (neighbor >= 0 && end_x > begin_x && depth < MAX_RECURSION_DEPTH)
        {
            // Allocation de nouveaux buffers de clipping pour le secteur voisin
            int *new_ytop = (int *)malloc(sizeof(int) * env->w);
            int *new_ybottom = (int *)malloc(sizeof(int) * env->w);
            
            if (new_ytop && new_ybottom)
            {
                // Copie des contraintes actuelles
                memcpy(new_ytop, ytop, sizeof(int) * env->w);
                memcpy(new_ybottom, ybottom, sizeof(int) * env->w);
                
                // Application de la fenêtre du portail SUR LA COPIE
                for (int x = begin_x; x < end_x; x++)
                {
                     // Recalcul necessaire des hauteurs (on pourrait les stocker, mais recalculer 
                     // est plus simple et moins gourmand en memoire heap/stack arrays)
                     double t = (double)(x - sx1) / (sx2 - sx1);
                     
                     int y_ceil = sy1_ceil + (sy2_ceil - sy1_ceil) * t;
                     int y_floor = sy1_floor + (sy2_floor - sy1_floor) * t;
                     
                     int ny_ceil = n_sy1_ceil + (n_sy2_ceil - n_sy1_ceil) * t;
                     int ny_floor = n_sy1_floor + (n_sy2_floor - n_sy1_floor) * t;
                     
                     // Clipping ecran actuel
                     int c_y_ceil = clamp(y_ceil, ytop[x], ybottom[x]);
                     int c_y_floor = clamp(y_floor, ytop[x], ybottom[x]);
                     int c_ny_ceil = clamp(ny_ceil, ytop[x], ybottom[x]);
                     int c_ny_floor = clamp(ny_floor, ytop[x], ybottom[x]);

                     // La fenêtre visible a travers le portail est l'intersection de :
                     // 1. Fenêtre courante (new_ytop[x], new_ybottom[x])
                     // 2. Le trou geometrique du portail (max des ceilings, min des floors)
                     
                     int portal_top = MAX(c_y_ceil, c_ny_ceil);
                     int portal_bot = MIN(c_y_floor, c_ny_floor);
                     
                     new_ytop[x] = clamp(portal_top, new_ytop[x], env->h - 1);
                     new_ybottom[x] = clamp(portal_bot, new_ytop[x], new_ybottom[x]);
                }
                
                // Appel recursif
                render_sectors_recursive(env, neighbor, begin_x, end_x, new_ytop, new_ybottom, depth + 1);
                
                free(new_ytop);
                free(new_ybottom);
            }
        }
    }
}

#undef MAX_RECURSION_DEPTH
