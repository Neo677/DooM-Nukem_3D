
#include "./header/game.h"
#include "./header/types.h"
#include <sys/time.h>

Camera_t cam;
polygon_t polys[MAX_POLYS];
Vec2_t vert[MAX_VERTS];
static int g_keys[65536];
int screenSpaceVisiblePlanes;
screenSpacePoly_t screenSpacePoly[MAX_POLYS][MAX_VERTS];
static double lastTime = 0.0;
static t_skybox skybox;
static t_texture_manager tex_manager = {0};
static int polyOrder[MAX_POLYS];
static float polyDistances[MAX_POLYS];

t_texture_type detectTextureType(const char *filename)
{
    char lower[256];
    int i = 0;
    while (filename[i] && i < 255) {
        lower[i] = tolower(filename[i]);
        i++;
    }
    lower[i] = '\0';

    if (strstr(lower, "ceil") || strstr(lower, "ceiling"))
        return (TEXTURE_CEILING);
    if (strstr(lower, "floor") || strstr(lower, "flat") || strstr(lower, "mflr") || strstr(lower, "sflr"))
        return (TEXTURE_FLOOR);
    if (strstr(lower, "lava") || strstr(lower, "slime") || strstr(lower, "nukage") || 
        strstr(lower, "water") || strstr(lower, "fwater") || strstr(lower, "swater"))
        return (TEXTURE_LIQUID);
    if (strstr(lower, "rrock") || strstr(lower, "aqf") || strstr(lower, "cons") || 
        strstr(lower, "dem") || strstr(lower, "gate") || strstr(lower, "step"))
        return (TEXTURE_WALL);
    return (TEXTURE_WALL);
}

int loadTexture(t_render *render, const char *path, const char *name)
{
    if (tex_manager.count >= MAX_TEXTURES) {
        printf("Erreur: Trop de texture chargees (max: %d)\n", MAX_TEXTURES);
        return (-1);
    }
    
    int idx = tex_manager.count;
    t_texture *tex = &tex_manager.textures[idx];
    int width, height;

    tex->img = mlx_png_file_to_image(render->mlx, (char *)path, &width, &height);
    if (!tex->img) {
        // Essayer XPM si PNG échoue
        tex->img = mlx_xpm_file_to_image(render->mlx, (char *)path, &width, &height);
    }
    
    if (!tex->img) {
        printf("Erreur: Impossible de charger la texture: %s\n", path);
        fflush(stdout);
        printf("  Creation d'une texture procedurale a la place...\n");
        fflush(stdout);
        
        // Créer une texture procédurale de 64x64
        width = 64;
        height = 64;
        tex->img = mlx_new_image(render->mlx, width, height);
        if (!tex->img) {
            printf("  Erreur: Impossible de créer une image procédurale\n");
            return (-1);
        }
        
        // Remplir avec un motif procédural coloré et visible
        char *img_data = mlx_get_data_addr(tex->img, &tex->bits_per_pixel, &tex->line_len, &tex->endian);
        
        // Couleur différente selon le nom de la texture
        int base_color = 0xFFFFFF; // Blanc par défaut
        if (strstr(name, "wall")) base_color = 0x8B4513; // Brun (mur)
        else if (strstr(name, "floor")) base_color = 0x696969; // Gris (sol)
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Motif damier
                int color;
                if ((x / 8 + y / 8) % 2 == 0) {
                    color = base_color; // Couleur vive
                } else {
                    // Version plus sombre (diviser par 2)
                    int r = ((base_color >> 16) & 0xFF) / 2;
                    int g = ((base_color >> 8) & 0xFF) / 2;
                    int b = (base_color & 0xFF) / 2;
                    color = (r << 16) | (g << 8) | b;
                }
                
                int pixel_offset = y * tex->line_len + x * (tex->bits_per_pixel / 8);
                *(unsigned int *)(img_data + pixel_offset) = color;
            }
        }
    }

    tex->width = width;
    tex->height = height;
    tex->addr = mlx_get_data_addr(tex->img, &tex->bits_per_pixel, &tex->line_len, &tex->endian);
    tex->loaded = 1;
    tex->type = detectTextureType(path);
    strncpy(tex->name, name ? name : path, 63);
    tex->name[63] = '\0';
    tex_manager.count++;
    printf("Texture chargee [%d]: %s (%dx%d) - Type: %d\n", idx, tex->name, width, height, tex->type);
    fflush(stdout);
    return (idx);
}

int findTextureByName(const char *name)
{
    for (int i = 0; i < tex_manager.count; i++) {
        if (strstr(tex_manager.textures[i].name, name) != NULL) {
            return (i);
        }
    }
    return (-1);
}

int getTexturePixel(int texId, int x, int y)
{
    if (texId < 0 || texId >= tex_manager.count)
        return (0);
    t_texture *tex = &tex_manager.textures[texId];
    if (!tex->loaded || x < 0 || x >= tex->width || y < 0 || y >= tex->height)
        return (0);
    x = x % tex->width;
    y = y % tex->height;
    if (x < 0)
        x += tex->width;
    if (y < 0)
        y += tex->height;
    char *pixel = tex->addr + (y * tex->line_len + x * (tex->bits_per_pixel / 8));
    return (*(unsigned int *)pixel);
}

void initTextureManager(t_render *render)
{
    tex_manager.mlx = render->mlx;
    tex_manager.count = 0;

    // Charger la texture pour les murs (converties en RGB)
    loadTexture(render, "textures/wall.png", "wall");
    
    // Charger la texture pour le sol (converties en RGB)
    loadTexture(render, "textures/floor.png", "floor");
}



void putPixel(t_render *render, int x, int y, int color)
{
    if (x >= screenW || y >= screenH || x < 0 || y < 0)
        return;
    char *dst = render->addr + (y * render->line_len + x * (render->bits_per_pixel / 8));
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
        g_keys[keycode] = 1;
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
        g_keys[keycode] = 0;
    return (0);
}

void CameraTranslate(double deltaTime)
{
    int isMoving = 0;
    cam.oldCamPos = cam.camPos;
    Vec2_t newPos = cam.camPos;

    if (g_keys[W]) {
        newPos.x += MOV_SPEED * cos(cam.camAngle) * deltaTime;
        newPos.y += MOV_SPEED * sin(cam.camAngle) * deltaTime;
        isMoving = 1;
    } else if (g_keys[S]) {
        newPos.x -= MOV_SPEED * cos(cam.camAngle) * deltaTime;
        newPos.y -= MOV_SPEED * sin(cam.camAngle) * deltaTime;
        isMoving = 1;
    }

    cam.camPos = resolveCollision(newPos);

    if (g_keys[A]) {
        cam.camAngle -= ROT_SPEED * deltaTime;
    } else if (g_keys[D]) {
        cam.camAngle += ROT_SPEED * deltaTime;
    }

    if (isMoving)
        cam.stepWave += 3 * deltaTime;
    if (cam.stepWave > M_PI * 2)
        cam.stepWave = 0;
}

double getDeltaTime(void)
{
    struct timeval tv;
    double currentTime;
    double deltaTime;

    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec + tv.tv_usec / 1000000.0;
    
    if (lastTime == 0.0)
    {
        lastTime = currentTime;
        return 0.016;  // First frame fallback
    }
    
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    // Cap delta time to avoid huge jumps (10 FPS min) and zero/negative values
    if (deltaTime > 0.1 || deltaTime <= 0.0)
        deltaTime = 0.016;
    
    return deltaTime;
}

void display_debug_info(t_render *render)
{
    (void)render;
}

void init()
{
    cam.camAngle = 0.42;
    cam.camPos.x = 451.96;
    cam.camPos.y = 209.24;
    cam.stepWave = 0.0;
    
    polys[0].vert[0].x = 141.00;
    polys[0].vert[0].y = 84.00;
    polys[0].vert[1].x = 496.00;
    polys[0].vert[1].y = 81.00;
    polys[0].vert[2].x = 553.00;
    polys[0].vert[2].y = 136.00;
    polys[0].vert[3].x = 135.00;
    polys[0].vert[3].y = 132.00;
    polys[0].height = 50000;
    polys[0].vertCnt = 4;
    polys[1].vert[0].x = 133.00;
    polys[1].vert[0].y = 441.00;
    polys[1].vert[1].x = 576.00;
    polys[1].vert[1].y = 438.00;
    polys[1].vert[2].x = 519.00;
    polys[1].vert[2].y = 493.00;
    polys[1].vert[3].x = 123.00;
    polys[1].vert[3].y = 497.00;
    polys[1].height = 50000;
    polys[1].vertCnt = 4;
    polys[2].vert[0].x = 691.00;
    polys[2].vert[0].y = 165.00;
    polys[2].vert[1].x = 736.00;
    polys[2].vert[1].y = 183.00;
    polys[2].vert[2].x = 737.00;
    polys[2].vert[2].y = 229.00;
    polys[2].vert[3].x = 697.00;
    polys[2].vert[3].y = 247.00;
    polys[2].vert[4].x = 656.00;
    polys[2].vert[4].y = 222.00;
    polys[2].vert[5].x = 653.00;
    polys[2].vert[5].y = 183.00;
    polys[2].height = 10000;
    polys[2].vertCnt = 6;
    polys[3].vert[0].x = 698.00;
    polys[3].vert[0].y = 330.00;
    polys[3].vert[1].x = 741.00;
    polys[3].vert[1].y = 350.00;
    polys[3].vert[2].x = 740.00;
    polys[3].vert[2].y = 392.00;
    polys[3].vert[3].x = 699.00;
    polys[3].vert[3].y = 414.00;
    polys[3].vert[4].x = 654.00;
    polys[3].vert[4].y = 384.00;
    polys[3].vert[5].x = 652.00;
    polys[3].vert[5].y = 348.00;
    polys[3].height = 10000;
    polys[3].vertCnt = 6;
    polys[4].vert[0].x = 419.00;
    polys[4].vert[0].y = 311.00;
    polys[4].vert[1].x = 461.00;
    polys[4].vert[1].y = 311.00;
    polys[4].vert[2].x = 404.00;
    polys[4].vert[2].y = 397.00;
    polys[4].vert[3].x = 346.00;
    polys[4].vert[3].y = 395.00;
    polys[4].vert[4].x = 348.00;
    polys[4].vert[4].y = 337.00;
    polys[4].height = 50000;
    polys[4].vertCnt = 5;
    polys[5].vert[0].x = 897.00;
    polys[5].vert[0].y = 98.00;
    polys[5].vert[1].x = 1079.00;
    polys[5].vert[1].y = 294.00;
    polys[5].vert[2].x = 1028.00;
    polys[5].vert[2].y = 297.00;
    polys[5].vert[3].x = 851.00;
    polys[5].vert[3].y = 96.00;
    polys[5].height = 10000;
    polys[5].vertCnt = 4;
    polys[6].vert[0].x = 1025.00;
    polys[6].vert[0].y = 294.00;
    polys[6].vert[1].x = 1080.00;
    polys[6].vert[1].y = 292.00;
    polys[6].vert[2].x = 1149.00;
    polys[6].vert[2].y = 485.00;
    polys[6].vert[3].x = 1072.00;
    polys[6].vert[3].y = 485.00;
    polys[6].height = 1000;
    polys[6].vertCnt = 4;
    polys[7].vert[0].x = 1070.00;
    polys[7].vert[0].y = 483.00;
    polys[7].vert[1].x = 1148.00;
    polys[7].vert[1].y = 484.00;
    polys[7].vert[2].x = 913.00;
    polys[7].vert[2].y = 717.00;
    polys[7].vert[3].x = 847.00;
    polys[7].vert[3].y = 718.00;
    polys[7].height = 1000;
    polys[7].vertCnt = 4;
    polys[8].vert[0].x = 690.00;
    polys[8].vert[0].y = 658.00;
    polys[8].vert[1].x = 807.00;
    polys[8].vert[1].y = 789.00;
    polys[8].vert[2].x = 564.00;
    polys[8].vert[2].y = 789.00;
    polys[8].height = 10000;
    polys[8].vertCnt = 3;
    polys[9].vert[0].x = 1306.00;
    polys[9].vert[0].y = 598.00;
    polys[9].vert[1].x = 1366.00;
    polys[9].vert[1].y = 624.00;
    polys[9].vert[2].x = 1369.00;
    polys[9].vert[2].y = 678.00;
    polys[9].vert[3].x = 1306.00;
    polys[9].vert[3].y = 713.00;
    polys[9].vert[4].x = 1245.00;
    polys[9].vert[4].y = 673.00;
    polys[9].vert[5].x = 1242.00;
    polys[9].vert[5].y = 623.00;
    polys[9].height = 50000;
    polys[9].vertCnt = 6;

    // Initialiser tous les textureId à -1 (pas de texture par défaut)
    for (int i = 0; i < MAX_POLYS; i++) {
        polys[i].textureId = -1;
    }
    
    for (int i = 0; i < MAX_POLYS; i++)
        polys[i].color = 0xF54927;
}

void assignTexturesToPolygons(void)
{
    // Assigner la même texture (flat1_2) à tous les polygones
    int wallTextureId = findTextureByName("wall");
    
    for (int i = 0; i < MAX_POLYS; i++) {
        polys[i].textureId = wallTextureId;
    }
    
    printf("Textures assignees aux polygones (texture: %d)\n", wallTextureId);
    fflush(stdout);
}

int loadSkybox(t_render *render, const char *path)
{
    int width, height;

    skybox.img = mlx_xpm_file_to_image(render->mlx, (char *)path, &width, &height);

    if (!skybox.img) {
        printf("Erreur: Impossible de charger la skybox: %s\n", path);
        skybox.loaded = 0;
        return (0);
    }
    skybox.width = width;
    skybox.height = height;
    skybox.addr = mlx_get_data_addr(skybox.img, &skybox.bits_per_pixel, &skybox.line_len, &skybox.endian);
    skybox.loaded = 1;

    printf("Skybox chargee: %dx%d pixels\n", width, height);
    return (1);
}

int getSkyboxPixel(int x, int y) 
{
    if (!skybox.loaded || x < 0 || x >= skybox.width || y < 0 || y >= skybox.height)
        return (0);
    char *pixel = skybox.addr + (y * skybox.line_len + x * (skybox.bits_per_pixel / 8));
    return (*(unsigned int *)pixel);
}

float cross2dpoints(float x1, float y1, float x2, float y2)
{
    return (x1 * y2 - y1 * x2);
}

Vec2_t intersection(
        float x1, float y1, float x2, float y2,
        float x3, float y3, float x4, float y4)
{
    Vec2_t p;

    p.x = cross2dpoints(x1, y1, x2, y2);
    p.y = cross2dpoints(x3, y3, x4, y4);
    float det = cross2dpoints(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    p.x = cross2dpoints(p.x, x1 - x2, p.y, x3 - x4) / det;
    p.y = cross2dpoints(p.x, y1 - y2, p.y, y3 - y4) / det;

    return (p);
}

int pointInPoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
    int i, j, isPointInside = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        int isSameCoordn = 0;

        if ((verty[i] > testy) == (verty[j] > testy))
            isSameCoordn = 1;
        
        float denom = verty[j] - verty[i];
        if (isSameCoordn == 0 && denom != 0.0f)
        {
            if (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / denom + vertx[i])
                isPointInside = !isPointInside;
        }
    }
    return (isPointInside);
}

color_t getColorBydistance(float dist)
{
    if (dist <= 0.1)
        dist = 0.1;
    float pixelShader = 100.0 / dist;
    if (pixelShader > 1.0)
        pixelShader = 1.0;
    else if (pixelShader < 0.15)
        pixelShader = 0.15;
    
    color_t clr;
    clr.R = (unsigned char)(0xFF * pixelShader);
    clr.G = (unsigned char)(0x64 * pixelShader);
    clr.B = (unsigned char)(0x00 * pixelShader);

    return (clr);
}

void renderSky(t_render *render)
{
    int maxy = screenH / 2 + (int)(WWAVE_MAG * sinf(cam.stepWave));
    
    if (maxy < 0)
        maxy = 0;
    if (maxy > screenH)
        maxy = screenH;

    if (!skybox.loaded) {
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
            float angle = cam.camAngle + (normalizedX - 0.5f) * M_PI;

            float u = fmod(angle + M_PI, 2.0f * M_PI) / (2.0f * M_PI) * skybox.width;
            if (u < 0)
                u += skybox.width;
            
            float v = (float)y / maxy * skybox.height;
            if (v >= skybox.height) 
                v = skybox.height - 1;

            int color = getSkyboxPixel((int)u, (int)v);
            putPixel(render, x, y, color);
        }
    }

}

void renderGround(t_render *render)
{
    float waveVal = WWAVE_MAG * sinf(cam.stepWave);
    int starty = screenH / 2 + (int)waveVal;

    if (starty < 0)
        starty = 0;
    if (starty >= screenH)
        starty = screenH - 1;

    // Trouver la texture du sol
    int floorTextureId = findTextureByName("floor");
    t_texture *floorTex = NULL;
    if (floorTextureId >= 0 && floorTextureId < tex_manager.count) {
        floorTex = &tex_manager.textures[floorTextureId];
    }

    for (int y = starty; y < screenH; y++) {
        // Distance relative pour le mapping de perspective
        float rowDistance = (screenH / 2.0f) / (y - screenH / 2.0f + 0.1f);
        
        for (int x = 0; x < screenW; x++) {
            int color;
            
            if (floorTex && floorTex->loaded) {
                // Calculer la position du sol dans l'espace monde
                float normalizedX = (x - screenW / 2.0f) / (screenW / 2.0f);
                
                // Position dans l'espace monde avec perspective
                float worldX = cam.camPos.x + cos(cam.camAngle) * rowDistance * 100 + sin(cam.camAngle) * normalizedX * rowDistance * 100;
                float worldY = cam.camPos.y + sin(cam.camAngle) * rowDistance * 100 - cos(cam.camAngle) * normalizedX * rowDistance * 100;
                
                // Mapper sur la texture avec répétition
                int texX = ((int)(worldX / 4) % floorTex->width);
                int texY = ((int)(worldY / 4) % floorTex->height);
                
                if (texX < 0) texX += floorTex->width;
                if (texY < 0) texY += floorTex->height;
                
                color = getTexturePixel(floorTextureId, texX, texY);
                
                // Appliquer un shader de distance (plus loin = plus sombre)
                float darkness = 1.0f - (rowDistance / 20.0f);
                if (darkness < 0.3f) darkness = 0.3f;
                if (darkness > 1.0f) darkness = 1.0f;
                
                int r = ((color >> 16) & 0xFF) * darkness;
                int g = ((color >> 8) & 0xFF) * darkness;
                int b = (color & 0xFF) * darkness;
                color = (r << 16) | (g << 8) | b;
            } else {
                // Fallback : gradient gris
                int intensity = y / 2;
                if (intensity > 255)
                    intensity = 255;
                color = (intensity << 16) | (intensity << 8) | intensity;
            }
            
            putPixel(render, x, y, color);
        }
    }
}

static float depthBuff[screenH][screenW];

void clearDepthBuffer()
{
    for (int y = 0; y < screenH; y++)
        for (int x = 0; x < screenW; x++)
            depthBuff[y][x] = 999999.0f;
}

void Rasterize(t_render *render)
{
    float vx[MAX_VERTS];
    float vy[MAX_VERTS];

    for (int polyIdx = 0; polyIdx < screenSpaceVisiblePlanes; polyIdx++) {
        
        for (int segIdx = 0; segIdx < MAX_VERTS; segIdx++) {
            int vertCnt = screenSpacePoly[polyIdx][segIdx].vertCnt;
            
            if (vertCnt == 0)
                continue;
            float dist = screenSpacePoly[polyIdx][segIdx].distFromCamera;
            if (dist <= 0)
                continue;
            
            // color_t c = getColorBydistance(dist);
            // int color = (c.R << 16) | (c.G << 8) | c.B;

            int textureId = -1;
            if (polyIdx < MAX_POLYS) 
                textureId = polys[polyOrder[polyIdx]].textureId;
            
            float minX = screenW, maxX = 0;
            float minY = screenH, maxY = 0;

            for (int nextv = 0; nextv < vertCnt; nextv++) {
                vx[nextv] = screenSpacePoly[polyIdx][segIdx].vert[nextv].x;
                vy[nextv] = screenSpacePoly[polyIdx][segIdx].vert[nextv].y;
                
                if (vx[nextv] < minX)
                    minX = vx[nextv];
                if (vx[nextv] > maxX)
                    maxX = vx[nextv];
                if (vy[nextv] < minY)
                    minY = vy[nextv];
                if (vy[nextv] > maxY)
                    maxY = vy[nextv];
            }

            if (minX < 0)
                minX = 0;
            if (maxX >= screenW)
                maxX = screenW - 1;
            if (minY < 0)
                minY = 0;
            if (maxY >= screenH)
                maxY = screenH - 1;

            if (minX >= screenW || maxX < 0 || minY >= screenH || maxY < 0)
                continue;
            if (maxX < minX || maxY < minY)
                continue;


            t_texture *tex = NULL;
            if (textureId >= 0 && textureId < tex_manager.count) {
                tex = &tex_manager.textures[textureId];
            }
            
            // Debug : afficher la première fois qu'une texture est utilisée
            static int debug_once = 0;
            if (tex && tex->loaded && debug_once == 0) {
                printf("DEBUG: Utilisation texture [%d]: %s (%dx%d)\n", textureId, tex->name, tex->width, tex->height);
                fflush(stdout);
                debug_once = 1;
            }

            for (int y = (int)minY; y <= (int)maxY; y++) {
                for (int x = (int)minX; x <= (int)maxX; x++) {
                    if (dist < depthBuff[y][x]) {
                        if (pointInPoly(vertCnt, vx, vy, x, y) == 1) {
                            int color;

                            if (tex && tex->loaded) {
                                float u = ((float)(x - minX) / (maxX - minX + 1)) * tex->width;
                                float v = ((float)(y - minY) / (maxY - minY + 1)) * tex->height;
                                float scale = dist / 50.0f;

                                u = fmod(u * scale, tex->width);
                                v = fmod(v * scale, tex->height);

                                color = getTexturePixel(textureId, (int)u, (int)v);
                                color_t c = getColorBydistance(dist);
                                int r = ((color >> 16) & 0xFF) * c.R / 255;
                                int g = ((color >> 8) & 0xFF) * c.G / 255;
                                int b = (color & 0xFF) * c.B / 255;
                                color = (r << 16) | (g << 8) | b;
                            } else {
                                color_t c = getColorBydistance(dist);
                                color = (c.R << 16) | (c.G << 8) | c.B;
                            }
                            putPixel(render, x, y, color);
                            depthBuff[y][x] = dist;
                        }
                    }
                }
            }
        }
    }
}

void clearRasterBuffer()
{
    memset(screenSpacePoly, 0, sizeof(screenSpacePoly));
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
        polyOrder[i] = i;
    for (int i = 0; i < MAX_POLYS; i++) {
        if (polys[i].vertCnt < 2) {
            polyDistances[i] = -1.0f;
            continue;
        }
        

        float centerX = 0, centerY = 0;
        for (int j = 0; j < polys[i].vertCnt; j++) {
            centerX += polys[i].vert[j].x;
            centerY += polys[i].vert[j].y;
        }
        centerX /= polys[i].vertCnt;
        centerY /= polys[i].vertCnt;
        

        float dx = centerX - cam.camPos.x;
        float dy = centerY - cam.camPos.y;
        polyDistances[i] = sqrt(dx * dx + dy * dy);
        polys[i].curDist = polyDistances[i];
    }
    
    for (int i = 0; i < MAX_POLYS - 1; i++) {
        for (int j = i + 1; j < MAX_POLYS; j++) {
            if (polyDistances[polyOrder[i]] < polyDistances[polyOrder[j]]) {
                int tmp = polyOrder[i];
                polyOrder[i] = polyOrder[j];
                polyOrder[j] = tmp;
            }
        }
    }
}

void render_scene(t_render *render)
{
    sortPolysByDeph();

    if (Should_Rasterize == 1) {
        clearRasterBuffer();
        clearDepthBuffer();
        screenSpaceVisiblePlanes = 0;
    }

    for (int i = 0; i < MAX_POLYS; i++) {
        int polyIdx = polyOrder[i]; 
        
        if (polys[polyIdx].vertCnt < 2)
            continue;
        
        int planeIdx = screenSpaceVisiblePlanes;
        int segmentCount = 0;
        
        for (int i = 0; i < polys[polyIdx].vertCnt; i++) {
            Vec2_t p1 = polys[polyIdx].vert[i];
            Vec2_t p2 = polys[polyIdx].vert[(i + 1) % polys[polyIdx].vertCnt];

            float height = -polys[polyIdx].height / RES_DIV;

            float distX1 = p1.x - cam.camPos.x;
            float distY1 = p1.y - cam.camPos.y;
            float z1 = distX1 * cos(cam.camAngle) + distY1 * sin(cam.camAngle);

            float distX2 = p2.x - cam.camPos.x;
            float distY2 = p2.y - cam.camPos.y;
            float z2 = distX2 * cos(cam.camAngle) + distY2 * sin(cam.camAngle);

            distX1 = distX1 * sin(cam.camAngle) - distY1 * cos(cam.camAngle);
            distX2 = distX2 * sin(cam.camAngle) - distY2 * cos(cam.camAngle);

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

            // drawLine(render, centerScreenW + x1, centerScreenH + y1a, centerScreenW + x2, centerScreenH + y2a, polys[polyIdx].color);
            // drawLine(render, centerScreenW + x1, centerScreenH + y1b, centerScreenW + x2, centerScreenH + y2b, polys[polyIdx].color);
            // drawLine(render, centerScreenW + x1, centerScreenH + y1a, centerScreenW + x1, centerScreenH + y1b, polys[polyIdx].color);
            // drawLine(render, centerScreenW + x2, centerScreenH + y2a, centerScreenW + x2, centerScreenH + y2b, polys[polyIdx].color);
            
            if (Should_Rasterize == 1 && planeIdx < MAX_POLYS && segmentCount < MAX_VERTS) {

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
                
                if (avgDist > 0.01 && inBounds)
                {
                    screenSpacePoly[planeIdx][segmentCount].vert[0].x = v0x;
                    screenSpacePoly[planeIdx][segmentCount].vert[0].y = v0y;
                    screenSpacePoly[planeIdx][segmentCount].vert[1].x = v1x;
                    screenSpacePoly[planeIdx][segmentCount].vert[1].y = v1y;
                    screenSpacePoly[planeIdx][segmentCount].vert[2].x = v2x;
                    screenSpacePoly[planeIdx][segmentCount].vert[2].y = v2y;
                    screenSpacePoly[planeIdx][segmentCount].vert[3].x = v3x;
                    screenSpacePoly[planeIdx][segmentCount].vert[3].y = v3y;
                    screenSpacePoly[planeIdx][segmentCount].vertCnt = 4;
                    screenSpacePoly[planeIdx][segmentCount].distFromCamera = avgDist;
                    segmentCount++;
                }
            }
        }
        
        if (Should_Rasterize == 1 && segmentCount > 0) {
            screenSpaceVisiblePlanes++;
        }
    }
    
    if (Should_Rasterize == 1)
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

int main()
{
    t_render render;

    init();
    memset(g_keys, 0, sizeof(g_keys));

    render.mlx = mlx_init();
    render.win = mlx_new_window(render.mlx, screenW, screenH, "doom-nukem");
    render.img = mlx_new_image(render.mlx, screenW, screenH);
    render.addr = mlx_get_data_addr(render.img, &render.bits_per_pixel, &render.line_len, &render.endian);


    skybox.loaded = 0;

    loadSkybox(&render, "../sprite_selection/skybox/CTYSKY01_1.xpm");
    initTextureManager(&render);
    assignTexturesToPolygons();  // Assigner les textures APRÈS avoir chargé le gestionnaire

    mlx_hook(render.win, 2, 1L<<0, key_press, &render);
    mlx_hook(render.win, 3, 1L<<1, key_release, &render);
    mlx_hook(render.win, 17, 0, close_window, &render);
    
    mlx_loop_hook(render.mlx, game_loop, &render);
    
    mlx_loop(render.mlx);
    return (0);
}