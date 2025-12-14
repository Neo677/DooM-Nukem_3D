#include "./header/game.h"
#include "./header/types.h"
#include "./header/texture.h"
#include "./header/rasterize.h"
#include "./header/ceiling.h"
#include "./header/map_test_arena.h"

t_global global;

void init(void)
{
    int i;

    global.cam.camAngle = 1.57;
    global.cam.camPos.x = 400.0;
    global.cam.camPos.y = 100.0;
    global.cam.stepWave = 0.0;
    global.cam.camPitch = 0.0f;
    global.currentSectorId = -1;
    global.cam.targetZ = EYE_HEIGHT;
    global.showOverlay = 1;
    global.keyE_pressed = 0;
    global.keyV_pressed = 0;
    global.flyMode = 0;
    global.mouseInit = 0;
    global.lastMouseX = 0;
    global.lastMouseY = 0;
    memset(&global.debugOverlay, 0, sizeof(t_debug_overlay));
    global.debugOverlay.enabled = 0;
    global.debugOverlay.showPlayer = 1;
    global.debugOverlay.showRender = 1;
    global.debugOverlay.showRaySamples = 0;
    i = 0;
    while (i < MAX_POLYS)
    {
        global.polys[i].vertCnt = 0;
        i++;
    }
    init_test_arena_map();
}

int main(void)
{
    t_render render;

    memset(global.g_keys, 0, sizeof(global.g_keys));
    render.mlx = mlx_init();
    if (!render.mlx)
    {
        fprintf(stderr, "Erreur: mlx_init() a echoue\n");
        return (1);
    }
    render.win = mlx_new_window(render.mlx, screenW, screenH, "doom-nukem");
    if (!render.win)
    {
        fprintf(stderr, "Erreur: mlx_new_window() a echoue\n");
        return (1);
    }
    render.img = mlx_new_image(render.mlx, screenW, screenH);
    if (!render.img)
    {
        fprintf(stderr, "Erreur: mlx_new_image() a echoue (%dx%d)\n", screenW, screenH);
        mlx_destroy_window(render.mlx, render.win);
        return (1);
    }
    render.addr = mlx_get_data_addr(render.img, &render.bits_per_pixel, &render.line_len, &render.endian);
    global.skybox.loaded = 0;
    loadSkybox(&render, "../sprite_selection/skybox/Orangecloudmtn.xpm");
    initTextureManager(&render);
    init();
    sortPolysByDeph();
    mlx_hook(render.win, 2, 1L << 0, key_press, &render);
    mlx_hook(render.win, 3, 1L << 1, key_release, &render);
    mlx_hook(render.win, 6, 1L << 6, mouse_move, &render);
    mlx_hook(render.win, 17, 0, close_window, &render);
    mlx_loop_hook(render.mlx, game_loop, &render);
    mlx_loop(render.mlx);
    return (0);
}