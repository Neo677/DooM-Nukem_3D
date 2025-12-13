
#include "./header/game.h"
#include "./header/types.h"
#include "./header/texture.h"
#include "./header/rasterize.h"
#include "./header/ceiling.h"
#include <sys/time.h>

t_global global;

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

    for (y = 0; y < screenH; y++)
        for (x = 0; x < screenW; x++) {
            // putPixel(render, x, y, 0xFFFFFF);
            putPixel(render, x, y, color);
            
        }
}

void drawLine(t_render *render, int x0, int y0, int x1, int y1, int color)
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
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1)
    {
        putPixel(render, x0, y0, color);

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

// ========================================
// OVERLAY & MINIMAP FUNCTIONS
// ========================================

void drawRect(t_render *render, int x, int y, int w, int h, int color)
{
    for (int py = y; py < y + h && py < screenH; py++)
        for (int px = x; px < x + w && px < screenW; px++)
            if (px >= 0 && py >= 0)
                putPixel(render, px, py, color);
}

void drawRectOutline(t_render *render, int x, int y, int w, int h, int color)
{
    drawLine(render, x, y, x + w, y, color);
    drawLine(render, x + w, y, x + w, y + h, color);
    drawLine(render, x + w, y + h, x, y + h, color);
    drawLine(render, x, y + h, x, y, color);
}

void drawOverlay(t_render *render)
{
    int overlayX = 10;
    int overlayY = 10;
    int overlayW = 160;
    int overlayH = 220;
    
    // Fond semi-transparent (noir)
    for (int py = overlayY; py < overlayY + overlayH && py < screenH; py++) {
        for (int px = overlayX; px < overlayX + overlayW && px < screenW; px++) {
            if (px >= 0 && py >= 0)
                putPixel(render, px, py, 0x1A1A2E);
        }
    }
    
    // Bordure
    drawRectOutline(render, overlayX, overlayY, overlayW, overlayH, 0x00FF00);
    
    // Titre
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 20, 0x00FF00, "=== STATUS ===");
    
    // Infos joueur
    char buf[64];
    sprintf(buf, "X: %.1f", global.cam.camPos.x);
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 45, 0xFFFFFF, buf);
    
    sprintf(buf, "Y: %.1f", global.cam.camPos.y);
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 60, 0xFFFFFF, buf);
    
    sprintf(buf, "Z: %.1f", global.cam.camZ);
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 75, 0xFFFFFF, buf);
    
    sprintf(buf, "Angle: %.2f", global.cam.camAngle);
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 95, 0xFFFFFF, buf);
    
    sprintf(buf, "Sector: %d", global.currentSectorId);
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 115, 0xFFFF00, buf);
    
    // Afficher le floorHeight du secteur actuel
    if (global.currentSectorId >= 0) {
        sprintf(buf, "Floor: %.1f", global.sectors[global.currentSectorId].floorHeight);
        mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 135, 0xFF8800, buf);
    }
    
    // État du joueur (au sol ou en l'air)
    if (global.cam.onGround)
        mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 155, 0x00FF00, "Ground: YES");
    else
        mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 155, 0xFF0000, "Ground: NO");
    
    sprintf(buf, "VelZ: %.1f", global.cam.velZ);
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 175, 0x00FFFF, buf);
    
    mlx_string_put(render->mlx, render->win, overlayX + 10, overlayY + 200, 0x888888, "[E] Toggle [SPACE] Jump");
}

void drawMinimap(t_render *render)
{
    int mapX = screenW - 210;
    int mapY = 10;
    int mapW = 200;
    int mapH = 200;
    
    // Trouver les bounds de la carte
    float minX = 99999, minY = 99999, maxX = -99999, maxY = -99999;
    for (int i = 0; i < global.sectorCount; i++) {
        t_sector *sector = &global.sectors[i];
        for (int j = 0; j < sector->wallCount; j++) {
            t_wall *wall = &global.walls[sector->wallIds[j]];
            if (wall->p1.x < minX) minX = wall->p1.x;
            if (wall->p1.y < minY) minY = wall->p1.y;
            if (wall->p1.x > maxX) maxX = wall->p1.x;
            if (wall->p1.y > maxY) maxY = wall->p1.y;
            if (wall->p2.x < minX) minX = wall->p2.x;
            if (wall->p2.y < minY) minY = wall->p2.y;
            if (wall->p2.x > maxX) maxX = wall->p2.x;
            if (wall->p2.y > maxY) maxY = wall->p2.y;
        }
    }
    
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    float scale = (mapW - 20) / (rangeX > rangeY ? rangeX : rangeY);
    
    // Fond de la minimap
    for (int py = mapY; py < mapY + mapH && py < screenH; py++) {
        for (int px = mapX; px < mapX + mapW && px < screenW; px++) {
            if (px >= 0 && py >= 0)
                putPixel(render, px, py, 0x0D0D1A);
        }
    }
    
    // Bordure
    drawRectOutline(render, mapX, mapY, mapW, mapH, 0x00FFFF);
    
    // Titre
    mlx_string_put(render->mlx, render->win, mapX + 60, mapY + 15, 0x00FFFF, "MINIMAP");
    
    // Dessiner les secteurs
    int colors[] = {0xFF6B6B, 0x4ECDC4, 0xFFE66D, 0x95E1D3, 0xF38181, 0xAA96DA, 0xFCBAD3};
    
    for (int i = 0; i < global.sectorCount; i++) {
        t_sector *sector = &global.sectors[i];
        int sectorColor = colors[i % 7];
        
        // Si c'est le secteur actuel, le rendre plus brillant
        if (i == global.currentSectorId)
            sectorColor = 0x00FF00;
        
        for (int j = 0; j < sector->wallCount; j++) {
            t_wall *wall = &global.walls[sector->wallIds[j]];
            
            int x1 = mapX + 10 + (int)((wall->p1.x - minX) * scale);
            int y1 = mapY + 25 + (int)((wall->p1.y - minY) * scale);
            int x2 = mapX + 10 + (int)((wall->p2.x - minX) * scale);
            int y2 = mapY + 25 + (int)((wall->p2.y - minY) * scale);
            
            // Portails en bleu, murs solides en couleur du secteur
            int wallColor = wall->isPortal ? 0x0066FF : sectorColor;
            drawLine(render, x1, y1, x2, y2, wallColor);
        }
    }
    
    // Dessiner le joueur (triangle directionnel)
    int playerX = mapX + 10 + (int)((global.cam.camPos.x - minX) * scale);
    int playerY = mapY + 25 + (int)((global.cam.camPos.y - minY) * scale);
    
    // Point du joueur
    for (int dy = -3; dy <= 3; dy++) {
        for (int dx = -3; dx <= 3; dx++) {
            if (dx*dx + dy*dy <= 9)
                putPixel(render, playerX + dx, playerY + dy, 0xFFFFFF);
        }
    }
    
    // Direction du joueur (ligne)
    int dirX = playerX + (int)(cos(global.cam.camAngle) * 15);
    int dirY = playerY + (int)(sin(global.cam.camAngle) * 15);
    drawLine(render, playerX, playerY, dirX, dirY, 0xFF0000);
    
    // Légende des secteurs
    int legendY = mapY + mapH + 10;
    mlx_string_put(render->mlx, render->win, mapX, legendY, 0xFFFFFF, "Sectors:");
    
    for (int i = 0; i < global.sectorCount && i < 4; i++) {
        char buf[32];
        int color = (i == global.currentSectorId) ? 0x00FF00 : colors[i % 7];
        sprintf(buf, "S%d", i);
        mlx_string_put(render->mlx, render->win, mapX + 70 + i * 35, legendY, color, buf);
    }
}

int key_press(int keycode, t_render *render)
{
    (void)render;
    if (keycode >= 0 && keycode < 65536)
        global.g_keys[keycode] = 1;
    if (keycode == ESC) {
        mlx_destroy_window(render->mlx, render->win);
        exit (0);
    }
    return (0);
}

int key_release(int keycode, t_render *render)
{
    (void)render;
    if (keycode >= 0 && keycode < 65536)
        global.g_keys[keycode] = 0;
    return (0);
}

void CameraTranslate(double deltaTime)
{
    int isMoving = 0;
    global.cam.oldCamPos = global.cam.camPos;
    Vec2_t newPos = global.cam.camPos;

    if (global.g_keys[W]) {
        newPos.x += MOV_SPEED * cos(global.cam.camAngle) * deltaTime;
        newPos.y += MOV_SPEED * sin(global.cam.camAngle) * deltaTime;
        isMoving = 1;
    } else if (global.g_keys[S]) {
        newPos.x -= MOV_SPEED * cos(global.cam.camAngle) * deltaTime;
        newPos.y -= MOV_SPEED * sin(global.cam.camAngle) * deltaTime;
        isMoving = 1;
    }

    global.cam.camPos = resolveCollision(newPos);

    if (global.g_keys[A]) {
        global.cam.camAngle -= ROT_SPEED * deltaTime;
    } else if (global.g_keys[D]) {
        global.cam.camAngle += ROT_SPEED * deltaTime;
    }

    if (isMoving)
        global.cam.stepWave += 3 * deltaTime;
    if (global.cam.stepWave > M_PI * 2)
        global.cam.stepWave = 0;
}

// ========================================
// PHYSIQUE VERTICALE (Step-up/down, Gravité, Saut)
// ========================================

void handleJump(void)
{
    // Saut avec ESPACE (seulement si au sol)
    if (global.g_keys[SPACE] && global.cam.onGround)
    {
        global.cam.velZ = JUMP_VELOCITY;
        global.cam.onGround = 0;
    }
}

void updatePlayerZ(double deltaTime)
{
    // Récupérer le secteur actuel
    if (global.currentSectorId < 0)
        return;
    
    t_sector *currentSector = &global.sectors[global.currentSectorId];
    float targetFloorZ = currentSector->floorHeight;
    
    // 1. APPLIQUER LA GRAVITÉ (si en l'air)
    if (!global.cam.onGround)
    {
        global.cam.velZ -= GRAVITY * deltaTime;
        
        // Limiter la vélocité de chute
        if (global.cam.velZ < -MAX_FALL_VELOCITY)
            global.cam.velZ = -MAX_FALL_VELOCITY;
        
        global.cam.camZ += global.cam.velZ * deltaTime;
    }
    
    // 2. CALCULER LA HAUTEUR DES PIEDS
    global.cam.footZ = global.cam.camZ - EYE_HEIGHT;
    
    // 3. VÉRIFIER COLLISION AVEC LE SOL
    if (global.cam.footZ <= targetFloorZ)
    {
        // Atterrissage
        global.cam.footZ = targetFloorZ;
        global.cam.camZ = targetFloorZ + EYE_HEIGHT;
        global.cam.velZ = 0;
        global.cam.onGround = 1;
    }
    
    // 4. VÉRIFIER COLLISION AVEC LE PLAFOND
    float ceilingLimit = currentSector->ceilingHeight - 5.0f;
    if (global.cam.camZ > ceilingLimit)
    {
        global.cam.camZ = ceilingLimit;
        if (global.cam.velZ > 0)
            global.cam.velZ = 0;  // Arrêter la montée
    }
    
    // 5. TRANSITION FLUIDE (interpolation vers targetZ pour step smooth)
    if (global.cam.onGround && global.cam.targetZ != global.cam.camZ)
    {
        float diff = global.cam.targetZ - global.cam.camZ;
        float step = STEP_SMOOTH_SPEED * deltaTime;
        
        if (fabs(diff) < step)
            global.cam.camZ = global.cam.targetZ;
        else if (diff > 0)
            global.cam.camZ += step;
        else
            global.cam.camZ -= step;
        
        global.cam.footZ = global.cam.camZ - EYE_HEIGHT;
    }
}

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
        return 0.016;  // first frame fallback
    }
    
    deltaTime = currentTime - global.lastTime;
    global.lastTime = currentTime;
    
    if (deltaTime > 0.1 || deltaTime <= 0.0)
        deltaTime = 0.016;
    
    return deltaTime;
}

void display_debug_info(t_render *render)
{
    (void)render;
}

int createWall(Vec2_t p1, Vec2_t p2, int frontSectorId)
{
    if (global.wallCount >= MAX_WALLS)
        return (-1);

    int wallId = global.wallCount;
    t_wall *wall = &global.walls[wallId];

    wall->id = wallId;
    wall->p1 = p1;
    wall->p2 = p2;
    wall->frontSectorId = frontSectorId;
    wall->backSectorId = -1;
    wall->middleTextureId = 0;
    wall->upperTextureId = -1;
    wall->lowerTextureId = -1;
    wall->isPortal = 0;
    wall->twoSided = 0;

    global.wallCount++;
    return (wallId);
}

void convertPolysToSectors()
{
    global.sectorCount = 0;
    global.wallCount = 0;

    for (int i = 0; i < MAX_POLYS; i++)
    {
        if (global.polys[i].vertCnt < 3)
            continue;

        t_sector *sector = &global.sectors[global.sectorCount];
        sector->id = global.sectorCount;
        sector->floorHeight = 0.0f;
        sector->ceilingHeight = global.polys[i].height / 1000.0f;
        sector->floorTextureId = findTextureByName("floor");
        sector->ceilingTextureId = -1;
        sector->lightLevel = 255;
        sector->wallCount = global.polys[i].vertCnt;
        sector->visited = 0;

        for (int j = 0; j < global.polys[i].vertCnt; j++)
        {
            Vec2_t p1 = global.polys[i].vert[j];
            Vec2_t p2 = global.polys[i].vert[(j + 1) % global.polys[i].vertCnt];

            int wallId = createWall(p1, p2, global.sectorCount);
            if (wallId >= 0)
            {
                sector->wallIds[j] = wallId;
                global.walls[wallId].middleTextureId = global.polys[i].textureId;
            }
        }

        global.sectorCount++;
    }
}

int wallsAreConnected(t_wall *w1, t_wall *w2)
{
    float epsilon = 1.0f;
    
    // Cas 1: Murs inversés (p1-p2 ↔ p2-p1) - cas classique
    float dist_p1_p2 = len_vec(w1->p1, w2->p2);
    float dist_p2_p1 = len_vec(w1->p2, w2->p1);
    if (dist_p1_p2 < epsilon && dist_p2_p1 < epsilon)
        return (1);
    
    // Cas 2: Murs identiques (p1-p2 ↔ p1-p2) - même direction
    float dist_p1_p1 = len_vec(w1->p1, w2->p1);
    float dist_p2_p2 = len_vec(w1->p2, w2->p2);
    if (dist_p1_p1 < epsilon && dist_p2_p2 < epsilon)
        return (1);
    
    return (0);
}

void detectPortals()
{
    int portalCount = 0;

    for (int i = 0; i < global.wallCount; i++)
    {
        for (int j = i + 1; j < global.wallCount; j++)
        {
            if (wallsAreConnected(&global.walls[i], &global.walls[j]))
            {
                global.walls[i].backSectorId = global.walls[j].frontSectorId;
                global.walls[j].backSectorId = global.walls[i].frontSectorId;
                global.walls[i].isPortal = 1;
                global.walls[j].isPortal = 1;
                global.walls[i].twoSided = 1;
                global.walls[j].twoSided = 1;

                global.walls[i].middleTextureId = -1;
                global.walls[j].middleTextureId = -1;

                portalCount++;
            }
        }
    }

    (void)portalCount;
}

int loadSkybox(t_render *render, const char *path)
{
    int width, height;

    global.skybox.img = mlx_xpm_file_to_image(render->mlx, (char *)path, &width, &height);
    if (!global.skybox.img) {
        // printf("Erreur: Impossible de charger la skybox: %s\n", path);
        global.skybox.loaded = 0;
        return (0);
    }
    global.skybox.width = width;
    global.skybox.height = height;
    global.skybox.addr = mlx_get_data_addr(global.skybox.img, &global.skybox.bits_per_pixel, &global.skybox.line_len, &global.skybox.endian);
    global.skybox.loaded = 1;

    // printf("Skybox chargee: %dx%d pixels\n", width, height);
    return (1);
}

int getSkyboxPixel(int x, int y) 
{
    if (!global.skybox.loaded || x < 0 || x >= global.skybox.width || y < 0 || y >= global.skybox.height)
        return (0);
    char *pixel = global.skybox.addr + (y * global.skybox.line_len + x * (global.skybox.bits_per_pixel / 8));
    return (*(unsigned int *)pixel);
}

float cross2dpoints(float x1, float y1, float x2, float y2) {
    return (x1 * y2 - y1 * x2);
}

Vec2_t intersection(float x1, float y1, float x2, float y2,float x3, float y3, float x4, float y4)
{
    Vec2_t p;

    p.x = cross2dpoints(x1, y1, x2, y2);
    p.y = cross2dpoints(x3, y3, x4, y4);
    float det = cross2dpoints(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    p.x = cross2dpoints(p.x, x1 - x2, p.y, x3 - x4) / det;
    p.y = cross2dpoints(p.x, y1 - y2, p.y, y3 - y4) / det;

    return (p);
}

void renderSky(t_render *render)
{
    int maxy = screenH / 2 + (int)(WWAVE_MAG * sinf(global.cam.stepWave));
    
    if (maxy < 0)
        maxy = 0;
    if (maxy > screenH)
        maxy = screenH;

    if (!global.skybox.loaded) {
        int skyColor = (77 << 16) | (181 << 8) | 255; // 0x4DB5FF
        for (int y = 0; y < maxy; y++) {
            for (int x = 0; x < screenW; x++) {
                putPixel(render, x, y, skyColor);
            }
        }
    }

    for (int y = 0; y < maxy; y++) {
        for (int x = 0; x < screenW; x++) {
            float normalizedX = (float)x / screenW;
            float angle = global.cam.camAngle + (normalizedX - 0.5f) * M_PI;

            float u = fmod(angle + M_PI, 2.0f * M_PI) / (2.0f * M_PI) * global.skybox.width;
            if (u < 0)
                u += global.skybox.width;
            
            float v = (float)y / maxy * global.skybox.height;
            if (v >= global.skybox.height) 
                v = global.skybox.height - 1;

            int color = getSkyboxPixel((int)u, (int)v);
            putPixel(render, x, y, color);
        }
    }

}

void renderGround(t_render *render)
{
    float waveVal = WWAVE_MAG * sinf(global.cam.stepWave);
    int starty = screenH / 2 + (int)waveVal;

    if (starty < 0)
        starty = 0;
    if (starty >= screenH)
        starty = screenH - 1;

    // Sol noir avec léger dégradé pour la profondeur
    for (int y = starty; y < screenH; y++) {
        float rowDistance = (screenH / 2.0f) / (y - screenH / 2.0f + 0.1f);
        
        for (int x = 0; x < screenW; x++) {
            // Sol noir avec légère variation pour la profondeur
            float darkness = 1.0f - (rowDistance / 25.0f);
            if (darkness < 0.1f) darkness = 0.1f;
            if (darkness > 0.4f) darkness = 0.4f;
            
            int intensity = (int)(40 * darkness);  // Noir avec léger reflet
            int color = (intensity << 16) | (intensity << 8) | intensity;
            putPixel(render, x, y, color);
        }
    }
}

float len(Vec2_t pointA, Vec2_t pointB)
{
    float distY = pointB.y - pointA.y;
    float distX = pointB.x - pointA.x;
    return (sqrt(distX * distX + distY * distY));
}

float closestVertexInPoly(polygon_t poly, Vec2_t pos)
{
    float dist = 9999999;
    for (int i = 0; i < poly.vertCnt; i++) {
        float d = len(pos, poly.vert[i]);
        if (d < dist)
            dist = d;
    }
    return (dist);
}


void sortPolysByDeph()
{

    for (int i = 0; i < MAX_POLYS; i++)
        global.polyOrder[i] = i;
    for (int i = 0; i < MAX_POLYS; i++) {
        if (global.polys[i].vertCnt < 2) {
            global.polyDistances[i] = -1.0f;
            continue;
        }
        float centerX = 0, centerY = 0;
        for (int j = 0; j < global.polys[i].vertCnt; j++) {
            centerX += global.polys[i].vert[j].x;
            centerY += global.polys[i].vert[j].y;
        }
        centerX /= global.polys[i].vertCnt;
        centerY /= global.polys[i].vertCnt;

        float dx = centerX - global.cam.camPos.x;
        float dy = centerY - global.cam.camPos.y;
        global.polyDistances[i] = sqrt(dx * dx + dy * dy);
        global.polys[i].curDist = global.polyDistances[i];
    }
    for (int i = 0; i < MAX_POLYS - 1; i++) {
        for (int j = i + 1; j < MAX_POLYS; j++) {
            if (global.polyDistances[global.polyOrder[i]] < global.polyDistances[global.polyOrder[j]]) {
                int tmp = global.polyOrder[i];
                global.polyOrder[i] = global.polyOrder[j];
                global.polyOrder[j] = tmp;
            }
        }
    }
}

int projectWallToScreen(t_wall *wall, int *screenX1, int *screenX2, float *z1_out, float *z2_out)
{
    Vec2_t p1 = wall->p1;
    Vec2_t p2 = wall->p2;
    float distX1 = p1.x - global.cam.camPos.x;
    float distY1 = p1.y - global.cam.camPos.y;
    float z1 = distX1 * cos(global.cam.camAngle) + distY1 * sin(global.cam.camAngle);

    float distX2 = p2.x - global.cam.camPos.x;
    float distY2 = p2.y - global.cam.camPos.y;
    float z2 = distX2 * cos(global.cam.camAngle) + distY2 * sin(global.cam.camAngle);

    if (z1 <= 0.1f && z2 <= 0.1f)
        return (0);
    if (z1 <= 0.1f)
        z1 = 0.1f;
    if (z2 <= 0.1f)
        z2 = 0.1f;

    distX1 = distX1 * sin(global.cam.camAngle) - distY1 * cos(global.cam.camAngle);
    distX2 = distX2 * sin(global.cam.camAngle) - distY2 * cos(global.cam.camAngle);

    float centerScreenW = screenW / 2;
    float widthRatio = screenW / 2;

    float x1 = -distX1 * widthRatio / z1;
    float x2 = -distX2 * widthRatio / z2;

    *screenX1 = (int)(centerScreenW + x1);
    *screenX2 = (int)(centerScreenW + x2);
    *z1_out = z1;
    *z2_out = z2;

    return (1);
}

void drawWallSlice(t_render *render, t_wall *wall, int screenX1, int screenX2, float z1, float z2, float bottomHeight, float topHeight, int textureId)
{
    if (screenX1 >= screenW || screenX2 < 0)
        return;
    float heightRatio = (screenW * screenH) / 60.0f;
    float centerScreenH = screenH / 2;

    float y1_top = (bottomHeight - topHeight - heightRatio) / z1;
    float y1_bot = (bottomHeight - heightRatio) / z1;
    float y2_top = (bottomHeight - topHeight - heightRatio) / z2;
    float y2_bot = (bottomHeight - heightRatio) / z2;

    int startX = (screenX1 < 0) ? 0 : screenX1;
    int endX = (screenX2 >= screenW) ? screenW - 1 : screenX2;
    
    for (int x = startX; x <= endX; x++)
    {
        float t = (float)(x - screenX1) / (float)(screenX2 - screenX1 + 1);
        float z = z1 + t * (z2 - z1);

        int yTop = (int)(centerScreenH + (y1_top + t * (y2_top - y1_top)));
        int yBot = (int)(centerScreenH + (y1_bot + t * (y2_bot - y1_bot)));

        if (yTop < 0)
            yTop = 0;
        if (yBot >= screenH)
            yBot = screenH - 1;

        for (int y = yTop; y <= yBot; y++)
        {
            color_t c = getColorBydistance(z);
            int color = (c.R << 16) | (c.G << 8) | c.B;
            putPixel(render, x, y, color);
        }
    }

    (void)wall;
    (void)textureId;
}

// ========================================
// PORTAL CLIPPING FUNCTIONS
// ========================================

// Dessiner un mur avec clipping horizontal (pour portails)
// + Rendu du plafond et sol par secteur avec Y-buffer
void drawWallSliceClipped(t_render *render, t_wall *wall,
                          int clippedX1, int clippedX2,
                          float z1, float z2,
                          int origX1, int origX2,
                          float bottomHeight, float topHeight,
                          int textureId, int sectorId)
{
    if (clippedX1 > clippedX2)
        return;

    float heightRatio = (screenW * screenH) / 60.0f;
    float centerScreenH = screenH / 2;

    // Calcul des Y pour les extremités ORIGINALES
    float y1_top = (bottomHeight - topHeight - heightRatio) / z1;
    float y1_bot = (bottomHeight - heightRatio) / z1;
    float y2_top = (bottomHeight - topHeight - heightRatio) / z2;
    float y2_bot = (bottomHeight - heightRatio) / z2;

    float origWidth = (float)(origX2 - origX1);
    if (origWidth == 0)
        origWidth = 1;

    for (int x = clippedX1; x <= clippedX2; x++)
    {
        // t est calculé par rapport aux coordonnées ORIGINALES
        float t = (float)(x - origX1) / origWidth;

        // Interpolation de la profondeur
        float z = z1 + t * (z2 - z1);
        if (z < 0.1f)
            z = 0.1f;

        // Interpolation des hauteurs
        int yTop = (int)(centerScreenH + (y1_top + t * (y2_top - y1_top)));
        int yBot = (int)(centerScreenH + (y1_bot + t * (y2_bot - y1_bot)));

        // Clamp aux limites de l'écran
        if (yTop < 0)
            yTop = 0;
        if (yBot >= screenH)
            yBot = screenH - 1;

        // ═══════════════════════════════════════════════════════
        // RENDU PLAFOND (entre yBuffer.yTop[x] et yTop)
        // ═══════════════════════════════════════════════════════
        if (global.ybuffer.yTop[x] < yTop)
        {
            renderCeilingSlice(render, x, global.ybuffer.yTop[x], yTop, sectorId);
            global.ybuffer.yTop[x] = yTop;
        }

        // ═══════════════════════════════════════════════════════
        // RENDU MUR
        // ═══════════════════════════════════════════════════════
        int wallYTop = (yTop > global.ybuffer.yTop[x]) ? yTop : global.ybuffer.yTop[x];
        int wallYBot = (yBot < global.ybuffer.yBottom[x]) ? yBot : global.ybuffer.yBottom[x];

        for (int y = wallYTop; y <= wallYBot; y++)
        {
            color_t c = getColorBydistance(z);
            int color = (c.R << 16) | (c.G << 8) | c.B;
            putPixel(render, x, y, color);
        }

        // ═══════════════════════════════════════════════════════
        // RENDU SOL (entre yBot et yBuffer.yBottom[x])
        // ═══════════════════════════════════════════════════════
        if (yBot < global.ybuffer.yBottom[x])
        {
            renderFloorSlice(render, x, yBot + 1, global.ybuffer.yBottom[x], sectorId);
            global.ybuffer.yBottom[x] = yBot;
        }
    }

    (void)wall;
    (void)textureId;
}

// ========================================
// BUFFER D'OCCLUSION PAR COLONNE
// ========================================

void initOcclusionBuffer(void)
{
    for (int x = 0; x < screenW; x++)
        global.columnOccluded[x] = 0;
}

int isRangeFullyOccluded(int x1, int x2)
{
    if (x1 < 0) x1 = 0;
    if (x2 >= screenW) x2 = screenW - 1;
    if (x1 > x2) return 1;
    
    for (int x = x1; x <= x2; x++) {
        if (!global.columnOccluded[x])
            return 0;
    }
    return 1;
}

void markColumnsOccluded(int x1, int x2)
{
    if (x1 < 0) x1 = 0;
    if (x2 >= screenW) x2 = screenW - 1;
    for (int x = x1; x <= x2; x++)
        global.columnOccluded[x] = 1;
}

// ========================================
// RENDU RÉCURSIF PAR PORTAIL (VERSION AMÉLIORÉE)
// Utilise une pile explicite pour éviter le stack overflow
// et un buffer d'occlusion pour éviter les cycles et le sur-rendu
// ========================================

void renderSectorRecursive(t_render *render, int startSectorId,
                           t_render_window initialWindow, int unused)
{
    (void)unused;  // Garde pour compatibilité API
    
    t_render_entry stack[MAX_PORTAL_DEPTH * MAX_SECTORS];
    int stackTop = 0;
    
    // Initialiser le buffer d'occlusion
    initOcclusionBuffer();
    
    // Empiler le secteur de départ
    stack[stackTop].sectorId = startSectorId;
    stack[stackTop].clipLeft = initialWindow.xStart;
    stack[stackTop].clipRight = initialWindow.xEnd;
    stack[stackTop].depth = 0;
    stackTop++;
    
    while (stackTop > 0)
    {
        // Dépiler
        stackTop--;
        t_render_entry entry = stack[stackTop];
        
        // Vérifications de sécurité
        if (entry.sectorId < 0 || entry.sectorId >= global.sectorCount)
            continue;
        if (entry.depth >= MAX_PORTAL_DEPTH)
            continue;
        if (entry.clipLeft > entry.clipRight)
            continue;
        
        // Vérifier si la fenêtre est déjà totalement occultée
        if (isRangeFullyOccluded(entry.clipLeft, entry.clipRight))
            continue;
        
        t_sector *sector = &global.sectors[entry.sectorId];
        
        // Parcourir tous les murs du secteur
        for (int i = 0; i < sector->wallCount; i++)
        {
            t_wall *wall = &global.walls[sector->wallIds[i]];
            
            int screenX1, screenX2;
            float z1, z2;
            
            // Projeter le mur
            if (!projectWallToScreen(wall, &screenX1, &screenX2, &z1, &z2))
                continue;
            
            // Assurer l'ordre gauche → droite
            if (screenX1 > screenX2)
            {
                int tmpX = screenX1;
                screenX1 = screenX2;
                screenX2 = tmpX;
                float tmpZ = z1;
                z1 = z2;
                z2 = tmpZ;
            }
            
            // Clipping horizontal avec les bornes du portail parent
            if (screenX2 < entry.clipLeft || screenX1 > entry.clipRight)
                continue;
            
            int clippedX1 = (screenX1 < entry.clipLeft) ? entry.clipLeft : screenX1;
            int clippedX2 = (screenX2 > entry.clipRight) ? entry.clipRight : screenX2;
            
            // Vérifier si cette portion est déjà occultée
            if (isRangeFullyOccluded(clippedX1, clippedX2))
                continue;
            
            if (wall->isPortal && wall->backSectorId >= 0)
            {
                // === PORTAIL ===
                t_sector *backSector = &global.sectors[wall->backSectorId];
                
                // Dessiner les segments upper/lower si différence de hauteur
                if (sector->ceilingHeight > backSector->ceilingHeight)
                {
                    drawWallSliceClipped(render, wall, clippedX1, clippedX2,
                                         z1, z2, screenX1, screenX2,
                                         backSector->ceilingHeight,
                                         sector->ceilingHeight,
                                         wall->upperTextureId, entry.sectorId);
                }
                
                if (sector->floorHeight < backSector->floorHeight)
                {
                    drawWallSliceClipped(render, wall, clippedX1, clippedX2,
                                         z1, z2, screenX1, screenX2,
                                         sector->floorHeight,
                                         backSector->floorHeight,
                                         wall->lowerTextureId, entry.sectorId);
                }
                
                // Empiler le secteur adjacent pour rendu récursif
                if (stackTop < MAX_PORTAL_DEPTH * MAX_SECTORS - 1)
                {
                    stack[stackTop].sectorId = wall->backSectorId;
                    stack[stackTop].clipLeft = clippedX1;
                    stack[stackTop].clipRight = clippedX2;
                    stack[stackTop].depth = entry.depth + 1;
                    stackTop++;
                }
            }
            else
            {
                // === MUR SOLIDE ===
                drawWallSliceClipped(render, wall, clippedX1, clippedX2,
                                     z1, z2, screenX1, screenX2,
                                     sector->floorHeight, sector->ceilingHeight,
                                     wall->middleTextureId, entry.sectorId);
                
                // Marquer les colonnes comme occultées (mur opaque)
                markColumnsOccluded(clippedX1, clippedX2);
            }
        }
    }
}

void renderSectorSimple(t_render *render, int sectorId)
{
    t_sector *sector = &global.sectors[sectorId];

    for (int i = 0; i < sector->wallCount; i++)
    {
        t_wall *wall = &global.walls[sector->wallIds[i]];

        int screenX1, screenX2;
        float z1, z2;

        if (!projectWallToScreen(wall, &screenX1, &screenX2, &z1, &z2))
            continue;

        if (wall->isPortal && wall->backSectorId >= 0)
        {
            t_sector *backSector = &global.sectors[wall->backSectorId];

            if (sector->ceilingHeight > backSector->ceilingHeight)
            {
                drawWallSlice(render, wall, screenX1, screenX2, z1, z2,
                    backSector->ceilingHeight, sector->ceilingHeight,
                    wall->upperTextureId);
            }

            if (sector->floorHeight < backSector->floorHeight)
            {
                drawWallSlice(render, wall, screenX1, screenX2, z1, z2,
                    sector->floorHeight, backSector->floorHeight,
                    wall->lowerTextureId);
            }
        }
        else
        {
            drawWallSlice(render, wall, screenX1, screenX2, z1, z2,
                sector->floorHeight, sector->ceilingHeight,
                wall->middleTextureId);
        }
    }
}

void render_scene(t_render *render)
{
    if (global.currentSectorId < 0)
        global.currentSectorId = findSectorContainingPoint(global.cam.camPos);

    // ═══════════════════════════════════════════════════════
    // Initialiser le Y-buffer pour le rendu plafond/sol
    // ═══════════════════════════════════════════════════════
    initYBuffer();

    if (global.currentSectorId >= 0)
    {
        // Fenêtre initiale = tout l'écran
        t_render_window fullScreen;
        fullScreen.xStart = 0;
        fullScreen.xEnd = screenW - 1;

        // Rendu récursif avec pile explicite et buffer d'occlusion
        renderSectorRecursive(render, global.currentSectorId, fullScreen, 0);
    }

    // ═══════════════════════════════════════════════════════
    // Remplir les zones restantes du Y-buffer
    // (plafond skybox/texturé et sol pour les colonnes non couvertes)
    // ═══════════════════════════════════════════════════════
    for (int x = 0; x < screenW; x++)
    {
        // Plafond non rendu → fallback skybox ou secteur courant
        if (global.ybuffer.yTop[x] < screenH / 2)
        {
            renderCeilingSlice(render, x, 0, global.ybuffer.yTop[x], 
                              global.currentSectorId);
        }
        // Sol non rendu → fallback secteur courant
        if (global.ybuffer.yBottom[x] > screenH / 2)
        {
            renderFloorSlice(render, x, global.ybuffer.yBottom[x], screenH,
                            global.currentSectorId);
        }
    }
}

void handleOverlayToggle(void)
{
    // Toggle overlay avec la touche E (détection front montant)
    if (global.g_keys[PRESS_E] && !global.keyE_pressed) {
        global.showOverlay = !global.showOverlay;
        global.keyE_pressed = 1;
    }
    if (!global.g_keys[PRESS_E]) {
        global.keyE_pressed = 0;
    }
}

int game_loop(t_render *render)
{
    double dt = getDeltaTime();
    
    // Gestion du toggle overlay
    handleOverlayToggle();
    
    // Physique du joueur
    handleJump();              // Gestion du saut
    CameraTranslate(dt);       // Mouvement horizontal (X, Y)
    updatePlayerZ(dt);         // Physique verticale (gravité, step-up/down)
    
    // Nettoyer le depth buffer avant le rendu
    clearDepthBuffer();
    
    // ═══════════════════════════════════════════════════════
    // RENDU PRINCIPAL
    // Le plafond (skybox/texturé) et le sol sont maintenant
    // rendus par secteur via le Y-buffer dans render_scene()
    // ═══════════════════════════════════════════════════════
    render_scene(render);
    
    // Afficher overlay et minimap si activés
    if (global.showOverlay) {
        drawOverlay(render);
        drawMinimap(render);
    }
    
    updateScreen(render);
    display_debug_info(render);
    return (0);
}

void init()
{
    global.cam.camAngle = 1.57;  // Regarder vers le sud (vers le couloir)
    global.cam.camPos.x = 400.0;  // Centre de la petite pièce
    global.cam.camPos.y = 100.0;  // Dans la petite pièce
    global.cam.stepWave = 0.0;
    global.cam.camPitch = 0.0f;  // Pas d'inclinaison verticale
    global.cam.camZ = 0.0f;      // Hauteur de base
    global.currentSectorId = -1;
    
    // Initialisation de la hauteur du joueur
    global.cam.camZ = EYE_HEIGHT;      // Yeux à hauteur standard
    global.cam.footZ = 0.0f;           // Pieds au sol
    global.cam.velZ = 0.0f;            // Pas de vélocité verticale
    global.cam.targetZ = EYE_HEIGHT;   // Cible = position actuelle
    global.cam.onGround = 1;           // Au sol au départ
    
    // Overlay & Minimap (activé par défaut)
    global.showOverlay = 1;
    global.keyE_pressed = 0;

    // Réinitialiser tous les polygones
    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].vertCnt = 0;

    // ========================================
    // MAP: 3 SECTEURS + 4 COLONNES (style Doom)
    // 
    //         +----------+
    //         |  SECT 0  |  (Petite pièce - spawn)
    //         +----[]----+
    //              ||
    //         +----[]----+  (Couloir - Sect 1)
    //              ||
    //    +------------------+
    //    |  [C1]     [C2]   |  (Grande pièce - Sect 2)
    //    |                  |  + 4 colonnes (Sect 3-6)
    //    |  [C3]     [C4]   |
    //    +------------------+
    // ========================================

    // ========================================
    // SECTEUR 0 : Petite pièce de départ (SPAWN)
    // Taille: 150x150, hauteur normale
    // ========================================
    global.polys[0].vert[0].x = 350.00;
    global.polys[0].vert[0].y = 50.00;
    global.polys[0].vert[1].x = 450.00;
    global.polys[0].vert[1].y = 50.00;
    global.polys[0].vert[2].x = 450.00;
    global.polys[0].vert[2].y = 150.00;  // Mur Sud → Portal vers Couloir
    global.polys[0].vert[3].x = 350.00;
    global.polys[0].vert[3].y = 150.00;
    global.polys[0].height = 20000000;  // Hauteur normale
    global.polys[0].vertCnt = 4;

    // ========================================
    // SECTEUR 1 : Couloir étroit
    // Connecte la petite pièce à la grande salle
    // Plafond plus bas pour effet oppressant
    // ========================================
    global.polys[1].vert[0].x = 450.00;  // Portal avec Sect 0
    global.polys[1].vert[0].y = 150.00;
    global.polys[1].vert[1].x = 350.00;
    global.polys[1].vert[1].y = 150.00;
    global.polys[1].vert[2].x = 350.00;
    global.polys[1].vert[2].y = 300.00;  // Portal avec Sect 2
    global.polys[1].vert[3].x = 450.00;
    global.polys[1].vert[3].y = 300.00;
    global.polys[1].height = 12000000;  // Couloir bas, oppressant
    global.polys[1].vertCnt = 4;

    // ========================================
    // SECTEUR 2 : Grande salle avec colonnes
    // Très grande pièce, plafond haut
    // ========================================
    global.polys[2].vert[0].x = 350.00;  // Portal avec couloir
    global.polys[2].vert[0].y = 300.00;
    global.polys[2].vert[1].x = 450.00;
    global.polys[2].vert[1].y = 300.00;
    global.polys[2].vert[2].x = 650.00;
    global.polys[2].vert[2].y = 300.00;
    global.polys[2].vert[3].x = 650.00;
    global.polys[2].vert[3].y = 700.00;
    global.polys[2].vert[4].x = 150.00;
    global.polys[2].vert[4].y = 700.00;
    global.polys[2].vert[5].x = 150.00;
    global.polys[2].vert[5].y = 300.00;
    global.polys[2].height = 35000000;  // Grande salle haute
    global.polys[2].vertCnt = 6;

    // ========================================
    // COLONNES (Secteurs 3-6)
    // 4 colonnes carrées dans la grande salle
    // ========================================
    
    // Colonne 1 (haut-gauche)
    global.polys[3].vert[0].x = 250.00;
    global.polys[3].vert[0].y = 400.00;
    global.polys[3].vert[1].x = 290.00;
    global.polys[3].vert[1].y = 400.00;
    global.polys[3].vert[2].x = 290.00;
    global.polys[3].vert[2].y = 440.00;
    global.polys[3].vert[3].x = 250.00;
    global.polys[3].vert[3].y = 440.00;
    global.polys[3].height = 35000000;  // Même hauteur que la salle
    global.polys[3].vertCnt = 4;

    // Colonne 2 (haut-droite)
    global.polys[4].vert[0].x = 510.00;
    global.polys[4].vert[0].y = 400.00;
    global.polys[4].vert[1].x = 550.00;
    global.polys[4].vert[1].y = 400.00;
    global.polys[4].vert[2].x = 550.00;
    global.polys[4].vert[2].y = 440.00;
    global.polys[4].vert[3].x = 510.00;
    global.polys[4].vert[3].y = 440.00;
    global.polys[4].height = 35000000;
    global.polys[4].vertCnt = 4;

    // Colonne 3 (bas-gauche)
    global.polys[5].vert[0].x = 250.00;
    global.polys[5].vert[0].y = 560.00;
    global.polys[5].vert[1].x = 290.00;
    global.polys[5].vert[1].y = 560.00;
    global.polys[5].vert[2].x = 290.00;
    global.polys[5].vert[2].y = 600.00;
    global.polys[5].vert[3].x = 250.00;
    global.polys[5].vert[3].y = 600.00;
    global.polys[5].height = 35000000;
    global.polys[5].vertCnt = 4;

    // Colonne 4 (bas-droite)
    global.polys[6].vert[0].x = 510.00;
    global.polys[6].vert[0].y = 560.00;
    global.polys[6].vert[1].x = 550.00;
    global.polys[6].vert[1].y = 560.00;
    global.polys[6].vert[2].x = 550.00;
    global.polys[6].vert[2].y = 600.00;
    global.polys[6].vert[3].x = 510.00;
    global.polys[6].vert[3].y = 600.00;
    global.polys[6].height = 35000000;
    global.polys[6].vertCnt = 4;

    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].textureId = -1;
    
    // Couleurs des secteurs
    global.polys[0].color = 0x8B4513;  // Petite pièce - brun
    global.polys[1].color = 0x654321;  // Couloir - brun foncé
    global.polys[2].color = 0x4A4A4A;  // Grande salle - gris
    global.polys[3].color = 0x2F4F4F;  // Colonnes - gris ardoise
    global.polys[4].color = 0x2F4F4F;
    global.polys[5].color = 0x2F4F4F;
    global.polys[6].color = 0x2F4F4F;
    convertPolysToSectors();
    detectPortals();
    
    // ========================================
    // CONFIGURATION DES HAUTEURS DE SOL (Step-up/down)
    // Pour tester les escaliers et les plateformes
    // ========================================
    // Secteur 0 : Spawn room - hauteur de base
    global.sectors[0].floorHeight = 0.0f;
    
    // Secteur 1 : Couloir - légèrement surélevé (step-up de 16 unités)
    global.sectors[1].floorHeight = 16.0f;
    
    // Secteur 2 : Grande salle - encore plus haut (step-up de 20 unités)
    global.sectors[2].floorHeight = 36.0f;
    
    // Colonnes : même hauteur que la grande salle
    global.sectors[3].floorHeight = 36.0f;
    global.sectors[4].floorHeight = 36.0f;
    global.sectors[5].floorHeight = 36.0f;
    global.sectors[6].floorHeight = 36.0f;
}

int main()
{
    t_render render;

    init();
    memset(global.g_keys, 0, sizeof(global.g_keys));

    render.mlx = mlx_init();
    render.win = mlx_new_window(render.mlx, screenW, screenH, "doom-nukem");
    render.img = mlx_new_image(render.mlx, screenW, screenH);
    render.addr = mlx_get_data_addr(render.img, &render.bits_per_pixel, &render.line_len, &render.endian);


    global.skybox.loaded = 0;

    loadSkybox(&render, "../sprite_selection/skybox/CTYSKY01_1.xpm");
    initTextureManager(&render);
    assignTexturesToPolygons();

    mlx_hook(render.win, 2, 1L<<0, key_press, &render);
    mlx_hook(render.win, 3, 1L<<1, key_release, &render);
    mlx_hook(render.win, 17, 0, close_window, &render);
    
    mlx_loop_hook(render.mlx, game_loop, &render);
    
    mlx_loop(render.mlx);
    return (0);
}