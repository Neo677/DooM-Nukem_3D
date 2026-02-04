#include "render_sector.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

void init_portal_renderer(t_env *env)
{
    (void)env;
    // No initialization needed for pool (done in init_game)
}

// Fonction principale de la boucle de mur (Wall Loop)
// Executee pour chaque colonne X du mur visible
static void render_wall_loop(t_render_context *ctx)
{
    t_env *env = ctx->env;
    t_sector *sect = ctx->sect;
    int neighbor = ctx->neighbor;

    for (int x = ctx->begin_x; x < ctx->end_x; x++)
    {
        // Interpolation lineaire
        double t = (double)(x - ctx->sx1) / (ctx->sx2 - ctx->sx1);
        
        // Hauteur plafond/sol actuels
        int y_ceil = ctx->sy1_ceil + (ctx->sy2_ceil - ctx->sy1_ceil) * t;
        int y_floor = ctx->sy1_floor + (ctx->sy2_floor - ctx->sy1_floor) * t;
        
        // Clip against current viewport y-buffers
        int c_y_ceil = clamp(y_ceil, ctx->ytop[x], ctx->ybottom[x]);
        int c_y_floor = clamp(y_floor, ctx->ytop[x], ctx->ybottom[x]);
        
        // Perspective correct U mapping
        // Interpolation of 1/z and u/z
        double iz = ctx->iz1 + (ctx->iz2 - ctx->iz1) * t;
        double uz = ctx->uz1 + (ctx->uz2 - ctx->uz1) * t;
        double u_current = uz / iz;
        
        // Depth for shading
        double z_current = 1.0 / iz;

        // Angle Rayon pour Sol/Plafond
        double ray_angle_col = env->angle_table[x];
        
        // -- Rendu --
        
        // Plafond Texture
        if (c_y_ceil > ctx->ytop[x])
            draw_ceiling_vertical(env, x, ctx->ytop[x], c_y_ceil, ray_angle_col, sect);

        // Sol Texture
        if (c_y_floor < ctx->ybottom[x])
            draw_floor_vertical(env, x, c_y_floor, ctx->ybottom[x], ray_angle_col, sect);
        
        if (neighbor >= 0) // C'est un PORTAIL
        {
            // Interpolation hauteurs voisin
            int ny_ceil = ctx->n_sy1_ceil + (ctx->n_sy2_ceil - ctx->n_sy1_ceil) * t;
            int ny_floor = ctx->n_sy1_floor + (ctx->n_sy2_floor - ctx->n_sy1_floor) * t;
            
            int c_ny_ceil = clamp(ny_ceil, ctx->ytop[x], ctx->ybottom[x]);
            int c_ny_floor = clamp(ny_floor, ctx->ytop[x], ctx->ybottom[x]);
            
            // Dessin du mur du haut ("Upper Wall")
            if (c_ny_ceil > c_y_ceil) {
                 if (ctx->upper_tex) vline_textured(env, x, c_y_ceil, c_ny_ceil, y_ceil, y_floor, u_current, ctx->upper_tex, ctx->ytop, ctx->ybottom, z_current);
                 else if (ctx->wall_tex) vline_textured(env, x, c_y_ceil, c_ny_ceil, y_ceil, y_floor, u_current, ctx->wall_tex, ctx->ytop, ctx->ybottom, z_current);
                 else vline(env, x, c_y_ceil, c_ny_ceil, apply_fog(0xFF888888, z_current), ctx->ytop, ctx->ybottom);
            }
            
            // Dessin du mur du bas ("Lower Wall")
            if (c_ny_floor < c_y_floor) {
                 if (ctx->lower_tex) vline_textured(env, x, c_ny_floor, c_y_floor, y_ceil, y_floor, u_current, ctx->lower_tex, ctx->ytop, ctx->ybottom, z_current);
                 else if (ctx->wall_tex) vline_textured(env, x, c_ny_floor, c_y_floor, y_ceil, y_floor, u_current, ctx->wall_tex, ctx->ytop, ctx->ybottom, z_current);
                 else vline(env, x, c_ny_floor, c_y_floor, apply_fog(0xFF666666, z_current), ctx->ytop, ctx->ybottom);
            }
        }
        else // C'est un MUR SOLIDE
        {
            if (ctx->wall_tex)
                vline_textured(env, x, c_y_ceil, c_y_floor, y_ceil, y_floor, u_current, ctx->wall_tex, ctx->ytop, ctx->ybottom, z_current);
            else
                vline(env, x, c_y_ceil, c_y_floor, apply_fog(0xFFAAAAAA, z_current), ctx->ytop, ctx->ybottom);
        }
    }
}

// Cœur du rendu recursif
void render_sectors_recursive(t_env *env, int sector_id, int xmin, int xmax, int *ytop, int *ybottom, int depth)
{
    if (sector_id < 0 || sector_id >= env->sector_map.nb_sectors) return;
    
    // Safety check: ensure we don't exceed pool size
    // Pool size is MAX_RECURSION_DEPTH chunks.
    // If depth corresponds to index, next recursion uses depth+1.
    if (depth >= MAX_RECURSION_DEPTH - 1) return;
    
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
        
        // Clipping 'Near Plane' basique
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
        
        // Prepare Contexte
        t_render_context ctx;
        ctx.env = env;
        ctx.sect = sect;
        ctx.neighbor = sect->neighbors[i];
        ctx.sx1 = sx1;
        ctx.sx2 = sx2;
        ctx.begin_x = clamp(sx1, xmin, xmax);
        ctx.end_x = clamp(sx2, xmin, xmax);
        ctx.ytop = ytop;
        ctx.ybottom = ybottom;
        ctx.depth = depth;

        if (ctx.end_x <= ctx.begin_x) continue;

        // 6. Calcul des hauteurs plafond/sol (Projected heights)
        double ceil_h = sect->ceiling_height - env->player.height;
        double floor_h = sect->floor_height - env->player.height;
        
        ctx.sy1_ceil = (env->h / 2) - (int)(ceil_h / rz1 * scale1);
        ctx.sy1_floor = (env->h / 2) - (int)(floor_h / rz1 * scale1);
        ctx.sy2_ceil = (env->h / 2) - (int)(ceil_h / rz2 * scale2);
        ctx.sy2_floor = (env->h / 2) - (int)(floor_h / rz2 * scale2);
        
        ctx.n_sy1_ceil = 0; ctx.n_sy1_floor = 0; ctx.n_sy2_ceil = 0; ctx.n_sy2_floor = 0;
        if (ctx.neighbor >= 0)
        {
            double n_ceil_h = env->sector_map.sectors[ctx.neighbor].ceiling_height - env->player.height;
            double n_floor_h = env->sector_map.sectors[ctx.neighbor].floor_height - env->player.height;
            ctx.n_sy1_ceil = (env->h / 2) - (int)(n_ceil_h / rz1 * scale1);
            ctx.n_sy1_floor = (env->h / 2) - (int)(n_floor_h / rz1 * scale1);
            ctx.n_sy2_ceil = (env->h / 2) - (int)(n_ceil_h / rz2 * scale2);
            ctx.n_sy2_floor = (env->h / 2) - (int)(n_floor_h / rz2 * scale2);
        }

        // Texture Prep
        ctx.wall_tex = NULL;
        ctx.upper_tex = NULL;
        ctx.lower_tex = NULL;

        if (env->num_textures > 0)
        {
            if (sect->wall_textures) {
                int tex_id = sect->wall_textures[i];
                if (tex_id >= 0 && tex_id < env->num_textures)
                    ctx.wall_tex = &env->wall_textures[tex_id];
            }
            if (sect->upper_textures) {
                int tex_id = sect->upper_textures[i];
                if (tex_id >= 0 && tex_id < env->num_textures)
                    ctx.upper_tex = &env->wall_textures[tex_id];
            }
            if (sect->lower_textures) {
                int tex_id = sect->lower_textures[i];
                if (tex_id >= 0 && tex_id < env->num_textures)
                    ctx.lower_tex = &env->wall_textures[tex_id];
            }
        }

        double wall_len = hypot(v2.x - v1.x, v2.y - v1.y);
        double tex_scale = 1.0; 
        ctx.iz1 = 1.0 / rz1;
        ctx.iz2 = 1.0 / rz2;
        ctx.uz1 = 0.0 * ctx.iz1; 
        ctx.uz2 = (wall_len * tex_scale) * ctx.iz2; 

        // EXECUTE WALL LOOP
        render_wall_loop(&ctx);
        
        // RECURSION (Extract from loop)
        if (ctx.neighbor >= 0)
        {
            // USE POOL instead of MALLOC
            int *new_ytop = env->ytop_pool + (depth + 1) * env->w;
            int *new_ybottom = env->ybottom_pool + (depth + 1) * env->w;
            
            // Check if pointer is valid (implicit if depth check passed)
            
            memcpy(new_ytop, ytop, sizeof(int) * env->w);
            memcpy(new_ybottom, ybottom, sizeof(int) * env->w);
            
            // Recalcul de la fenetre pour le segment
            // NOTE: Only update the range [ctx.begin_x, ctx.end_x]
            
            for (int x = ctx.begin_x; x < ctx.end_x; x++)
            {
                    double t = (double)(x - sx1) / (sx2 - sx1);
                    
                    // Current sector heights
                    int y_ceil = ctx.sy1_ceil + (ctx.sy2_ceil - ctx.sy1_ceil) * t;
                    int y_floor = ctx.sy1_floor + (ctx.sy2_floor - ctx.sy1_floor) * t;
                    
                    // Neighbor sector heights
                    int ny_ceil = ctx.n_sy1_ceil + (ctx.n_sy2_ceil - ctx.n_sy1_ceil) * t;
                    int ny_floor = ctx.n_sy1_floor + (ctx.n_sy2_floor - ctx.n_sy1_floor) * t;
                    
                    // Clipping
                    int c_y_ceil = clamp(y_ceil, ytop[x], ybottom[x]);
                    int c_y_floor = clamp(y_floor, ytop[x], ybottom[x]);
                    int c_ny_ceil = clamp(ny_ceil, ytop[x], ybottom[x]);
                    int c_ny_floor = clamp(ny_floor, ytop[x], ybottom[x]);

                    // Portal aperture
                    int portal_top = MAX(c_y_ceil, c_ny_ceil);
                    int portal_bot = MIN(c_y_floor, c_ny_floor);
                    
                    new_ytop[x] = clamp(portal_top, new_ytop[x], env->h - 1);
                    new_ybottom[x] = clamp(portal_bot, new_ytop[x], new_ybottom[x]);
            }
            
            // RECURSIVE CALL
            render_sectors_recursive(env, ctx.neighbor, ctx.begin_x, ctx.end_x, new_ytop, new_ybottom, depth + 1);
            
            // No free() needed!
        }
    }
}
