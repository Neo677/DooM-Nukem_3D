
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

// // ----------------- MATH -----------
// float dotPoints(float x1, float y1, float x2, float y2)
// {
//     return (x1 * x2 + y1 * y2);
// }

// float ft_dot(Vec2_t pointA, Vec2_t pointB)
// {
//     return (dotPoints(pointA.x, pointA.y, pointB.x, pointB.y));
// }

// Vec2_t normalize(Vec2_t vec)
// {
//     float len = sqrt((vec.x * vec.x) + (vec.y * vec.y));
//     Vec2_t normalize;
//     normalize.x = vec.x / len;
//     normalize.y = vec.y / len;

//     return (normalize);
// }

// Vec2_t vecMinus(Vec2_t v1, Vec2_t v2)
// {
//     Vec2_t v3;
//     v3.x = v1.x - v2.x;
//     v3.y = v1.y - v2.y;

//     return (v3);
// }

// Vec2_t vecPlus(Vec2_t v1, Vec2_t v2)
// {
//     Vec2_t v3;
//     v3.x = v1.x + v2.x;
//     v3.y = v1.y + v2.y;

//     return (v3);
// }

// Vec2_t vecMulF(Vec2_t v1, float val)
// {
//     Vec2_t v2;
//     v2.x = v1.x * val;
//     v2.y = v1.y * val;
//     return (v2);
// }

// float len_vec(Vec2_t pointA, Vec2_t pointB)
// {
//     float distX = pointB.x - pointA.x;
//     float distY = pointB.y - pointA.y;

//     return (sqrt(distX * distX + distY * distY));
// }

// Vec2_t closestPointOnLine(lineSeg_t line, Vec2_t point)
// {
//     float lineLen = len_vec(line.p1, line.p2);
//     float dot = (((point.x - line.p1.x) * (line.p2.x - line.p1.x)) + ((point.y - line.p1.y) * (line.p2.y - line.p1.y))) / (lineLen * lineLen);

//     if (dot > 1) {
//         dot = 1;
//     } else if (dot < 0) {
//         dot = 0;
//     }
//     Vec2_t closestPoint;
//     closestPoint.x = line.p1.x + (dot * (line.p2.x - line.p1.x));
//     closestPoint.y = line.p1.y + (dot * (line.p2.y - line.p1.y));
//     return (closestPoint);
// }

// int isPointOnLine(lineSeg_t line, Vec2_t point)
// {
//     float lineLen = len_vec(line.p1, line.p2);
//     float pointDist1 = len_vec(point, line.p1);
//     float pointDist2 = len_vec(point, line.p2);
//     float resolution = 0.1f;
//     float lineLenMarginHigh = lineLen + resolution;
//     float lineLenMarginLow = lineLen - resolution;
//     float distFromLineEnds = pointDist1 + pointDist2;

//     if (distFromLineEnds >= lineLenMarginLow && distFromLineEnds <= lineLenMarginHigh)
//         return (1);
//     return (0);
// }

// int lineCircleCollision(lineSeg_t line, Vec2_t circleCenter, float circleRadius)
// {
//     Vec2_t closestPointToLine = closestPointOnLine(line, circleCenter);
//     float circleToPointOnLineDist = len_vec(closestPointToLine, circleCenter);

//     if (circleToPointOnLineDist < circleRadius)
//         return (1);
//     return (0);
// }

// Vec2_t resolveCollision(Vec2_t newPos)
// {
//     float RADIUS = 20.0f;
//     int iterations = 0;
//     int maxIterations = 5;
    
//     while (iterations < maxIterations)
//     {
//         int collisionDetected = 0;
//         Vec2_t totalPush = {0, 0};
        
//         for (int polyIdx = 0; polyIdx < MAX_POLYS; polyIdx++)
//         {
//             if (polys[polyIdx].vertCnt < 2)
//                 continue;
//             // Test tous les segments du polygone
//             for (int i = 0; i < polys[polyIdx].vertCnt; i++)
//             {
//                 lineSeg_t wall;
//                 wall.p1 = polys[polyIdx].vert[i];
//                 wall.p2 = polys[polyIdx].vert[(i + 1) % polys[polyIdx].vertCnt];
                
//                 if (lineCircleCollision(wall, newPos, RADIUS))
//                 {
//                     collisionDetected = 1;
//                     Vec2_t closestPoint = closestPointOnLine(wall, newPos);
//                     Vec2_t pushDir = vecMinus(newPos, closestPoint);
                    
//                     float dist = sqrt(pushDir.x * pushDir.x + pushDir.y * pushDir.y);
                    
//                     if (dist > 0.001f)
//                     {
//                         pushDir.x /= dist;
//                         pushDir.y /= dist;
                        
//                         float pushAmount = RADIUS - dist + 0.5f;
//                         totalPush.x += pushDir.x * pushAmount;
//                         totalPush.y += pushDir.y * pushAmount;
//                     }
//                     else
//                     {
//                         totalPush.x += RADIUS + 0.5f;
//                         totalPush.y += 0.0f;
//                     }
//                 }
//             }
//         }
        
//         if (!collisionDetected)
//             break;
        
//         newPos.x += totalPush.x;
//         newPos.y += totalPush.y;
        
//         iterations++;
//     }
    
//     return newPos;
// }

// int checkCollision(Vec2_t newPos)
// {
//     for (int polyIdx = 0; polyIdx < MAX_POLYS; polyIdx++) {
//         if (polys[polyIdx].vertCnt < 2) 
//             continue;
//         for (int i = 0; i < polys[polyIdx].vertCnt; i++) {
//             lineSeg_t wall;
//             wall.p1 = polys[polyIdx].vert[i];
//             wall.p2 = polys[polyIdx].vert[(i + 1) % polys[polyIdx].vertCnt];

//             if (lineCircleCollision(wall, newPos, 20.0f))
//                 return (1);
//         }
//     }
//     return (0);
// }


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
    if (keycode == KEY_ESC) {
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

    if (g_keys[KEY_W]) {
        newPos.x += MOV_SPEED * cos(cam.camAngle) * deltaTime;
        newPos.y += MOV_SPEED * sin(cam.camAngle) * deltaTime;
        isMoving = 1;
    } else if (g_keys[KEY_S]) {
        newPos.x -= MOV_SPEED * cos(cam.camAngle) * deltaTime;
        newPos.y -= MOV_SPEED * sin(cam.camAngle) * deltaTime;
        isMoving = 1;
    }

    cam.camPos = resolveCollision(newPos);

    if (g_keys[KEY_A]) {
        cam.camAngle -= ROT_SPEED * deltaTime;
    } else if (g_keys[KEY_D]) {
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

    for (int i = 0; i < MAX_POLYS; i++)
        polys[i].color = 0xF54927;
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
        pixelShader = 0.15;  // Minimum brightness to prevent pure black
    
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

    int skyColor = (77 << 16) | (181 << 8) | 255;  // 0x4DB5FF
    for (int y = 0; y < maxy; y++) {
        for (int x = 0; x < screenW; x++) {
            putPixel(render, x, y, skyColor);
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

    for (int y = starty; y < screenH; y++) {
        int intensity = y / 2;
        if (intensity > 255)
            intensity = 255;
        int groundColor = (intensity << 16) | (intensity << 8) | intensity;

        for (int x = 0; x < screenW; x++) {
            putPixel(render, x, y, groundColor);
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

    // Iterate forward - z-buffer handles depth sorting
    for (int polyIdx = 0; polyIdx < screenSpaceVisiblePlanes; polyIdx++) {
        
        for (int segIdx = 0; segIdx < MAX_VERTS; segIdx++) {
            int vertCnt = screenSpacePoly[polyIdx][segIdx].vertCnt;
            
            if (vertCnt == 0)
                continue;
            float dist = screenSpacePoly[polyIdx][segIdx].distFromCamera;
            if (dist <= 0)
                continue;
            
            color_t c = getColorBydistance(dist);
            int color = (c.R << 16) | (c.G << 8) | c.B;
            
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
            
            // Skip if bounding box is completely off screen
            if (minX >= screenW || maxX < 0 || minY >= screenH || maxY < 0)
                continue;
            if (maxX < minX || maxY < minY)
                continue;

            for (int y = (int)minY; y <= (int)maxY; y++) {
                for (int x = (int)minX; x <= (int)maxX; x++) {
                    if (dist < depthBuff[y][x]) {
                        if (pointInPoly(vertCnt, vx, vy, x, y) == 1) {
                            putPixel(render, x, y, color);
                            depthBuff[y][x] = dist;  // Stocke la distance
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
    for (int i = 0; i < MAX_POLYS; i++) {
        for (int j = 0; j < MAX_POLYS - i - 1; j++) {
            polygon_t poly1 = polys[j];
            polygon_t poly2 = polys[j + 1];

            float distP1 = closestVertexInPoly(poly1, cam.camPos);
            float distP2 = closestVertexInPoly(poly2, cam.camPos);

            polys[j].curDist = distP1;
            polys[j + 1].curDist = distP2;

            if (distP1 < distP2) {
                polygon_t tmp = polys[j + 1];
                polys[j + 1] = polys[j];
                polys[j] = tmp;
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

    for (int polyIdx = 0; polyIdx < MAX_POLYS; polyIdx++) {
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
                } if (z2 <= 0)
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

    mlx_hook(render.win, 2, 1L<<0, key_press, &render);
    mlx_hook(render.win, 3, 1L<<1, key_release, &render);
    mlx_hook(render.win, 17, 0, close_window, &render);
    
    mlx_loop_hook(render.mlx, game_loop, &render);
    
    mlx_loop(render.mlx);
    return (0);
}