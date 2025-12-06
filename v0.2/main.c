
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

void drawLine(t_render *render, int x0, int y0, int, x1, int y1)
{
    int dx, dy;
    if (x1 > x0)
        dx = x1 - x0;
    else
        dx = x0 - x1;

    if (y1 > y0)
        dy = y1 - y0;
    else
        dy = y0 - y1;

    int sx = x0 < x1 ? 1 : -1;
    int sy = x0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2
    int e2;

    while (1)
    {
        putPixel(render, x0, y0, 0xFFFFFF);

        if (x0 == x1 && y0 == y1)
            break;
        e2 = err; // snapchot
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        } if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

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