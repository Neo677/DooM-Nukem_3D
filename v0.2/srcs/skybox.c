#include "../header/game.h"
#include "../header/rasterize.h"

int loadSkybox(t_render *render, const char *path)
{
    int width;
    int height;

    global.skybox.img = mlx_xpm_file_to_image(render->mlx, (char *)path, &width, &height);
    if (!global.skybox.img)
    {
        printf("Erreur: Impossible de charger la skybox: %s\n", path);
        char absPath[1024];
        if (realpath(path, absPath))
             printf("Absolute path tried: %s\n", absPath);
        else
             printf("Could not resolve absolute path\n");
        
        global.skybox.loaded = 0;
        return (0);
    }
    global.skybox.width = width;
    global.skybox.height = height;
    global.skybox.addr = mlx_get_data_addr(global.skybox.img, &global.skybox.bits_per_pixel, &global.skybox.line_len, &global.skybox.endian);
    global.skybox.loaded = 1;
    printf("Skybox chargee: %dx%d pixels\n", width, height);
    return (1);
}

int getSkyboxPixel(int x, int y)
{
    char *pixel;

    if (!global.skybox.loaded || x < 0 || x >= global.skybox.width || y < 0 || y >= global.skybox.height)
        return (0);
    pixel = global.skybox.addr + (y * global.skybox.line_len + x * (global.skybox.bits_per_pixel / 8));
    return (*(unsigned int *)pixel);
}

void renderSky(t_render *render)
{
    int maxy;
    int skyColor;
    int y;
    int x;
    float normalizedX;
    float angle;
    float u;
    float v;
    int color;

    maxy = screenH / 2 + (int)(WWAVE_MAG * sinf(global.cam.stepWave));
    if (maxy < 0)
        maxy = 0;
    if (maxy > screenH)
        maxy = screenH;
    if (!global.skybox.loaded)
    {
        skyColor = (77 << 16) | (181 << 8) | 255;
        y = 0;
        while (y < maxy)
        {
            x = 0;
            while (x < screenW)
            {
                putPixel(render, x, y, skyColor);
                x++;
            }
            y++;
        }
        return;
    }
    y = 0;
    while (y < maxy)
    {
        x = 0;
        while (x < screenW)
        {
            normalizedX = (float)x / screenW;
            angle = global.cam.camAngle + (normalizedX - 0.5f) * M_PI;
            u = fmod(angle + M_PI, 2.0f * M_PI) / (2.0f * M_PI) * global.skybox.width;
            if (u < 0)
                u += global.skybox.width;
            v = (float)y / maxy * global.skybox.height;
            if (v >= global.skybox.height)
                v = global.skybox.height - 1;
            color = getSkyboxPixel((int)u, (int)v);
            putPixel(render, x, y, color);
            x++;
        }
        y++;
    }
}
