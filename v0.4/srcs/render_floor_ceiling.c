#include "env.h"
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846

static double get_floor_distance(t_env *env, int screen_y)
{
    int p = screen_y - env->h / 2;
    double cam_height = 0.5;
    if (p == 0)
        return (1000.0);
    double row_distance = cam_height / ((double)p / env->h);
    return (fabs(row_distance));
}

static void render_horizontal_line(t_env *env, int y, int is_floor)
{
    double distance = get_floor_distance(env, y);
    if (distance > 20.0)
        distance = 20.0;
    double fov_rad = (FOV * PI) / 180.0;
    double left_angle = env->player.angle - fov_rad / 2.0;
    double right_angle = env->player.angle + fov_rad / 2.0;
    double left_x = env->player.pos.x + cos(left_angle) * distance;
    double left_z = env->player.pos.y + sin(left_angle) * distance;
    double right_x = env->player.pos.x + cos(right_angle) * distance;
    double right_z = env->player.pos.y + sin(right_angle) * distance;
    t_texture *texture = is_floor ? &env->floor_texture : &env->ceiling_texture;
    if (!texture->pixels || texture->width == 0 || texture->height == 0) {
        Uint32 fallback_color = is_floor ? 0xFFFF00FF : 0xFF00FFFF;  
        for (int x = 0; x < env->w; x++)
            env->sdl.texture_pixels[y * env->w + x] = fallback_color;
        return;
    }
    for (int x = 0; x < env->w; x++) {
        double t = (double)x / env->w;
        double world_x = left_x + (right_x - left_x) * t;
        double world_z = left_z + (right_z - left_z) * t;
        if (!is_floor && env->skybox_enabled) {
            if (world_x > 3.0 && world_x < 7.0 && world_z > 3.0 && world_z < 7.0)
                continue;
        }
        int tex_x = (int)(fabs(world_x * 64.0)) % texture->width;
        int tex_y = (int)(fabs(world_z * 64.0)) % texture->height;
        if (tex_x < 0) 
            tex_x = 0;
        if (tex_y < 0) 
            tex_y = 0;
        if (tex_x >= texture->width)
            tex_x = texture->width - 1;
        if (tex_y >= texture->height)
            tex_y = texture->height - 1;
        Uint32 color = texture->pixels[tex_y * texture->width + tex_x];
        double fog_factor = 1.0 - (distance / 20.0);
        if (fog_factor < 0.3)
            fog_factor = 0.3;
        Uint8 r = ((color >> 16) & 0xFF) * fog_factor;
        Uint8 g = ((color >> 8) & 0xFF) * fog_factor;
        Uint8 b = (color & 0xFF) * fog_factor; 
        Uint32 final_color = 0xFF000000 | (r << 16) | (g << 8) | b;
        env->sdl.texture_pixels[y * env->w + x] = final_color;
    }
}

void render_floor_ceiling(t_env *env)
{
    int half_h = env->h / 2;
    for (int y = half_h; y < env->h; y++)
        render_horizontal_line(env, y, 1);
    for (int y = 0; y < half_h; y++)
        render_horizontal_line(env, y, 0);
}
