#include "env.h"
#include <math.h>

#define PI 3.14159265358979323846

void handle_input(t_env *env)
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    double speed = 0.05;
    double rot_speed = 0.05;
    
    // ========== TOGGLE 2D/3D avec TAB ==========
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
    
    // ========== CONTRÔLES MODE 2D ==========
    if (env->render_mode == MODE_2D)
    {
        // Zoom +/- 
        if (keys[SDL_SCANCODE_KP_PLUS] || keys[SDL_SCANCODE_EQUALS])
        {
            env->view_2d.zoom *= 1.05;  // +5%
            if (env->view_2d.zoom > 200.0)
                env->view_2d.zoom = 200.0;
        }
        if (keys[SDL_SCANCODE_KP_MINUS] || keys[SDL_SCANCODE_MINUS])
        {
            env->view_2d.zoom *= 0.95;  // -5%
            if (env->view_2d.zoom < 10.0)
                env->view_2d.zoom = 10.0;
        }
        
        // Toggle rayons avec R
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
        
        // Toggle grille avec G
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
    
    // ========== TOGGLE MINIMAP avec M (fonctionne en mode 3D) ==========
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
    
    // ========== TOGGLE CAPTURE SOURIS avec C ==========
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
    
    // ========== MOUVEMENT JOUEUR (fonctionne dans les 2 modes) ==========
    
    // Avancer/reculer
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
    {
        double dx = cos(env->player.angle) * speed;
        double dy = sin(env->player.angle) * speed;
        player_move(env, dx, dy);
    }
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
    {
        double dx = -cos(env->player.angle) * speed;
        double dy = -sin(env->player.angle) * speed;
        player_move(env, dx, dy);
    }
    
    // Strafe gauche/droite
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
    
    // Rotation (fleches gauche/droite)
    if (keys[SDL_SCANCODE_LEFT])
        env->player.angle -= rot_speed;
    if (keys[SDL_SCANCODE_RIGHT])
        env->player.angle += rot_speed;
    
    // Normaliser l'angle
    while (env->player.angle < 0)
        env->player.angle += 2 * PI;
    while (env->player.angle >= 2 * PI)
        env->player.angle -= 2 * PI;
    // ========== CONTROLES SKYBOX (B, 1, 2, 3) ==========
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
}