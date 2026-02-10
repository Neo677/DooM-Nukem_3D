#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

void handle_input(t_env *env)
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    double speed = 0.05;
    double rot_speed = 0.05;
    
    
    static int tab_pressed = 0;
    if (keys[SDL_SCANCODE_TAB])
    {
        if (!tab_pressed)
        {
            if (env->render_mode == MODE_2D)
                env->render_mode = MODE_3D;
            else
                env->render_mode = MODE_2D;
            
            tab_pressed = 1;
        }
    }
    else
    {
        tab_pressed = 0;
    }
    
    
    if (env->render_mode == MODE_2D)
    {
        
        if (keys[SDL_SCANCODE_KP_PLUS] || keys[SDL_SCANCODE_EQUALS])
        {
            env->view_2d.zoom *= 1.05;  
            if (env->view_2d.zoom > 200.0)
                env->view_2d.zoom = 200.0;
        }
        if (keys[SDL_SCANCODE_KP_MINUS] || keys[SDL_SCANCODE_MINUS])
        {
            env->view_2d.zoom *= 0.95;  
            if (env->view_2d.zoom < 10.0)
                env->view_2d.zoom = 10.0;
        }
        
        
        static int r_pressed = 0;
        if (keys[SDL_SCANCODE_R])
        {
            if (!r_pressed)
            {
                env->view_2d.show_rays = !env->view_2d.show_rays;
                r_pressed = 1;
            }
        }
        else
        {
            r_pressed = 0;
        }
        
        
        static int g_pressed = 0;
        if (keys[SDL_SCANCODE_G])
        {
            if (!g_pressed)
            {
                env->view_2d.show_grid = !env->view_2d.show_grid;
                g_pressed = 1;
            }
        }
        else
        {
            g_pressed = 0;
        }
    }
    
    
    static int m_pressed = 0;
    if (keys[SDL_SCANCODE_M])
    {
        if (!m_pressed)
        {
            env->view_2d.show_minimap = !env->view_2d.show_minimap;
            m_pressed = 1;
        }
    }
    else
    {
        m_pressed = 0;
    }
    
    
    static int c_pressed = 0;
    if (keys[SDL_SCANCODE_C])
    {
        if (!c_pressed)
        {
            env->mouse_captured = !env->mouse_captured;
            SDL_SetRelativeMouseMode(env->mouse_captured ? SDL_TRUE : SDL_FALSE);
            c_pressed = 1;
        }
    }
    else
    {
        c_pressed = 0;
    }
    
    
    
    
    
    if (keys[SDL_SCANCODE_W])
    {
        double dx = cos(env->player.angle) * speed;
        double dy = sin(env->player.angle) * speed;
        player_move(env, dx, dy);
    }
    if (keys[SDL_SCANCODE_S])
    {
        double dx = -cos(env->player.angle) * speed;
        double dy = -sin(env->player.angle) * speed;
        player_move(env, dx, dy);
    }
    
    
    if (keys[SDL_SCANCODE_A])
    {
        double dx = cos(env->player.angle - PI / 2) * speed;
        double dy = sin(env->player.angle - PI / 2) * speed;
        player_move(env, dx, dy);
    }
    if (keys[SDL_SCANCODE_D])
    {
        double dx = cos(env->player.angle + PI / 2) * speed;
        double dy = sin(env->player.angle + PI / 2) * speed;
        player_move(env, dx, dy);
    }
    
    
    if (keys[SDL_SCANCODE_LEFT])
        env->player.angle -= rot_speed;
    if (keys[SDL_SCANCODE_RIGHT])
        env->player.angle += rot_speed;
    
    
    while (env->player.angle < 0)
        env->player.angle += 2 * PI;
    while (env->player.angle >= 2 * PI)
        env->player.angle -= 2 * PI;
    
    
    
    if (env->mouse_captured && env->render_mode == MODE_3D)
    {
        double pitch_sensitivity = 0.005;
        env->player.pitch += env->sdl.mouse_y * pitch_sensitivity;
    }
    
    
    if (keys[SDL_SCANCODE_UP])
        env->player.pitch += PITCH_SPEED;
    if (keys[SDL_SCANCODE_DOWN])
        env->player.pitch -= PITCH_SPEED;

    
    if (env->player.pitch < -0.8) env->player.pitch = -0.8;
    if (env->player.pitch > 0.8) env->player.pitch = 0.8;
    
    
    env->player.pitch_cos = cos(env->player.pitch);
    env->player.pitch_sin = sin(env->player.pitch);
    
    
    env->player.horizon = (env->h / 2.0) + (env->player.pitch * PITCH_FACTOR);

    
    if (keys[SDL_SCANCODE_SPACE])
    {
        if (!env->player.is_falling)
        {
            env->player.velocity_z = JUMP_VELOCITY;
            env->player.is_falling = 1;
            DEBUG_LOG("JUMP! vz=%.2f\n", env->player.velocity_z);
        }
    }
    
    
    if (keys[SDL_SCANCODE_LSHIFT]) env->player.height += 0.05;
    if (keys[SDL_SCANCODE_LCTRL]) env->player.height -= 0.05;
    
    
    if (env->mouse_captured && env->render_mode == MODE_3D)
    {
        double mouse_sensitivity = 0.002;
        env->player.angle += env->sdl.mouse_x * mouse_sensitivity;
        
        
        while (env->player.angle < 0)
            env->player.angle += 2 * PI;
        while (env->player.angle >= 2 * PI)
            env->player.angle -= 2 * PI;
    }
    
    
    static int b_pressed = 0;
    if (keys[SDL_SCANCODE_B])
    {
        if (!b_pressed)
        {
            toggle_skybox(env);
            b_pressed = 1;
        }
    }
    else
    {
        b_pressed = 0;
    }
    
    if (keys[SDL_SCANCODE_1]) switch_skybox(env, 0);
    if (keys[SDL_SCANCODE_2]) switch_skybox(env, 1);
    if (keys[SDL_SCANCODE_3]) switch_skybox(env, 2);
    
    
    static int f8_pressed = 0;
    if (keys[SDL_SCANCODE_F8]) {
        if (!f8_pressed) { env->debug_physics = !env->debug_physics; f8_pressed = 1; }
    } else f8_pressed = 0;

    static int f9_pressed = 0;
    if (keys[SDL_SCANCODE_F9]) {
        if (!f9_pressed) { env->slow_motion = !env->slow_motion; f9_pressed = 1; }
    } else f9_pressed = 0;

    static int f10_pressed = 0;
    if (keys[SDL_SCANCODE_F10]) {
        if (!f10_pressed) { 
            if (!env->single_step_mode) env->single_step_mode = 1;
            else env->step_trigger = 1; 
            f10_pressed = 1; 
        }
    } else f10_pressed = 0;
}