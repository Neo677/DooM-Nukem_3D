
#include "./header/game.h"
#include "./header/types.h"
#include "./header/texture.h"
#include "./header/rasterize.h"
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

int createWall(Vec2_t p1 Vec2_t p2, int frontSectorId) 
{
    if (global.wallCount >= MAX_WALLS) {
        // printf("ERREUR: Trop de murs (max: %d)\n", MAX_WALLS);
        return (-1);
    }

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

    for (int i = 0; i < MAX_POLYS; i++) {
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

        for (int j = 0; j < global.polys[i].vertCnt; j++) {
            Vec2_t p1 = global.polys[i].vert[j];
            Vec2_t p2 = global.polys[i].vert[[j + 1] % global.polys[i].vertCnt];

            int wallId = createWall(p1, p2, global.sectorCount);
            if (wallId >= 0) {
                sector->wallIds[j] = wallId;
                global.walls[wallId].middleTextureId = global.polys.textureId;
            }
        }
        printf("Secteur %d cree: %d murs, plafond=%.1f\n", sector->id, sector->wallCount, sector->ceilingHeight);
        global.sectorCount++;
    }
    printf("=== Conversion terminee: %d secteurs, %d murs ===\n\n", global.sectorCount, global.wallCount);
}

int wallAreConnected(t_wall *w1, t_wall *w2) 
{
    float epsilon  = 1.0f;// https://fr.wikipedia.org/wiki/Epsilon
    float dist_p1_p2_1 = len(w1->p1, w2->p2);
    float dist_p2_p1_1 = len(w1->p2, w2->p1);
    return (dist_p1_p2_1 < epsilon && dist_p2_p1_1 < epsilon);
}

void detec_Portals()
{
    int portalCount = 0;

    for (int i = 0; i < global.wallCount; i++) {
        for (int j = 0; j < global.wallCount; j++) {
            if (wallAreConnected(&global.walls[i], &global.walls[j])) {
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
}

int pointInSector(t_sector *sector, Vec2_t point) 
{
    float vx[MAX_VERTS];
    float vy[MAX_VERTS];

    for (int i = 0; i < sector.wallCount; i++) {
        t_wall *wall = &global.walls[sector->wallIds[i]];
        vx[i] = wall->p1.x;
        vy[i] = wall->p1.y;
    }
    return (pointInPoly(sector->wallCount, vx, vy, point.x, point.y));
}

int findSectorContaining(Vec2_t point)
{
    for (int i = 0; i < global.sectorCount; i++) {
        if (pointInSector(&global.sectors[i], point)) {
            return (i);
        }
    }
    return (-1);
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

    // Trouver la texture du sol
    int floorTextureId = findTextureByName("floor");
    t_texture *floorTex = NULL;
    if (floorTextureId >= 0 && floorTextureId < global.tex_manager.count) {
        floorTex = &global.tex_manager.textures[floorTextureId];
    }

    for (int y = starty; y < screenH; y++) {
        float rowDistance = (screenH / 2.0f) / (y - screenH / 2.0f + 0.1f);
        
        for (int x = 0; x < screenW; x++) {
            int color;
            
            if (floorTex && floorTex->loaded) {
                float normalizedX = (x - screenW / 2.0f) / (screenW / 2.0f);
                float worldX = global.cam.camPos.x + cos(global.cam.camAngle) * rowDistance * 100 + sin(global.cam.camAngle) * normalizedX * rowDistance * 100;
                float worldY = global.cam.camPos.y + sin(global.cam.camAngle) * rowDistance * 100 - cos(global.cam.camAngle) * normalizedX * rowDistance * 100;                
                int texX = ((int)(worldX / 4) % floorTex->width);
                int texY = ((int)(worldY / 4) % floorTex->height);
                
                if (texX < 0) 
                    texX += floorTex->width;
                if (texY < 0)
                    texY += floorTex->height;
                
                color = getTexturePixel(floorTextureId, texX, texY);

                float darkness = 1.0f - (rowDistance / 20.0f);
                if (darkness < 0.3f) darkness = 0.3f;
                if (darkness > 1.0f) darkness = 1.0f;
                
                int r = ((color >> 16) & 0xFF) * darkness;
                int g = ((color >> 8) & 0xFF) * darkness;
                int b = (color & 0xFF) * darkness;
                color = (r << 16) | (g << 8) | b;
            } else {
                int intensity = y / 2;
                if (intensity > 255)
                    intensity = 255;
                color = (intensity << 16) | (intensity << 8) | intensity;
            }
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
    distX2 = distx2 * sin(global.cam.camAngle) - distY2 * cos(global.cam.camAngle);

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
    
    for (int x = startX; x <= endX; x++) {
        float t = (float)(x - screenX1) / (float)(screenX2 - screenX1 + 1);

        
    }
}

void render_scene(t_render *render)
{
    sortPolysByDeph();

    if (SHOULD_RASTERIZE == 1) {
        clearRasterBuffer();
        clearDepthBuffer();
        global.screenSpaceVisiblePlanes = 0;
    }

    for (int i = 0; i < MAX_POLYS; i++) {
        int polyIdx = global.polyOrder[i]; 
        
        if (global.polys[polyIdx].vertCnt < 2)
            continue;
        
        int planeIdx = global.screenSpaceVisiblePlanes;
        int segmentCount = 0;
        for (int i = 0; i < global.polys[polyIdx].vertCnt; i++) {
            Vec2_t p1 = global.polys[polyIdx].vert[i];
            Vec2_t p2 = global.polys[polyIdx].vert[(i + 1) % global.polys[polyIdx].vertCnt];
            float height = -global.polys[polyIdx].height / RES_DIV;

            float distX1 = p1.x - global.cam.camPos.x;
            float distY1 = p1.y - global.cam.camPos.y;
            float z1 = distX1 * cos(global.cam.camAngle) + distY1 * sin(global.cam.camAngle);

            float distX2 = p2.x - global.cam.camPos.x;
            float distY2 = p2.y - global.cam.camPos.y;
            float z2 = distX2 * cos(global.cam.camAngle) + distY2 * sin(global.cam.camAngle);

            distX1 = distX1 * sin(global.cam.camAngle) - distY1 * cos(global.cam.camAngle);
            distX2 = distX2 * sin(global.cam.camAngle) - distY2 * cos(global.cam.camAngle);

            if (z1 > 0 || z2 > 0)
            {
                Vec2_t i1 = intersection(distX1, z1, distX2, z2, -0.0001, 0.0001, -20, 5);
                Vec2_t i2 = intersection(distX1, z1, distX2, z2, 0.0001, 0.0001, 20, 5);
                if (z1 <= 0) {
                    if (i1.y > 0)
                    {
                        distX1 = i1.x;
                        z1 = i1.y;
                    } else {
                        distX1 = i2.x;
                        z1 = i2.y;
                    }
                }
                else if (z2 <= 0) 
                {
                    if (i1.y > 0) {
                        distX2 = i1.x;
                        z2 = i1.y;
                    } else {
                        distX2 = i2.x;
                        z2 = i2.y;
                    }
                }
            } else {
                continue;
            }

            float widthRatio = screenW / 2;
            float heightRatio = (screenW * screenH) / 60.0;
            float centerScreenH = screenH / 2;
            float centerScreenW = screenW / 2;

            float x1 = -distX1 * widthRatio / z1;
            float x2 = -distX2 * widthRatio / z2;
            float y1a = (height - heightRatio) / z1;
            float y1b = heightRatio / z1;
            float y2a = (height - heightRatio) / z2;
            float y2b = heightRatio / z2;

            // drawLine(render, centerScreenW + x1, centerScreenH + y1a, centerScreenW + x2, centerScreenH + y2a, global.polys[polyIdx].color);
            // drawLine(render, centerScreenW + x1, centerScreenH + y1b, centerScreenW + x2, centerScreenH + y2b, global.polys[polyIdx].color);
            // drawLine(render, centerScreenW + x1, centerScreenH + y1a, centerScreenW + x1, centerScreenH + y1b, global.polys[polyIdx].color);
            // drawLine(render, centerScreenW + x2, centerScreenH + y2a, centerScreenW + x2, centerScreenH + y2b, global.polys[polyIdx].color);
            
            if (SHOULD_RASTERIZE == 1 && planeIdx < MAX_POLYS && segmentCount < MAX_VERTS) {

                float avgDist = (z1 + z2) / 2.0;
                float v0x = centerScreenW + x2;
                float v0y = centerScreenH + y2a;
                float v1x = centerScreenW + x1;
                float v1y = centerScreenH + y1a;
                float v2x = centerScreenW + x1;
                float v2y = centerScreenH + y1b;
                float v3x = centerScreenW + x2;
                float v3y = centerScreenH + y2b;
                int allOutOfBounds = 1;
                if (v0x >= -screenW * 3 && v0x <= screenW * 3 && v0y >= -screenH * 3 && v0y <= screenH * 3)
                    allOutOfBounds = 0;
                if (v1x >= -screenW * 3 && v1x <= screenW * 3 && v1y >= -screenH * 3 && v1y <= screenH * 3)
                    allOutOfBounds = 0;
                if (v2x >= -screenW * 3 && v2x <= screenW * 3 && v2y >= -screenH * 3 && v2y <= screenH * 3)
                    allOutOfBounds = 0;
                if (v3x >= -screenW * 3 && v3x <= screenW * 3 && v3y >= -screenH * 3 && v3y <= screenH * 3)
                    allOutOfBounds = 0;
                
                int inBounds = !allOutOfBounds;
                
                if (avgDist > 0.01 && inBounds) {
                    global.screenSpacePoly[planeIdx][segmentCount].vert[0].x = v0x;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[0].y = v0y;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[1].x = v1x;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[1].y = v1y;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[2].x = v2x;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[2].y = v2y;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[3].x = v3x;
                    global.screenSpacePoly[planeIdx][segmentCount].vert[3].y = v3y;
                    global.screenSpacePoly[planeIdx][segmentCount].vertCnt = 4;
                    global.screenSpacePoly[planeIdx][segmentCount].distFromCamera = avgDist;
                    segmentCount++;
                }
            }
        }  
        if (SHOULD_RASTERIZE == 1 && segmentCount > 0) {
            global.screenSpaceVisiblePlanes++;
        }
    }
    
    if (SHOULD_RASTERIZE == 1)
        Rasterize(render);
}

int game_loop(t_render *render)
{
    double dt = getDeltaTime();
    CameraTranslate(dt);
    // clearScreen(render, 0x000000);
    renderSky(render);
    renderGround(render);
    render_scene(render);   
    updateScreen(render);
    display_debug_info(render);
    return (0);
}

void init()
{
    global.cam.camAngle = 0.42;
    global.cam.camPos.x = 451.96;
    global.cam.camPos.y = 209.24;
    global.cam.stepWave = 0.0;

    global.polys[0].vert[0].x = 141.00;
    global.polys[0].vert[0].y = 84.00;
    global.polys[0].vert[1].x = 496.00;
    global.polys[0].vert[1].y = 81.00;
    global.polys[0].vert[2].x = 553.00;
    global.polys[0].vert[2].y = 136.00;
    global.polys[0].vert[3].x = 135.00;
    global.polys[0].vert[3].y = 132.00;
    global.polys[0].height = 50000;
    global.polys[0].vertCnt = 4;
    global.polys[1].vert[0].x = 133.00;
    global.polys[1].vert[0].y = 441.00;
    global.polys[1].vert[1].x = 576.00;
    global.polys[1].vert[1].y = 438.00;
    global.polys[1].vert[2].x = 519.00;
    global.polys[1].vert[2].y = 493.00;
    global.polys[1].vert[3].x = 123.00;
    global.polys[1].vert[3].y = 497.00;
    global.polys[1].height = 50000;
    global.polys[1].vertCnt = 4;
    global.polys[2].vert[0].x = 691.00;
    global.polys[2].vert[0].y = 165.00;
    global.polys[2].vert[1].x = 736.00;
    global.polys[2].vert[1].y = 183.00;
    global.polys[2].vert[2].x = 737.00;
    global.polys[2].vert[2].y = 229.00;
    global.polys[2].vert[3].x = 697.00;
    global.polys[2].vert[3].y = 247.00;
    global.polys[2].vert[4].x = 656.00;
    global.polys[2].vert[4].y = 222.00;
    global.polys[2].vert[5].x = 653.00;
    global.polys[2].vert[5].y = 183.00;
    global.polys[2].height = 10000;
    global.polys[2].vertCnt = 6;
    global.polys[3].vert[0].x = 698.00;
    global.polys[3].vert[0].y = 330.00;
    global.polys[3].vert[1].x = 741.00;
    global.polys[3].vert[1].y = 350.00;
    global.polys[3].vert[2].x = 740.00;
    global.polys[3].vert[2].y = 392.00;
    global.polys[3].vert[3].x = 699.00;
    global.polys[3].vert[3].y = 414.00;
    global.polys[3].vert[4].x = 654.00;
    global.polys[3].vert[4].y = 384.00;
    global.polys[3].vert[5].x = 652.00;
    global.polys[3].vert[5].y = 348.00;
    global.polys[3].height = 10000;
    global.polys[3].vertCnt = 6;
    global.polys[4].vert[0].x = 419.00;
    global.polys[4].vert[0].y = 311.00;
    global.polys[4].vert[1].x = 461.00;
    global.polys[4].vert[1].y = 311.00;
    global.polys[4].vert[2].x = 404.00;
    global.polys[4].vert[2].y = 397.00;
    global.polys[4].vert[3].x = 346.00;
    global.polys[4].vert[3].y = 395.00;
    global.polys[4].vert[4].x = 348.00;
    global.polys[4].vert[4].y = 337.00;
    global.polys[4].height = 50000;
    global.polys[4].vertCnt = 5;
    global.polys[5].vert[0].x = 897.00;
    global.polys[5].vert[0].y = 98.00;
    global.polys[5].vert[1].x = 1079.00;
    global.polys[5].vert[1].y = 294.00;
    global.polys[5].vert[2].x = 1028.00;
    global.polys[5].vert[2].y = 297.00;
    global.polys[5].vert[3].x = 851.00;
    global.polys[5].vert[3].y = 96.00;
    global.polys[5].height = 10000;
    global.polys[5].vertCnt = 4;
    global.polys[6].vert[0].x = 1025.00;
    global.polys[6].vert[0].y = 294.00;
    global.polys[6].vert[1].x = 1080.00;
    global.polys[6].vert[1].y = 292.00;
    global.polys[6].vert[2].x = 1149.00;
    global.polys[6].vert[2].y = 485.00;
    global.polys[6].vert[3].x = 1072.00;
    global.polys[6].vert[3].y = 485.00;
    global.polys[6].height = 1000;
    global.polys[6].vertCnt = 4;
    global.polys[7].vert[0].x = 1070.00;
    global.polys[7].vert[0].y = 483.00;
    global.polys[7].vert[1].x = 1148.00;
    global.polys[7].vert[1].y = 484.00;
    global.polys[7].vert[2].x = 913.00;
    global.polys[7].vert[2].y = 717.00;
    global.polys[7].vert[3].x = 847.00;
    global.polys[7].vert[3].y = 718.00;
    global.polys[7].height = 1000;
    global.polys[7].vertCnt = 4;
    global.polys[8].vert[0].x = 690.00;
    global.polys[8].vert[0].y = 658.00;
    global.polys[8].vert[1].x = 807.00;
    global.polys[8].vert[1].y = 789.00;
    global.polys[8].vert[2].x = 564.00;
    global.polys[8].vert[2].y = 789.00;
    global.polys[8].height = 10000;
    global.polys[8].vertCnt = 3;
    global.polys[9].vert[0].x = 1306.00;
    global.polys[9].vert[0].y = 598.00;
    global.polys[9].vert[1].x = 1366.00;
    global.polys[9].vert[1].y = 624.00;
    global.polys[9].vert[2].x = 1369.00;
    global.polys[9].vert[2].y = 678.00;
    global.polys[9].vert[3].x = 1306.00;
    global.polys[9].vert[3].y = 713.00;
    global.polys[9].vert[4].x = 1245.00;
    global.polys[9].vert[4].y = 673.00;
    global.polys[9].vert[5].x = 1242.00;
    global.polys[9].vert[5].y = 623.00;
    global.polys[9].height = 50000;
    global.polys[9].vertCnt = 6;

    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].textureId = -1;
    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].color = 0xF54927;
    convertPolysToSectors();
    detectPortals();
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