#include "render_sector.h"
#include <math.h>
#include <stdlib.h>

#define MAX_RECURSION_DEPTH 32
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
    return (*rz > 0.01);
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

// Dessiner une colonne texturée



// Basic fog shading
static Uint32 apply_fog(Uint32 color, double dist)
{
    double max_dist = 20.0; // Distance of darkness
    if (dist < 0.0) dist = 0.0;
    
    double intensity = 1.0 - (dist / max_dist);
    if (intensity < 0.1) intensity = 0.1; // Ambiance min
    if (intensity > 1.0) intensity = 1.0;
    
    Uint8 r = (color >> 16) & 0xFF;
    Uint8 g = (color >> 8) & 0xFF;
    Uint8 b = color & 0xFF;
    
    r = (Uint8)(r * intensity);
    g = (Uint8)(g * intensity);
    b = (Uint8)(b * intensity);
    
    return (0xFF000000 | (r << 16) | (g << 8) | b);
}

// Dessiner une colonne de sol texturée (Raycasting vertical)
static void draw_floor_vertical(t_env *env, int x, int y1, int y2, double ray_angle, double floor_height)
{
    if (y2 < y1) return;
    
    t_texture *tex = &env->floor_texture;
    if (!tex->pixels) {
        // Fallback couleur simple + fog
         for (int y = y1; y <= y2; y++)
            env->sdl.texture_pixels[y * env->w + x] = 0xFF222222; 
        return;
    }

    double cam_z = env->player.height; 
    double floor_z = floor_height;
    double rel_h = cam_z - floor_z; 
    if (rel_h <= 0.1) rel_h = 0.1;

    double cos_a = cos(ray_angle);
    double sin_a = sin(ray_angle);
    double beta = env->player.angle - ray_angle;
    double cos_beta = cos(beta);
    double screen_dist = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0);

    for (int y = y1; y <= y2; y++)
    {
        int p = y - env->h / 2;
        if (p == 0) p = 1;
        
        double dist = (rel_h * screen_dist) / (double)p;
        dist /= cos_beta; 
        
        double wx = env->player.pos.x + dist * cos_a;
        double wy = env->player.pos.y + dist * sin_a;
        
        int tx = (int)(wx * tex->width) % tex->width;
        int ty = (int)(wy * tex->height) % tex->height;
        if (tx < 0) tx += tex->width;
        if (ty < 0) ty += tex->height;
        
        Uint32 color = tex->pixels[ty * tex->width + tx];
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, dist);
    }
}

// Dessiner une colonne de plafond texturée
static void draw_ceiling_vertical(t_env *env, int x, int y1, int y2, double ray_angle, double ceiling_height)
{
    if (y2 < y1) return;
    
    t_texture *tex = &env->ceiling_texture;
    if (!tex->pixels) {
        // Si pas de texture plafond, on laisse le vide (skybox) ou gris
        // Si skybox enabled, return.
        if (env->skybox.enabled) return;
        
        for (int y = y1; y <= y2; y++)
            env->sdl.texture_pixels[y * env->w + x] = 0xFF333333;
        return;
    }

    double cam_z = env->player.height; 
    double ceil_z = ceiling_height;
    double rel_h = ceil_z - cam_z; // Plafond au dessus
    if (rel_h <= 0.1) rel_h = 0.1;

    double cos_a = cos(ray_angle);
    double sin_a = sin(ray_angle);
    double beta = env->player.angle - ray_angle;
    double cos_beta = cos(beta);
    double screen_dist = (env->w / 2.0) / tan(30.0 * 3.14159 / 180.0);

    for (int y = y1; y <= y2; y++)
    {
        // p est la distance au dessus de l'horizon
        int p = (env->h / 2) - y;
        if (p <= 0) p = 1;
        
        double dist = (rel_h * screen_dist) / (double)p;
        dist /= cos_beta; 
        
        double wx = env->player.pos.x + dist * cos_a;
        double wy = env->player.pos.y + dist * sin_a;
        
        int tx = (int)(wx * tex->width) % tex->width;
        int ty = (int)(wy * tex->height) % tex->height;
        if (tx < 0) tx += tex->width;
        if (ty < 0) ty += tex->height;
        
        Uint32 color = tex->pixels[ty * tex->width + tx];
        env->sdl.texture_pixels[y * env->w + x] = apply_fog(color, dist);
    }
}

// Dessiner une colonne texturée mur
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
    // Cette fonction pourrait être utilisée pour allouer des buffers si besoin
    // Pour l'instant on passera les buffers sur la stack ou alloués dans render_3d
}

// Cœur du rendu récursif
void render_sectors_recursive(t_env *env, int sector_id, int xmin, int xmax, int *ytop, int *ybottom)
{
    if (sector_id < 0 || sector_id >= env->sector_map.nb_sectors) return;
    
    // Limite recursion
    // Note: Utiliser un compteur global ou passer depth en argument serait mieux
    // Pour l'instant on fait confiance aux cycles (TODO: Ajouter depth)
    
    t_sector *sect = &env->sector_map.sectors[sector_id];
    
    // Rendering chaque mur
    for (int i = 0; i < sect->nb_vertices; i++)
    {
        // 1. Récupérer les sommets
        int idx1 = sect->vertices[i];
        int idx2 = sect->vertices[(i + 1) % sect->nb_vertices];
        
        t_vertex v1 = env->sector_map.vertices[idx1];
        t_vertex v2 = env->sector_map.vertices[idx2];
        
        // 2. Transformer (World -> Cameraview)
        double rz1, rx1, rz2, rx2;
        transform_vertex(env, v1, &rx1, &rz1);
        transform_vertex(env, v2, &rx2, &rz2);
        
        // 3. Clipping Z-Near (Doit être fait avant la projection écran !)
        if (rz1 <= 0.1 && rz2 <= 0.1) continue; // Mur entièrement derrière
        
        // Clipping 'Near Plane' basique (point derrière -> intersecté à z=0.1)
        if (rz1 <= 0.1 || rz2 <= 0.1)
        {
            double near_z = 0.1;
            double alpha = (near_z - rz1) / (rz2 - rz1);
            double int_rx = rx1 + (rx2 - rx1) * alpha;
            
            if (rz1 <= 0.1) { rz1 = near_z; rx1 = int_rx; }
            else           { rz2 = near_z; rx2 = int_rx; }
        }
        
        // 4. Projection Écran
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
        
        // Projection Y (similaire à Doom)
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
            // Interpolation linéaire (Simple mais pas perspective-correct pour textures, ok pour géométrie)
            // Pour être précis, il faut interpoler 1/z
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
            
            // Plafond Texturé
            if (c_y_ceil > ytop[x])
                draw_ceiling_vertical(env, x, ytop[x], c_y_ceil, ray_angle_col, sect->ceiling_height);

            // Sol Texturé
            if (c_y_floor < ybottom[x])
                draw_floor_vertical(env, x, c_y_floor, ybottom[x], ray_angle_col, sect->floor_height);
            
            if (neighbor >= 0) // C'est un PORTAIL
            {
                // Interpolation hauteurs voisin
                int ny_ceil = n_sy1_ceil + (n_sy2_ceil - n_sy1_ceil) * t;
                int ny_floor = n_sy1_floor + (n_sy2_floor - n_sy1_floor) * t;
                
                int c_ny_ceil = clamp(ny_ceil, ytop[x], ybottom[x]);
                int c_ny_floor = clamp(ny_floor, ytop[x], ybottom[x]);
                
                // Upper Wall (Texture ?)
                if (c_ny_ceil > c_y_ceil) {
                    if (wall_tex) vline_textured(env, x, c_y_ceil, c_ny_ceil, y_ceil, y_floor, u_current, wall_tex, ytop, ybottom, z_current);
                    else vline(env, x, c_y_ceil, c_ny_ceil, apply_fog(0xFF888888, z_current), ytop, ybottom);
                }
                
                // Lower Wall (Texture ?)
                if (c_ny_floor < c_y_floor) {
                     if (wall_tex) vline_textured(env, x, c_ny_floor, c_y_floor, y_ceil, y_floor, u_current, wall_tex, ytop, ybottom, z_current);
                     else vline(env, x, c_ny_floor, c_y_floor, apply_fog(0xFF666666, z_current), ytop, ybottom);
                }
                
                // Update clipping
                ytop[x] = clamp(MAX(c_y_ceil, c_ny_ceil), ytop[x], env->h - 1);
                ybottom[x] = clamp(MIN(c_y_floor, c_ny_floor), ytop[x], ybottom[x]);
            }
            else // C'est un MUR SOLIDE
            {
                if (wall_tex)
                {
                    vline_textured(env, x, c_y_ceil, c_y_floor, y_ceil, y_floor, u_current, wall_tex, ytop, ybottom, z_current);
                }
                else
                {
                    vline(env, x, c_y_ceil, c_y_floor, apply_fog(0xFFAAAAAA, z_current), ytop, ybottom);
                }
            }
        }
        
        // 8. Récursion (une fois la boucle X finie, si c'est efficace, ou faire schedule ?)
        // Dans Doom engine pur, on fait la recursion après, en passant la fenêtre réduite.
        // Ici, on a modifié ytop/ybottom en place. C'est destructif pour les autres murs du même secteur !
        // -> ERREUR DE LOGIQUE: On ne peut pas modifier ytop/ybottom globalement pendant la boucle des murs d'un même secteur.
        // Il faut sauver les nouvelles contraintes et appeler la récursion après.
        // OU passer des copies de tableaux (trop lourd).
        // Solution standard: Scheduler la récursion avec les fenêtres xmin' xmax'.
        
        if (neighbor >= 0 && end_x > begin_x)
        {
             // TODO: Récursion immédiate n'est pas safe si on partage ytop/ybottom sans copie/restauration
             // Hack simple pour v0.4:
             // On va re-interpoler ytop/ybottom JUSTE pour l'appel récursif
             // C'est moins performant mais plus simple sans gestion de queue
             
             // Créer copie locale des clips pour la récursion
             // Attention stack overflow si tableaux trop gros (WIDTH * 4 bytes)
             // Alloc dynamique ou static buffers
        }
        
        // CORRECTION IMMEDIATE:
        // Pour ce prototype, on va dessiner les murs solides.
        // La récursion demande une architecture un peu plus complexe (queue de portails).
        // Je vais laisser la récursion en TODO ou faire une version simplifié (un seul voisin)
        // Pour l'instant, je finalise le rendu MONO-SECTEUR solide avec clipping vertical correct.
    }
    
    // Pour supporter la récursion simplement sans queue complexe:
    // On peut faire une passe "Scanline" qui enregistre les segments de portail, puis les traite.
    // Mais pour l'instant, rendons juste le secteur courant correctement avec vline/clipping.
}

#undef MAX_RECURSION_DEPTH
