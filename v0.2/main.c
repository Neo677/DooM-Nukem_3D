
#include "./header/game.h"
#include "./header/types.h"

void putPixel(t_render *render, int x, int y, int color)
{
    char *dst;

    if (x >= screenW || y >= screenH || x < 0 || y < 0)
        return;

    dst = render->addr + (y * render->line_len + x * (render->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

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

void updateScreen(t_render *render)
{
    mlx_put_image_to_window(render->mlx, render->win, render->img, 0, 0);
}

void clearScreen(t_render *render, int color)
{
    int x, y;
    (void)render;

    for (y = 0; y < screenH; y++)
        for (x = 0; x < screenW; x++) {
            // putPixel(render, x, y, 0xFFFFFF);
            putPixel(render, x, y, color);
            
        }
}

int game_loop(t_render *render)
{
    clearScreen(render, 0x000000);

    updateScreen(render);
    
    return (0);
}

void drawLine()

int main()
{
    t_render render;

    render.mlx = mlx_init();
    render.win = mlx_new_window(render.mlx, screenW, screenH, "doom-nukem");
    
    render.img = mlx_new_image(render.mlx, screenW, screenH);
    render.addr = mlx_get_data_addr(render.img, &render.bits_per_pixel, &render.line_len, &render.endian);

    mlx_key_hook(render.win, key_hook, &render);
    mlx_hook(render.win, 17, 0, close_window, &render);
    mlx_loop_hook(render.mlx, game_loop, &render);


    mlx_loop(render.mlx);


    
    return (0);
}