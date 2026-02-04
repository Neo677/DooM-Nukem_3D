#include "env.h"
#include <stdio.h> // snprintf

void draw_hud(t_env *env)
{
    // 1. Draw Background Bar
    // Bar height: 100 pixels at the bottom
    int bar_height = 80;
    int y_start = env->h - bar_height;
    
    // Gray background with dark gray border
    t_rectangle rect = new_rectangle(0xFF333333, 0xFF555555, 1, 2);
    t_point pos = new_point(0, y_start);
    t_point size = new_point(env->w, bar_height);
    
    draw_rectangle(env, rect, pos, size);
    
    // 2. Draw Stats
    char text[64];
    int text_y = y_start + 30; // Centered vertically in the bar approx
    
    // Health
    snprintf(text, sizeof(text), "HEALTH: %d", env->player.health);
    draw_text(env, text, 50, text_y, 0xFFFF0000); // Red
    
    // Armor
    snprintf(text, sizeof(text), "ARMOR: %d", env->player.armor);
    draw_text(env, text, 300, text_y, 0xFF00FF00); // Green
    
    // Ammo
    snprintf(text, sizeof(text), "AMMO: %d", env->player.ammo);
    draw_text(env, text, 550, text_y, 0xFFFFFF00); // Yellow
    
    // Optional: Face placeholder (just text for now or simple rect)
    // draw_text(env, "(^_^)", 800, text_y, 0xFFFFFFFF);
}
