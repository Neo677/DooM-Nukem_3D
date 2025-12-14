#include "../header/game.h"
#include "../header/rasterize.h"
#include <sys/time.h>

void drawGradientRect(t_render *render, int x, int y, int w, int h, int color1, int color2)
{
    int py;
    int px;
    float ratio;
    int r1;
    int g1;
    int b1;
    int r2;
    int g2;
    int b2;
    int r;
    int g;
    int b;
    int color;

    r1 = (color1 >> 16) & 0xFF;
    g1 = (color1 >> 8) & 0xFF;
    b1 = color1 & 0xFF;
    r2 = (color2 >> 16) & 0xFF;
    g2 = (color2 >> 8) & 0xFF;
    b2 = color2 & 0xFF;
    py = y;
    while (py < y + h && py < screenH)
    {
        ratio = (float)(py - y) / (float)h;
        r = r1 + (int)((r2 - r1) * ratio);
        g = g1 + (int)((g2 - g1) * ratio);
        b = b1 + (int)((b2 - b1) * ratio);
        color = (r << 16) | (g << 8) | b;
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

void drawOverlayBackground(t_render *render)
{
    int overlayX;
    int overlayY;
    int overlayW;
    int overlayH;
    int i;

    overlayX = 10;
    overlayY = 10;
    overlayW = 180;
    overlayH = 240;
    
    // Draw gradient background (dark blue to darker blue)
    drawGradientRect(render, overlayX, overlayY, overlayW, overlayH, 0x1E1E3F, 0x0D0D1A);
    
    // Draw glowing border effect
    drawRectOutline(render, overlayX, overlayY, overlayW, overlayH, 0x00D4FF);
    drawRectOutline(render, overlayX + 1, overlayY + 1, overlayW - 2, overlayH - 2, 0x0088BB);
    
    // Draw accent line at top
    i = overlayX + 5;
    while (i < overlayX + overlayW - 5)
    {
        putPixel(render, i, overlayY + 33, 0x00D4FF);
        i++;
    }
}

void drawOverlayText(t_render *render)
{
    int overlayX;
    int overlayY;
    char buf[64];
    int flyModeColor;

    overlayX = 10;
    overlayY = 10;
    
    // Title with modern styling
    mlx_string_put(render->mlx, render->win, overlayX + 45, overlayY + 20, 0x00D4FF, "STATUS");
    
    // Position info with icons
    snprintf(buf, sizeof(buf), "► X: %.1f", global.cam.camPos.x);
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 50, 0xFFFFFF, buf);
    
    snprintf(buf, sizeof(buf), "► Y: %.1f", global.cam.camPos.y);
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 68, 0xFFFFFF, buf);
    
    snprintf(buf, sizeof(buf), "► Z: %.1f", global.cam.camZ);
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 86, 0xFFFFFF, buf);
    
    // Angle with degree symbol
    snprintf(buf, sizeof(buf), "∠ %.1f°", global.cam.camAngle * 57.2958f);
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 108, 0xAACCFF, buf);
    
    // Sector with highlight
    snprintf(buf, sizeof(buf), "⬢ Sector: %d", global.currentSectorId);
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 130, 0xFFDD00, buf);
    
    // Floor height
    if (global.currentSectorId >= 0)
    {
        snprintf(buf, sizeof(buf), "▔ Floor: %.1f", global.sectors[global.currentSectorId].floorHeight);
        mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 152, 0xFFAA44, buf);
    }
    
    // Ground status with colored indicator
    if (global.cam.onGround)
    {
        mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 174, 0x00FF00, "● Ground");
        mlx_string_put(render->mlx, render->win, overlayX + 90, overlayY + 174, 0x44FF44, "YES");
    }
    else
    {
        mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 174, 0xFF3333, "● Ground");
        mlx_string_put(render->mlx, render->win, overlayX + 90, overlayY + 174, 0xFF6666, "NO");
    }
    
    // Velocity
    snprintf(buf, sizeof(buf), "↕ VelZ: %.1f", global.cam.velZ);
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 196, 0x00FFCC, buf);
    
    // Fly mode indicator
    if (global.flyMode)
    {
        flyModeColor = ((int)(global.cam.camZ * 10) % 20 < 10) ? 0xFFFF00 : 0xFFAA00;
        mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 218, flyModeColor, "✈ FLY MODE");
    }
    
    // Help text with better styling
    mlx_string_put(render->mlx, render->win, overlayX + 15, overlayY + 232, 0x6688AA, "[E]Toggle [V]Fly");
}

void drawMinimapBackground(t_render *render)
{
    int mapX;
    int mapY;
    int mapW;
    int mapH;
    int py;
    int px;
    float minX;
    float minY;
    float maxX;
    float maxY;
    int i;
    int j;
    t_sector *sector;
    t_wall *wall;
    float rangeX;
    float rangeY;
    float denom;
    float scale;
    int colors[] = {0xFF6B6B, 0x4ECDC4, 0xFFE66D, 0x95E1D3, 0xF38181, 0xAA96DA, 0xFCBAD3};
    int sectorColor;
    int x1;
    int y1;
    int x2;
    int y2;
    int wallColor;
    int playerX;
    int playerY;
    int dy;
    int dx;
    int dirX;
    int dirY;

    mapX = screenW - 210;
    mapY = 10;
    mapW = 200;
    mapH = 200;
    py = mapY;
    while (py < mapY + mapH && py < screenH)
    {
        px = mapX;
        while (px < mapX + mapW && px < screenW)
        {
            if (px >= 0 && py >= 0)
                putPixel(render, px, py, 0x0D0D1A);
            px++;
        }
        py++;
    }
    drawRectOutline(render, mapX, mapY, mapW, mapH, 0x00FFFF);
    if (global.sectorCount <= 0)
        return;
    minX = 99999;
    minY = 99999;
    maxX = -99999;
    maxY = -99999;
    i = 0;
    while (i < global.sectorCount)
    {
        sector = &global.sectors[i];
        j = 0;
        while (j < sector->wallCount)
        {
            wall = &global.walls[sector->wallIds[j]];
            if (wall->p1.x < minX) minX = wall->p1.x;
            if (wall->p1.y < minY) minY = wall->p1.y;
            if (wall->p1.x > maxX) maxX = wall->p1.x;
            if (wall->p1.y > maxY) maxY = wall->p1.y;
            if (wall->p2.x < minX) minX = wall->p2.x;
            if (wall->p2.y < minY) minY = wall->p2.y;
            if (wall->p2.x > maxX) maxX = wall->p2.x;
            if (wall->p2.y > maxY) maxY = wall->p2.y;
            j++;
        }
        i++;
    }
    rangeX = maxX - minX;
    rangeY = maxY - minY;
    denom = (rangeX > rangeY ? rangeX : rangeY);
    if (denom < 1.0f)
        denom = 1.0f;
    scale = (mapW - 20) / denom;
    if (scale < 0.05f)
        scale = 0.05f;
    if (scale > 2.0f)
        scale = 2.0f;
    i = 0;
    while (i < global.sectorCount)
    {
        sector = &global.sectors[i];
        sectorColor = colors[i % 7];
        if (i == global.currentSectorId)
            sectorColor = 0x00FF00;
        j = 0;
        while (j < sector->wallCount)
        {
            wall = &global.walls[sector->wallIds[j]];
            x1 = mapX + 10 + (int)((wall->p1.x - minX) * scale);
            y1 = mapY + 25 + (int)((wall->p1.y - minY) * scale);
            x2 = mapX + 10 + (int)((wall->p2.x - minX) * scale);
            y2 = mapY + 25 + (int)((wall->p2.y - minY) * scale);
            if (!((x1 < mapX && x2 < mapX) || (x1 > mapX + mapW && x2 > mapX + mapW) ||
                (y1 < mapY && y2 < mapY) || (y1 > mapY + mapH && y2 > mapY + mapH)))
            {
                wallColor = wall->isPortal ? 0x0066FF : sectorColor;
                drawLine(render, x1, y1, x2, y2, wallColor);
            }
            j++;
        }
        i++;
    }
    playerX = mapX + 10 + (int)((global.cam.camPos.x - minX) * scale);
    playerY = mapY + 25 + (int)((global.cam.camPos.y - minY) * scale);
    if (playerX < mapX + 2) playerX = mapX + 2;
    if (playerX > mapX + mapW - 3) playerX = mapX + mapW - 3;
    if (playerY < mapY + 2) playerY = mapY + 2;
    if (playerY > mapY + mapH - 3) playerY = mapY + mapH - 3;
    dy = -3;
    while (dy <= 3)
    {
        dx = -3;
        while (dx <= 3)
        {
            if (dx * dx + dy * dy <= 9)
                putPixel(render, playerX + dx, playerY + dy, 0xFFFFFF);
            dx++;
        }
        dy++;
    }
    dirX = playerX + (int)(cos(global.cam.camAngle) * 15);
    dirY = playerY + (int)(sin(global.cam.camAngle) * 15);
    drawLine(render, playerX, playerY, dirX, dirY, 0xFF0000);
}

void drawMinimapText(t_render *render)
{
    int mapX;
    int mapY;
    int mapH;
    int legendY;
    int colors[] = {0xFF6B6B, 0x4ECDC4, 0xFFE66D, 0x95E1D3, 0xF38181, 0xAA96DA, 0xFCBAD3};
    int i;
    char buf[32];
    int color;

    mapX = screenW - 210;
    mapY = 10;
    mapH = 200;
    mlx_string_put(render->mlx, render->win, mapX + 60, mapY + 15, 0x00FFFF, "MINIMAP");
    legendY = mapY + mapH + 10;
    mlx_string_put(render->mlx, render->win, mapX, legendY, 0xFFFFFF, "Sectors:");
    i = 0;
    while (i < global.sectorCount && i < 4)
    {
        color = (i == global.currentSectorId) ? 0x00FF00 : colors[i % 7];
        snprintf(buf, sizeof(buf), "S%d", i);
        mlx_string_put(render->mlx, render->win, mapX + 70 + i * 35, legendY, color, buf);
        i++;
    }
}

void handleOverlayToggle(void)
{
    if (global.g_keys[PRESS_E] && !global.keyE_pressed)
    {
        global.showOverlay = !global.showOverlay;
        global.keyE_pressed = 1;
    }
    if (!global.g_keys[PRESS_E])
        global.keyE_pressed = 0;
}

void overlay_begin_frame(void)
{
    struct timeval tv;
    double currentTime;
    double elapsed;
    int i;

    if (!global.debugOverlay.enabled)
        return;
    global.debugOverlay.sectorsVisited = 0;
    global.debugOverlay.portalTraversals = 0;
    global.debugOverlay.maxDepthThisFrame = 0;
    global.debugOverlay.clipWindowStart = 0;
    global.debugOverlay.clipWindowEnd = screenW - 1;
    global.debugOverlay.frameCount++;
    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec + tv.tv_usec / 1000000.0;
    if (global.debugOverlay.lastFpsTime == 0.0)
        global.debugOverlay.lastFpsTime = currentTime;
    elapsed = currentTime - global.debugOverlay.lastFpsTime;
    if (elapsed >= 0.5)
    {
        global.debugOverlay.currentFPS = global.debugOverlay.frameCount / elapsed;
        global.debugOverlay.lastFrameTime = (float)(elapsed * 1000.0 / global.debugOverlay.frameCount);
        global.debugOverlay.frameCount = 0;
        global.debugOverlay.lastFpsTime = currentTime;
    }
    i = 0;
    while (i < 3)
    {
        global.debugOverlay.samples[i].distance = -1.0f;
        global.debugOverlay.samples[i].wallId = -1;
        global.debugOverlay.samples[i].sectorId = -1;
        global.debugOverlay.samples[i].zbuffer = -1.0f;
        i++;
    }
    global.debugOverlay.samples[0].screenX = screenW / 2;
    global.debugOverlay.samples[1].screenX = screenW / 2 - 100;
    global.debugOverlay.samples[2].screenX = screenW / 2 + 100;
}

void overlay_handle_keys(void)
{
    if (global.g_keys[F1] && !global.debugOverlay.keyF1Pressed)
    {
        global.debugOverlay.enabled = !global.debugOverlay.enabled;
        global.debugOverlay.keyF1Pressed = 1;
    }
    if (!global.g_keys[F1])
        global.debugOverlay.keyF1Pressed = 0;
    if (global.g_keys[F2] && !global.debugOverlay.keyF2Pressed)
    {
        global.debugOverlay.showPlayer = !global.debugOverlay.showPlayer;
        global.debugOverlay.keyF2Pressed = 1;
    }
    if (!global.g_keys[F2])
        global.debugOverlay.keyF2Pressed = 0;
    if (global.g_keys[F3] && !global.debugOverlay.keyF3Pressed)
    {
        global.debugOverlay.showRender = !global.debugOverlay.showRender;
        global.debugOverlay.keyF3Pressed = 1;
    }
    if (!global.g_keys[F3])
        global.debugOverlay.keyF3Pressed = 0;
    if (global.g_keys[F4] && !global.debugOverlay.keyF4Pressed)
    {
        global.debugOverlay.showRaySamples = !global.debugOverlay.showRaySamples;
        global.debugOverlay.keyF4Pressed = 1;
    }
    if (!global.g_keys[F4])
        global.debugOverlay.keyF4Pressed = 0;
}

void overlay_draw(t_render *render)
{
    int panelX;
    int panelY;
    int lineHeight;
    char buf[128];
    int y;
    t_sector *s;
    int i;
    t_ray_sample *sample;
    int warnY;
    t_wall *w;

    if (!global.debugOverlay.enabled)
        return;
    panelX = 10;
    panelY = 10;
    lineHeight = 18;
    if (global.debugOverlay.showPlayer)
    {
        y = panelY;
        mlx_string_put(render->mlx, render->win, panelX, y, 0x00FF00, "=== PLAYER (F2) ===");
        y += lineHeight + 5;
        snprintf(buf, sizeof(buf), "Pos: (%.1f, %.1f, %.1f)", global.cam.camPos.x, global.cam.camPos.y, global.cam.camZ);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFFFF, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "VelZ: %.1f", global.cam.velZ);
        mlx_string_put(render->mlx, render->win, panelX, y, 0x00FFFF, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "OnGround: %s", global.cam.onGround ? "YES" : "NO");
        mlx_string_put(render->mlx, render->win, panelX, y, global.cam.onGround ? 0x00FF00 : 0xFF0000, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "Yaw: %.2f Pitch: %.2f", global.cam.camAngle, global.cam.camPitch);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFFFF, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "Sector: %d", global.currentSectorId);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFF00, buf);
        y += lineHeight;
        if (global.currentSectorId >= 0 && global.currentSectorId < global.sectorCount)
        {
            s = &global.sectors[global.currentSectorId];
            snprintf(buf, sizeof(buf), "Floor: %.1f Ceil: %.1f", s->floorHeight, s->ceilingHeight);
            mlx_string_put(render->mlx, render->win, panelX, y, 0xFF8800, buf);
            y += lineHeight;
            if (s->floorHeight >= s->ceilingHeight)
            {
                mlx_string_put(render->mlx, render->win, panelX, y, 0xFF0000, "WARNING: Floor >= Ceiling!");
                y += lineHeight;
            }
        }
        panelY = y + 10;
    }
    if (global.debugOverlay.showRender)
    {
        y = panelY;
        mlx_string_put(render->mlx, render->win, panelX, y, 0x00FFFF, "=== RENDER (F3) ===");
        y += lineHeight + 5;
        snprintf(buf, sizeof(buf), "FPS: %.1f (%.2fms)", global.debugOverlay.currentFPS, global.debugOverlay.lastFrameTime);
        mlx_string_put(render->mlx, render->win, panelX, y, 0x00FF00, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "Max Depth: %d/%d", global.debugOverlay.maxDepthThisFrame, MAX_PORTAL_DEPTH);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFFFF, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "Sectors Visited: %d", global.debugOverlay.sectorsVisited);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFFFF, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "Portal Traversals: %d", global.debugOverlay.portalTraversals);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFFFF, buf);
        y += lineHeight;
        snprintf(buf, sizeof(buf), "Clip: [%d, %d]", global.debugOverlay.clipWindowStart, global.debugOverlay.clipWindowEnd);
        mlx_string_put(render->mlx, render->win, panelX, y, 0xAAAAAA, buf);
        y += lineHeight;
        panelY = y + 10;
    }
    if (global.debugOverlay.showRaySamples)
    {
        y = panelY;
        mlx_string_put(render->mlx, render->win, panelX, y, 0xFF00FF, "=== RAY SAMPLES (F4) ===");
        y += lineHeight + 5;
        i = 0;
        while (i < 3)
        {
            sample = &global.debugOverlay.samples[i];
            if (sample->distance >= 0)
            {
                snprintf(buf, sizeof(buf), "X%d: D=%.1f W=%d S=%d Z=%.1f", sample->screenX, sample->distance, sample->wallId, sample->sectorId, sample->zbuffer);
                mlx_string_put(render->mlx, render->win, panelX, y, 0xFFFFFF, buf);
            }
            else
            {
                snprintf(buf, sizeof(buf), "X%d: No hit", sample->screenX);
                mlx_string_put(render->mlx, render->win, panelX, y, 0x888888, buf);
            }
            y += lineHeight;
            i++;
        }
        panelY = y + 10;
    }
    warnY = screenH - 40;
    if (global.currentSectorId < 0 || global.currentSectorId >= global.sectorCount)
    {
        mlx_string_put(render->mlx, render->win, 10, warnY, 0xFF0000, "WARNING: Invalid sector ID!");
        warnY -= lineHeight;
    }
    i = 0;
    while (i < global.wallCount)
    {
        w = &global.walls[i];
        if (w->isPortal && (w->backSectorId < 0 || w->backSectorId >= global.sectorCount))
        {
            snprintf(buf, sizeof(buf), "WARNING: Wall %d portal to missing sector %d", w->id, w->backSectorId);
            mlx_string_put(render->mlx, render->win, 10, warnY, 0xFF0000, buf);
            warnY -= lineHeight;
            break;
        }
        i++;
    }
    mlx_string_put(render->mlx, render->win, screenW - 200, screenH - 20, 0x888888, "F1:Toggle F2:Player F3:Render F4:Rays");
}
