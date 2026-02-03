#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

// Helper: Trouver quelle face de skybox intersecte le rayon à la colonne X
static int find_skybox_face(t_env *env, int x)
{
    // Parcourir les 4 faces
    for (int i = 0; i < 4; i++)
    {
        t_skybox_vertex *v = &env->skybox.vertices[i];
        
        if (!v->draw) continue;
        
        // Vérifier si X est dans la range de cette face
        if (x >= v->clipped_x1 && x < v->clipped_x2)
            return i;
    }
    
    return -1;  // Aucune face trouvée
}

// Helper: Calcul de la coordonnée U texturee (perspective-correct)
static double get_texture_u(t_skybox_vertex *v, int x, t_texture *tex)
{
    // Interpolation linéaire de la position dans la face
    double t = (x - v->clipped_x1) / (v->clipped_x2 - v->clipped_x1);
    
    // Perspective-correct texture mapping
    // Formule: U = (x0*z1 + t*(x1*z0 - x0*z1)) / (z0 + t*(z1 - z0))
    double z_lerp = v->clipped_vz1 + t * (v->clipped_vz2 - v->clipped_vz1);
    double u_world = (v->x0z1 + t * v->xzrange) / z_lerp;
    
    // Normaliser à [0, 1] puis mapper sur texture width
    u_world = fmod(u_world, 10.0);  // Répéter toutes les 10 unités (box_size)
    if (u_world < 0) u_world += 10.0;
    
    double u_norm = u_world / 10.0;
    
    return u_norm * tex->width;
}

// Helper: Calcul de la coordonnée V texturee basée sur l'angle vertical
static double get_texture_v(t_env *env, int y, t_texture *tex)
{
    // Calculer l'angle vertical pour ce pixel Y
    double fov_scale = (env->w / 2.0) / tan(30.0 * PI / 180.0);
    double vertical_angle = atan((y - env->h / 2.0) / fov_scale);
    
    // Mapper angle à texture (assumant texture couvre ~180° verticalement)
    // Range vertical: -PI/2 à +PI/2
    double v_normalized = (vertical_angle / (PI / 2.0)) * 0.5 + 0.5;  // 0.0 à 1.0
    
    // Clamp
    if (v_normalized < 0.0) v_normalized = 0.0;
    if (v_normalized > 1.0) v_normalized = 1.0;
    
    return v_normalized * tex->height;
}

// Fonction principale: Dessiner une colonne de skybox
void draw_skybox_column(t_env *env, int x, int y1, int y2, double ray_angle)
{
    (void)ray_angle;  // Pas utilisé dans cette version simplifiée
    
    if (!env->skybox.enabled || !env->skybox.computed)
        return;
    
    if (y2 < y1) return;
    
    // Trouver quelle face dessiner
    int face_id = find_skybox_face(env, x);
    
    if (face_id == -1)
    {
        // Aucune face trouvée: dessiner noir ou couleur de fallback
        for (int y = y1; y <= y2; y++)
            env->sdl.texture_pixels[y * env->w + x] = 0xFF000020;  // Bleu très foncé
        return;
    }
    
    // Récupérer la face et la texture
    t_skybox_vertex *v = &env->skybox.vertices[face_id];
    t_texture *tex = &env->skybox.textures[env->skybox.current_id];
    
    if (!tex->pixels) return;
    
    // Calculer U une seule fois pour cette colonne
    double tex_u = get_texture_u(v, x, tex);
    int u_pix = (int)tex_u % tex->width;
    if (u_pix < 0) u_pix += tex->width;
    
    // Dessiner chaque pixel Y
    for (int y = y1; y <= y2; y++)
    {
        // Calculer V pour ce pixel Y
        double tex_v = get_texture_v(env, y, tex);
        int v_pix = (int)tex_v % tex->height;
        if (v_pix < 0) v_pix += tex->height;
        
        // Sample texture
        Uint32 color = tex->pixels[v_pix * tex->width + u_pix];
        
        // Écrire pixel
        env->sdl.texture_pixels[y * env->w + x] = color;
    }
}
