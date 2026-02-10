#include "env.h"
#include <stdio.h> 

void draw_hud(t_env *env)
{
    
    
    int bar_height = 80;
    int y_start = env->h - bar_height;
    
    
    t_rectangle rect = new_rectangle(0xFF333333, 0xFF555555, 1, 2);
    t_point pos = new_point(0, y_start);
    t_point size = new_point(env->w, bar_height);
    
    draw_rectangle(env, rect, pos, size);
    
    
    char text[64];
    int text_y = y_start + 30; 
    
    
    snprintf(text, sizeof(text), "HEALTH: %d", env->player.health);
    draw_text(env, text, 50, text_y, 0xFFFF0000); 
    
    
    snprintf(text, sizeof(text), "ARMOR: %d", env->player.armor);
    draw_text(env, text, 300, text_y, 0xFF00FF00); 
    
    
    snprintf(text, sizeof(text), "AMMO: %d", env->player.ammo);
    draw_text(env, text, 550, text_y, 0xFFFFFF00); 
    
    
    
    
    
    if (env->debug_physics)
    {
        char debug_text[128];
        int dy = 20;
        int col = 0xFF00FFFF; 
        
        snprintf(debug_text, sizeof(debug_text), "Sect: %d  Pos: %.2f, %.2f", 
            env->player.current_sector, env->player.pos.x, env->player.pos.y);
        draw_text(env, debug_text, 10, dy, col);
        
        snprintf(debug_text, sizeof(debug_text), "Z: %.3f  VZ: %.3f  Fall: %d", 
            env->player.height, env->player.velocity_z, env->player.is_falling);
        draw_text(env, debug_text, 10, dy + 20, col);
        
        
        
        if (env->slow_motion) draw_text(env, "[SLOW MO]", 10, dy + 40, 0xFFFFAA00);
        if (env->single_step_mode) draw_text(env, "[PAUSED] (F10 Step)", 100, dy + 40, 0xFFFFAA00);
    }
}
