#ifndef DEFINES_H
# define DEFINES_H

// ============ ENGINE CONSTANTS ============

// Rendering
#define Z_NEAR_PLANE 0.2
#define FOG_MAX_DISTANCE 20.0
#define FOG_MIN_INTENSITY 0.1

// Physics
#define MAX_STEP_HEIGHT 0.6
#define PLAYER_RADIUS 0.3
#define PLAYER_EYE_HEIGHT 0.5
#define GRAVITY_CONSTANT 0.002
#define GRAVITY_FRAME_SCALE 0.05

// Collision
#define COLLISION_EPSILON 1e-6
#define MAX_COLLISION_BUFFER 256

// Rendering limits
#define MAX_RECURSION_DEPTH 32
#define MAX_RENDER_DISTANCE 100.0

// Skybox
#define MAX_SKYBOX 8
#define SKYBOX_FACES 6

// Debug
#ifdef DEBUG_MODE
    #define DEBUG_LOG(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...) ((void)0)
#endif

#ifdef VERBOSE_MODE
    #define VERBOSE_LOG(fmt, ...) printf("[VERBOSE] " fmt, ##__VA_ARGS__)
#else
    #define VERBOSE_LOG(fmt, ...) ((void)0)
#endif

#endif
