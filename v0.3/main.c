#include "./header/global.h"
#include "./header/maths.h"
#include "./header/render.h"
#include "./header/map.h"
#include "./header/collision.h"
#include "./header/debug_log.h"
#include <sys/time.h>

// Get current time in seconds (with microsecond precision)
f64 getTime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (f64)tv.tv_sec + (f64)tv.tv_usec / 1000000.0;
}

int closeWindow(t_engine *engine) {
    engine->isRunning = false;
    debug_log_close(&engine->debugLog);
    if (engine->render.mlx)
        mlx_loop_end(engine->render.mlx);
    return (0);
}

void updatePlayerSector(t_engine *engine) {
    #define QUEUE_MAX 64
    int queue[QUEUE_MAX];
    int qh = 0, qt = 0;
    int found = SECTOR_NONE;
    queue[qt++] = engine->camera.sector;

    while (qh != qt) {
        int id = queue[qh++];
        const t_sector * sector = &engine->sectors[id];

        if (pointInSector(engine, sector, engine->camera.pos)) {
            found = id;
            break;
        }
        for (usize i = 0; i < sector->nWalls; i++) {
            const t_wall *wall = &engine->walls[sector->firstWall + i];
            if (wall->portal > SECTOR_NONE) {
                if (qt >= QUEUE_MAX) {
                    fprintf(stderr, "Queue overflow !\n");
                    goto done;
                }
                queue[qt++] = wall->portal;
            }
        }
    }
    done :
        if (found == SECTOR_NONE) {
            fprintf(stderr, "Player not in any sector !\n");
            engine->camera.sector = 1;
        } else {
            engine->camera.sector = found;
        }
        #undef QUEUE_MAX
}

void cleanUpEngine(t_engine *engine) {
    if (engine->render.img)
        mlx_destroy_image(engine->render.mlx, engine->render.img);
    if (engine->render.win)
        mlx_destroy_window(engine->render.mlx, engine->render.win);
}

// Debugging helper
void debug_keys(int keycode) {
    printf("DEBUG: Key Pressed (Code: %d)\n", keycode);
    fflush(stdout);
}

void present(t_engine *engine) {
    if (!engine || !engine->render.mlx || !engine->render.win)
        return ;
    mlx_put_image_to_window(engine->render.mlx,
                            engine->render.win,
                            engine->render.img,
                            0, 0);
}

int keyPress(int keycode, t_engine *engine) {
    debug_keys(keycode);
    if (keycode == ESC || keycode == ESC_RAW) { // ESC
        engine->isRunning = false;
        if (engine->render.mlx)
            mlx_loop_end(engine->render.mlx);
    }
    else if (keycode == W || keycode == Z) // Forward
        engine->key.up = true;
    else if (keycode == S) // Backward
        engine->key.down = true;
    else if (keycode == A || keycode == Q) // Strafe left
        engine->key.strafeLeft = true;
    else if (keycode == D) // Strafe right
        engine->key.strafeRight = true;
    else if (keycode == LEFT) // Turn left
        engine->key.left = true;
    else if (keycode == RIGHT) // Turn right
        engine->key.right = true;
    else if (keycode == SPACE) // Toggle debug mode
        engine->debugMode = !engine->debugMode;
    else if (keycode == F2) { // Toggle debug logging
        engine->debugLog.enabled = !engine->debugLog.enabled;
        printf("Debug logging %s\n", engine->debugLog.enabled ? "ENABLED" : "DISABLED");
    }
    else if (keycode == UP)
        engine->key.lookUp = true;
    else if (keycode == DOWN) 
        engine->key.lookDown = true;
    return (0);
}

int keyReleas(int keycode, t_engine *engine) {
    if (keycode == W || keycode == Z) // Forward
        engine->key.up = false;
    else if (keycode == S) // Backward
        engine->key.down = false;
    else if (keycode == A || keycode == Q) // Strafe left
        engine->key.strafeLeft = false;
    else if (keycode == D) // Strafe right
        engine->key.strafeRight = false;
    else if (keycode == LEFT) // Turn left
        engine->key.left = false;
    else if (keycode == RIGHT) // Turn right
        engine->key.right = false;
    else if (keycode == UP) // Look up
        engine->key.lookUp = false;
    else if (keycode == DOWN) // Look down
        engine->key.lookDown = false;
    return (0);
}

int gameLoop(t_engine *engine) {
    // Check if we should exit
    if (!engine->isRunning) {
        if (engine->render.mlx)
            mlx_loop_end(engine->render.mlx);
        return (0);
    }

    // Calculate delta time
    engine->currentTime = getTime();
    engine->deltaTime = (f32)(engine->currentTime - engine->lastTime);
    engine->lastTime = engine->currentTime;

    // Clamp delta time to avoid huge jumps (e.g., when debugging)
    if (engine->deltaTime > 0.1f)
        engine->deltaTime = 0.1f;

    // Movement speeds (units per second)
    const f32 rotSpeed = 2.0f;      // radians per second
    const f32 moveSpeed = 5.0f;     // units per second
    const f32 playerRadius = 3.0f;

    if (engine->key.right)
        engine->camera.angle += rotSpeed * engine->deltaTime;
    if (engine->key.left)
        engine->camera.angle -= rotSpeed * engine->deltaTime;

    engine->camera.cosA = cosf(engine->camera.angle);
    engine->camera.sinA = sinf(engine->camera.angle);

    // Pitch / Look up-down
    #define PITCH_SPEED 2.0f  // radians per second
    
    if (engine->key.lookUp) {
        engine->camera.pitch += PITCH_SPEED * engine->deltaTime;
        if (engine->camera.pitch > PI_2)  // Clamp to +90°
            engine->camera.pitch = PI_2;
    }
    if (engine->key.lookDown) {
        engine->camera.pitch -= PITCH_SPEED * engine->deltaTime;
        if (engine->camera.pitch < -PI_2)  // Clamp to -90°
            engine->camera.pitch = -PI_2;
    }
    
    // Precompute cos/sin of pitch
    engine->camera.pitch_cos = cosf(engine->camera.pitch);
    engine->camera.pitch_sin = sinf(engine->camera.pitch);
    
    // Calculate new horizon position
    engine->camera.horizon = SCREENH / 2.0f + (engine->camera.pitch / PI_2) * (SCREENH / 2.0f);

    t_v2 moveVec = {0};
    // Forward/backward movement
    if (engine->key.up) {
        moveVec.x += engine->camera.cosA;
        moveVec.y += engine->camera.sinA;
    }
    if (engine->key.down) {
        moveVec.x -= engine->camera.cosA;
        moveVec.y -= engine->camera.sinA;
    }
    
    // Strafe movement (perpendicular to forward direction)
    // Perpendicular vector: rotate forward by 90° → (cosA, sinA) becomes (-sinA, cosA)
    if (engine->key.strafeRight) {
        moveVec.x += -engine->camera.sinA;
        moveVec.y += engine->camera.cosA;
    }
    if (engine->key.strafeLeft) {
        moveVec.x -= -engine->camera.sinA;
        moveVec.y -= engine->camera.cosA;
    }
    
    if (moveVec.x != 0 || moveVec.y != 0) {
        t_v2 target = {
            engine->camera.pos.x + moveVec.x * moveSpeed * engine->deltaTime,
            engine->camera.pos.y + moveVec.y * moveSpeed * engine->deltaTime
        };
        tryMove(engine, &engine->camera.pos, target, playerRadius);
    }
    
    // updatePlayerSector(engine); // Removed - handled by tryMove now
    
    // Log position every 60 frames (~1 second at 60 FPS)
    if (engine->frameCount % 60 == 0)
        debug_log_position(&engine->debugLog, engine->frameCount, 
                          engine->currentTime, &engine->camera);
    
    engine->frameCount++;
    
    render(engine);
    present(engine);

    return (0);
}

int initEngine(t_engine *engine) {
    memset(engine, 0, sizeof(t_engine));
    engine->render.mlx = mlx_init();
    if (!engine->render.mlx) {

        return (-1);
    }

    engine->render.win = mlx_new_window(engine->render.mlx, SCREENW, SCREENH, "Doom N");
    if (!engine->render.win) {

        return (-1);
    }

    engine->render.img = mlx_new_image(engine->render.mlx, SCREENW, SCREENH);
    if (!engine->render.img) {

        return (-1);
    }
    engine->render.pixels = (u32*)mlx_get_data_addr(engine->render.img, &engine->render.bpp, &engine->render.lineLen, &engine->render.endian);
    engine->camera.pos = (t_v2) { 3.0f, 3.0f };
    engine->camera.z = EYE_Z;           // Hauteur des yeux (1.65m)
    engine->camera.angle = PI / 2.0f;
    engine->camera.sector = 1;
    engine->camera.pitch = 0.0f;
    engine->camera.pitch_cos = cos(0.0f);
    engine->camera.pitch_sin = sin(0.0f);
    engine->camera.horizon = SCREENH / 2.0f;
    engine->isRunning = true;
    engine->debugMode = false;
    engine->lastTime = getTime();
    engine->deltaTime = 0.016f;  // Initial value ~60 FPS
    engine->frameCount = 0;
    
    // Initialize debug logging
    debug_log_init(&engine->debugLog, "debug_log.json");
    printf("Debug log initialized. Press F2 to toggle logging.\n");

    return (0);
}

int main(void)
{

    t_engine engine;
    int ret;

    if (initEngine(&engine) != 0) {
        fprintf(stderr, "Failed to init engine\n");
        return (1);
    }

    ret = load_sectors(&engine, "res/level.txt");
    if (ret != 0) {
        fprintf(stderr, "Failed to load map: error %d\n", ret);
        cleanUpEngine(&engine);
        return (1);
    }

    printf("Loaded %zu sector with %zu walls\n", engine.nSectors, engine.nWalls);
    mlx_hook(engine.render.win, 2, 1L<<0, keyPress, &engine);
    mlx_hook(engine.render.win, 3, 1L<<1, keyReleas, &engine);
    mlx_hook(engine.render.win, 17, 0, closeWindow, &engine);
    mlx_loop_hook(engine.render.mlx, gameLoop, &engine);
    mlx_loop(engine.render.mlx);

    return (0);
}