#include "../header/game.h"

int key_hook(int keycode, t_render *render)
{
    if (keycode == ESC)
    {
        mlx_destroy_window(render->mlx, render->win);
        exit(0);
    }
    return (0);
}

int close_window(int x, int y, t_render *render)
{
    (void)x;
    (void)y;
    mlx_destroy_window(render->mlx, render->win);
    exit(0);
    return (0);
}

int key_press(int keycode, t_render *render)
{
    (void)render;
    if (keycode >= 0 && keycode < 65536)
        global.g_keys[keycode] = 1;
    if (keycode == V && !global.keyV_pressed)
    {
        global.flyMode = !global.flyMode;
        global.keyV_pressed = 1;
        if (global.flyMode)
        {
            global.cam.onGround = 0;
            global.cam.velZ = 0;
        }
    }
    if (keycode == ESC)
    {
        mlx_destroy_window(render->mlx, render->win);
        exit(0);
    }
    return (0);
}

int key_release(int keycode, t_render *render)
{
    (void)render;
    if (keycode >= 0 && keycode < 65536)
        global.g_keys[keycode] = 0;
    if (keycode == V)
        global.keyV_pressed = 0;
    return (0);
}

int mouse_move(int x, int y, t_render *render)
{
    float dx;
    float dy;

    (void)render;
    if (!global.mouseInit)
    {
        global.mouseInit = 1;
        global.lastMouseX = x;
        global.lastMouseY = y;
        return (0);
    }
    dx = (float)(x - global.lastMouseX);
    dy = (float)(y - global.lastMouseY);
    global.lastMouseX = x;
    global.lastMouseY = y;
    global.cam.camAngle += dx * SENSI;
    global.cam.camPitch += (-dy) * SENSI;
    if (global.cam.camPitch > 1.0f)
        global.cam.camPitch = 1.0f;
    if (global.cam.camPitch < -1.0f)
        global.cam.camPitch = -1.0f;
    return (0);
}
