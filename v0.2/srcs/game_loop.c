#include "../header/game.h"
#include "../header/rasterize.h"
#include <sys/time.h>

double getDeltaTime(void)
{
    struct timeval tv;
    double currentTime;
    double deltaTime;

    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec + tv.tv_usec / 1000000.0;
    if (global.lastTime == 0.0)
    {
        global.lastTime = currentTime;
        return (0.016);
    }
    deltaTime = currentTime - global.lastTime;
    global.lastTime = currentTime;
    if (deltaTime > 0.1 || deltaTime <= 0.0)
        deltaTime = 0.016;
    return (deltaTime);
}

void display_debug_info(t_render *render)
{
    (void)render;
}

int game_loop(t_render *render)
{
    double dt;

    dt = getDeltaTime();
    overlay_begin_frame();
    handleOverlayToggle();
    overlay_handle_keys();
    handleJump();
    CameraTranslate(dt);
    updatePlayerZ(dt);
    clearDepthBuffer();
    clearScreen(render, 0x000000);
    render_scene(render);
    if (global.showOverlay)
    {
        drawOverlayBackground(render);
        drawMinimapBackground(render);
    }
    updateScreen(render);
    if (global.showOverlay)
    {
        drawOverlayText(render);
        drawMinimapText(render);
    }
    overlay_draw(render);
    display_debug_info(render);
    return (0);
}
