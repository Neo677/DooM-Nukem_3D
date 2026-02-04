#include "entities.h"
#include "enemy_types.h"
#include "env.h"
#include "defines.h"
#include <math.h>

// Système de rendu des ennemis
// Compatible avec le système de sprite rendering existant

// Structure pour le rendu d'un sprite ennemi
typedef struct {
    double  sprite_x;       // Position écran X
    double  sprite_y;       // Position écran Y (base)
    double  sprite_width;   // Largeur à l'écran
    double  sprite_height;  // Hauteur à l'écran
    double  distance;       // Distance au joueur
    int     visible;        // 1 si visible, 0 sinon
} t_sprite_render_info;

// Calcul de la position écran d'un sprite ennemi
static t_sprite_render_info calculate_sprite_position(t_env *env, t_entity *entity)
{
    t_sprite_render_info info;
    double dx, dy;
    double sprite_angle;
    double sprite_screen_x;
    double perp_distance;
    double sprite_height_screen;
    
    // Position relative au joueur
    dx = entity->x - env->player.pos.x;
    dy = entity->y - env->player.pos.y;
    
    // Angle du sprite par rapport au joueur
    sprite_angle = atan2(dy, dx) - env->player.angle;
    
    // Normaliser l'angle
    while (sprite_angle < -M_PI) sprite_angle += 2 * M_PI;
    while (sprite_angle > M_PI) sprite_angle -= 2 * M_PI;
    
    // Distance perpendiculaire (comme pour les murs)
    perp_distance = sqrt(dx * dx + dy * dy) * cos(sprite_angle);
    
    // Vérifier si le sprite est derrière le joueur
    if (perp_distance < 0.1)
    {
        info.visible = 0;
        return info;
    }
    
    // Position X à l'écran (projection)
    sprite_screen_x = tan(sprite_angle) * (env->w / 2.0) / tan(FOV * M_PI / 180.0 / 2.0);
    sprite_screen_x += env->w / 2.0;
    
    // Hauteur du sprite à l'écran (basée sur la distance)
    sprite_height_screen = (env->h / perp_distance) * entity->scale;
    
    // Position Y à l'écran (en tenant compte de la hauteur du joueur et de l'entité)
    info.sprite_y = env->h / 2.0 + (env->player.height - entity->z) * (env->h / perp_distance);
    
    info.sprite_x = sprite_screen_x;
    info.sprite_width = sprite_height_screen;  // Largeur = hauteur pour garder les proportions
    info.sprite_height = sprite_height_screen;
    info.distance = perp_distance;
    info.visible = 1;
    
    // Vérifier si le sprite est dans les limites de l'écran
    if (sprite_screen_x < -sprite_height_screen || 
        sprite_screen_x > env->w + sprite_height_screen)
    {
        info.visible = 0;
    }
    
    return info;
}

// Échantillonner une couleur dans une texture (avec clipping)
static Uint32 sample_texture(t_texture *tex, double u, double v)
{
    int x, y;
    
    if (!tex || !tex->pixels || tex->width == 0 || tex->height == 0)
        return 0xFF00FF; // Magenta si texture invalide
    
    // Convertir u,v (0.0-1.0) en coordonnées pixel
    x = (int)(u * tex->width);
    y = (int)(v * tex->height);
    
    // Clipping
    if (x < 0) x = 0;
    if (x >= tex->width) x = tex->width - 1;
    if (y < 0) y = 0;
    if (y >= tex->height) y = tex->height - 1;
    
    // Retourner le pixel
    return tex->pixels[y * tex->width + x];
}

// Dessiner un sprite ennemi
static void draw_enemy_sprite(t_env *env, t_entity *entity, t_sprite_render_info *info)
{
    int screen_x, screen_y;
    int start_x, end_x;
    int start_y, end_y;
    t_texture *tex;
    int tex_index;
    
    if (!info->visible)
        return;
    
    // Récupérer la texture de l'ennemi
    // Les sprites ennemis sont indexés de 0 à 21 dans enemy_textures
    tex_index = entity->sprite_id - 100; // sprite_id 100-121 -> index 0-21
    
    if (tex_index < 0 || tex_index >= env->num_enemy_textures)
    {
        DEBUG_LOG("Invalid enemy sprite ID: %d\n", entity->sprite_id);
        return;
    }
    
    tex = &env->enemy_textures[tex_index];
    
    if (!tex->pixels)
    {
        // Texture non chargée, dessiner un rectangle coloré
        DEBUG_LOG("Enemy texture %d not loaded\n", tex_index);
        return;
    }
    
    // Calculer les limites du sprite à l'écran
    start_x = (int)(info->sprite_x - info->sprite_width / 2.0);
    end_x = (int)(info->sprite_x + info->sprite_width / 2.0);
    start_y = (int)(info->sprite_y - info->sprite_height);
    end_y = (int)(info->sprite_y);
    
    // Clipping aux bords de l'écran
    if (start_x < 0) start_x = 0;
    if (end_x >= env->w) end_x = env->w - 1;
    if (start_y < 0) start_y = 0;
    if (end_y >= env->h) end_y = env->h - 1;
    
    // Dessiner le sprite colonne par colonne
    for (screen_x = start_x; screen_x < end_x; screen_x++)
    {
        // Vérifier le Z-buffer
        if (env->zbuffer[screen_x] < info->distance)
            continue;
        
        // Calculer u (coordonnée horizontale texture 0.0-1.0)
        double u = (double)(screen_x - (info->sprite_x - info->sprite_width / 2.0)) / info->sprite_width;
        
        // Dessiner la colonne verticale du sprite
        for (screen_y = start_y; screen_y < end_y; screen_y++)
        {
            // Calculer v (coordonnée verticale texture 0.0-1.0)
            double v = (double)(screen_y - (info->sprite_y - info->sprite_height)) / info->sprite_height;
            
            // Échantillonner la texture
            Uint32 color = sample_texture(tex, u, v);
            
            // Sauter les pixels transparents (magenta = 0xFF00FF ou noir = 0x000000)
            if (color == 0xFF00FF || color == 0x000000 || (color & 0xFF000000) == 0)
                continue;
            
            // Dessiner le pixel
            int pixel_index = screen_y * env->w + screen_x;
            if (pixel_index >= 0 && pixel_index < env->w * env->h)
                env->sdl.texture_pixels[pixel_index] = color;
        }
    }
}

// Fonction principale de rendu de tous les ennemis
void render_enemies(t_env *env)
{
    t_entity *current;
    t_sprite_render_info info;
    int count;
    
    if (!env || !env->entity_mgr.entities)
        return;
    
    count = 0;
    current = env->entity_mgr.entities;
    
    // Parcourir toutes les entités
    while (current)
    {
        // Ne rendre que les ennemis actifs
        if (current->type == ENTITY_ENEMY && current->active)
        {
            // Calculer la position du sprite
            info = calculate_sprite_position(env, current);
            
            // Dessiner le sprite si visible
            if (info.visible)
            {
                draw_enemy_sprite(env, current, &info);
                count++;
            }
        }
        
        current = current->next;
    }
    
    VERBOSE_LOG("Rendered %d enemies\n", count);
}

// Fonction pour trier les ennemis par distance (du plus loin au plus proche)
// Nécessaire pour le rendu correct avec le Z-buffer
void sort_enemies_by_distance(t_env *env)
{
    // TODO: Implémenter le tri des entités par distance
    // Pour l'instant, le rendu simple vérifie le zbuffer colonne par colonne
    (void)env;
}
