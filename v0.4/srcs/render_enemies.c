#include "entities.h"
#include "enemy_types.h"
#include "env.h"
#include "defines.h"
#include <math.h>

typedef struct {
    double  sprite_x;       
    double  sprite_y;       
    double  sprite_width;   
    double  sprite_height;  
    double  distance;       
    int     visible;        
} t_sprite_render_info;

static t_sprite_render_info calculate_sprite_position(t_env *env, t_entity *entity)
{
    t_sprite_render_info info;
    double dx, dy;
    double sprite_angle;
    double sprite_screen_x;
    double perp_distance;
    double sprite_height_screen;
    dx = entity->x - env->player.pos.x;
    dy = entity->y - env->player.pos.y;
    sprite_angle = atan2(dy, dx) - env->player.angle;
    while (sprite_angle < -M_PI) sprite_angle += 2 * M_PI;
    while (sprite_angle > M_PI) sprite_angle -= 2 * M_PI;
    perp_distance = sqrt(dx * dx + dy * dy) * cos(sprite_angle);
    if (perp_distance < 0.1) {
        info.visible = 0;
        return info;
    }
    sprite_screen_x = tan(sprite_angle) * (env->w / 2.0) / tan(FOV * M_PI / 180.0 / 2.0);
    sprite_screen_x += env->w / 2.0;
    sprite_height_screen = (env->h / perp_distance) * entity->scale;
    info.sprite_y = env->h / 2.0 + (env->player.height - entity->z) * (env->h / perp_distance);
    info.sprite_x = sprite_screen_x;
    info.sprite_width = sprite_height_screen;  
    info.sprite_height = sprite_height_screen;
    info.distance = perp_distance;
    info.visible = 1;
    if (sprite_screen_x < -sprite_height_screen || sprite_screen_x > env->w + sprite_height_screen) 
        info.visible = 0;
    return info;
}

static Uint32 sample_texture(t_texture *tex, double u, double v)
{
    int x, y;
    if (!tex || !tex->pixels || tex->width == 0 || tex->height == 0)
        return 0xFF00FF;     
    x = (int)(u * tex->width);
    y = (int)(v * tex->height);
    if (x < 0) x = 0;
    if (x >= tex->width) x = tex->width - 1;
    if (y < 0) y = 0;
    if (y >= tex->height) y = tex->height - 1;
    
    
    return tex->pixels[y * tex->width + x];
}


static void draw_enemy_sprite(t_env *env, t_entity *entity, t_sprite_render_info *info)
{
    int screen_x, screen_y;
    int start_x, end_x;
    int start_y, end_y;
    t_texture *tex;
    int tex_index;
    
    if (!info->visible)
        return;
    
    
    
    tex_index = entity->sprite_id - 100; 
    
    if (tex_index < 0 || tex_index >= env->num_enemy_textures)
    {
        DEBUG_LOG("Invalid enemy sprite ID: %d\n", entity->sprite_id);
        return;
    }
    
    tex = &env->enemy_textures[tex_index];
    
    if (!tex->pixels)
    {
        
        DEBUG_LOG("Enemy texture %d not loaded\n", tex_index);
        return;
    }
    
    
    start_x = (int)(info->sprite_x - info->sprite_width / 2.0);
    end_x = (int)(info->sprite_x + info->sprite_width / 2.0);
    start_y = (int)(info->sprite_y - info->sprite_height);
    end_y = (int)(info->sprite_y);
    
    
    if (start_x < 0) start_x = 0;
    if (end_x >= env->w) end_x = env->w - 1;
    if (start_y < 0) start_y = 0;
    if (end_y >= env->h) end_y = env->h - 1;
    
    
    for (screen_x = start_x; screen_x < end_x; screen_x++)
    {
        
        if (env->zbuffer[screen_x] < info->distance)
            continue;
        
        
        double u = (double)(screen_x - (info->sprite_x - info->sprite_width / 2.0)) / info->sprite_width;
        
        
        for (screen_y = start_y; screen_y < end_y; screen_y++)
        {
            
            double v = (double)(screen_y - (info->sprite_y - info->sprite_height)) / info->sprite_height;
            
            
            Uint32 color = sample_texture(tex, u, v);
            
            
            if (color == 0xFF00FF || color == 0x000000 || (color & 0xFF000000) == 0)
                continue;
            
            
            int pixel_index = screen_y * env->w + screen_x;
            if (pixel_index >= 0 && pixel_index < env->w * env->h)
                env->sdl.texture_pixels[pixel_index] = color;
        }
    }
}


void render_enemies(t_env *env)
{
    t_entity *current;
    t_sprite_render_info info;
    int count;
    
    if (!env || !env->entity_mgr.entities)
        return;
    
    count = 0;
    current = env->entity_mgr.entities;
    
    
    while (current)
    {
        
        if (current->type == ENTITY_ENEMY && current->active)
        {
            
            info = calculate_sprite_position(env, current);
            
            
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



void sort_enemies_by_distance(t_env *env)
{
    
    
    (void)env;
}
