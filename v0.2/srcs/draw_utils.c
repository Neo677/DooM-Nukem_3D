#include "../header/game.h"
#include "../header/rasterize.h"

void updateScreen(t_render *render)
{
    mlx_put_image_to_window(render->mlx, render->win, render->img, 0, 0);
}

void clearScreen(t_render *render, int color)
{
    int x;
    int y;

    y = 0;
    while (y < screenH)
    {
        x = 0;
        while (x < screenW)
        {
            putPixel(render, x, y, color);
            x++;
        }
        y++;
    }
}

void drawLine(t_render *render, int x0, int y0, int x1, int y1, int color)
{
    int dx;
    int dy;
    int sx;
    int sy;
    int err;
    int e2;

    if (x1 > x0)
        dx = x1 - x0;
    else
        dx = x0 - x1;
    if (y1 > y0)
        dy = y1 - y0;
    else
        dy = y0 - y1;
    sx = x0 < x1 ? 1 : -1;
    sy = y0 < y1 ? 1 : -1;
    err = (dx > dy ? dx : -dy) / 2;
    while (1)
    {
        putPixel(render, x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void drawRect(t_render *render, int x, int y, int w, int h, int color)
{
    int py;
    int px;

    py = y;
    while (py < y + h && py < screenH)
    {
        px = x;
        while (px < x + w && px < screenW)
        {
            if (px >= 0 && py >= 0)
                putPixel(render, px, py, color);
            px++;
        }
        py++;
    }
}

void drawRectOutline(t_render *render, int x, int y, int w, int h, int color)
{
    drawLine(render, x, y, x + w, y, color);
    drawLine(render, x + w, y, x + w, y + h, color);
    drawLine(render, x + w, y + h, x, y + h, color);
    drawLine(render, x, y + h, x, y, color);
}
