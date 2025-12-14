#ifndef GAME_H
# define GAME_H

# include <math.h>
# include <stdio.h>
# include <memory.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include "types.h"
# include "collision_vector.h"

#ifdef OS_MAC
#include "../minilibx_opengl_20191021/mlx.h"
#else
#include "../minilibx-linux/mlx.h"
#include "../minilibx-linux/mlx_int.h"
#endif

# define SENSI 0.0005f
# define MOV_SPEED 300
# define ROT_SPEED 5
# define RES_DIV 3
# define CAM_RADIUS 20.0f;
# define POL_RES 1.025
# define SHOULD_RASTERIZE 1
# define WWAVE_MAG 1
# define SLY_COLOR 0x4DB5FF

# define GRAVITY          800.0f
# define JUMP_VELOCITY    350.0f
# define MAX_STEP_HEIGHT  24.0f
# define STEP_SMOOTH_SPEED 150.0f
# define MAX_FALL_VELOCITY 1000.0f

# define screenW 800
# define screenH 600

typedef struct s_render {
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_len;
    int endian;
}     t_render;

#include "key.h"

typedef struct s_texture_manager {
    t_texture textures[MAX_TEXTURES];
    int count;
    void *mlx;
}       t_texture_manager;

#include "key.h"

typedef struct s_global {
    Camera_t cam;
    polygon_t polys[MAX_POLYS];
    Vec2_t vert[MAX_VERTS];
    int g_keys[65536];
    int mouseInit;
    int lastMouseX;
    int lastMouseY;
    int screenSpaceVisiblePlanes;
    screenSpacePoly_t screenSpacePoly[MAX_POLYS][MAX_VERTS];
    double lastTime;
    t_skybox skybox;
    t_texture_manager tex_manager;
    int polyOrder[MAX_POLYS];
    float polyDistances[MAX_POLYS];

    t_sector sectors[MAX_SECTORS];
    t_wall walls[MAX_WALLS];
    int sectorCount;
    int wallCount;
    int currentSectorId;
    
    int showOverlay;
    int keyE_pressed;
    int keyV_pressed;
    int flyMode;
    
    int columnOccluded[screenW];
    
    t_ybuffer ybuffer;
    
    t_debug_overlay debugOverlay;
}           t_global;

extern t_global global;

// draw_utils.c
void updateScreen(t_render *render);
void clearScreen(t_render *render, int color);
void drawLine(t_render *render, int x0, int y0, int x1, int y1, int color);
void drawRect(t_render *render, int x, int y, int w, int h, int color);
void drawRectOutline(t_render *render, int x, int y, int w, int h, int color);

// ui_overlay.c
void drawOverlayBackground(t_render *render);
void drawOverlayText(t_render *render);
void drawMinimapBackground(t_render *render);
void drawMinimapText(t_render *render);
void handleOverlayToggle(void);
void overlay_begin_frame(void);
void overlay_handle_keys(void);
void overlay_draw(t_render *render);

// input_handler.c
int key_hook(int keycode, t_render *render);
int close_window(int x, int y, t_render *render);
int key_press(int keycode, t_render *render);
int key_release(int keycode, t_render *render);
int mouse_move(int x, int y, t_render *render);

// player_physics.c
void CameraTranslate(double deltaTime);
void handleJump(void);
void updatePlayerZ(double deltaTime);

// map_manager.c
int createWall(Vec2_t p1, Vec2_t p2, int frontSectorId);
void convertPolysToSectors(void);
int wallsAreConnected(t_wall *w1, t_wall *w2);
void detectPortals(void);
void sortPolysByDeph(void);

// skybox.c
int loadSkybox(t_render *render, const char *path);
int getSkyboxPixel(int x, int y);
void renderSky(t_render *render);

// math_utils.c
float cross2dpoints(float x1, float y1, float x2, float y2);
Vec2_t intersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
float len(Vec2_t pointA, Vec2_t pointB);
float closestVertexInPoly(polygon_t poly, Vec2_t pos);

// render_engine.c
void renderGround(t_render *render);
int clipWallToNearPlane(Vec2_t *p1, Vec2_t *p2, float *z1, float *z2, float *u1, float *u2);
int projectWallToScreen(t_wall *wall, int *screenX1, int *screenX2, float *z1_out, float *z2_out, float *u1_out, float *u2_out);
Vec2_t calculateWallHitPoint(t_wall *wall, int screenX, float z);
float calculateWallU(t_wall *wall, Vec2_t hitPoint);
int getWallOrientation(t_wall *wall);
void drawWallSlice(t_render *render, t_wall *wall, int screenX1, int screenX2, float z1, float z2, float bottomHeight, float topHeight, int textureId);
void drawWallSliceClipped(t_render *render, t_wall *wall, int clippedX1, int clippedX2, float z1, float z2, int origX1, int origX2, float bottomHeight, float topHeight, int textureId, int sectorId, int flags, float u1, float u2);
void initOcclusionBuffer(void);
int isRangeFullyOccluded(int x1, int x2);
void markColumnsOccluded(int x1, int x2);
void renderSectorRecursive(t_render *render, int startSectorId, t_render_window initialWindow, int unused);
void renderSectorSimple(t_render *render, int sectorId);
void render_scene(t_render *render);

// game_loop.c
double getDeltaTime(void);
void display_debug_info(t_render *render);
int game_loop(t_render *render);

int findTextureByName(const char *name);

#endif