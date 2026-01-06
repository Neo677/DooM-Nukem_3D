#include "./header/global.h"
#include "./header/maths.h"
#include "./header/render.h"
#include "./header/map.h"
#include "./header/collision.h"

int closeWindow(t_engine *engine) {
    engine->isRunning = false;
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

void present(t_engine *engine) {
    if (!engine || !engine->render.mlx || !engine->render.win)
        return ;
    mlx_put_image_to_window(engine->render.mlx,
                            engine->render.win,
                            engine->render.img,
                            0, 0);
}

int keyPress(int keycode, t_engine *engine) {
    if (keycode == ESC) // ESC
        engine->isRunning = false;
    else if (keycode == W) // UP / W
        engine->key.up = true;
    else if (keycode == S) // DOWN / S
        engine->key.down = true;
    else if (keycode == A) // LEFT / A
        engine->key.left = true;
    else if (keycode == D) // RIGHT / D
        engine->key.right = true;
    else if (keycode == SPACE) // F1
        engine->debugMode = !engine->debugMode;
    return (0);
}

int keyReleas(int keycode, t_engine *engine) {
    if (keycode == W) // UP / W
        engine->key.up = false;
    else if (keycode == S) // DOWN / S
        engine->key.down = false;
    else if (keycode == A) // LEFT / A
        engine->key.left = false;
    else if (keycode == D) // RIGHT / D
        engine->key.right = false;
    return (0);
}

int gameLoop(t_engine *engine) {
    const f32 rotSpeed = 3.0f * 0.016f;
    const f32 moveSpeed = 3.0f * 0.016f;
    const f32 playerRadius = 3.0f;

    if (engine->key.right)
        engine->camera.angle += rotSpeed;
    if (engine->key.left)
        engine->camera.angle -= rotSpeed;

    engine->camera.cosA = cosf(engine->camera.angle);
    engine->camera.sinA = sinf(engine->camera.angle);


    t_v2 oldPos = engine->camera.pos;
    t_v2 newPos = oldPos;

    if (engine->key.up) {
        engine->camera.pos.x += moveSpeed * engine->camera.cosA;
        engine->camera.pos.y += moveSpeed * engine->camera.sinA;
    }
    if (engine->key.down) {
        engine->camera.pos.x -= moveSpeed * engine->camera.cosA;
        engine->camera.pos.y -= moveSpeed * engine->camera.sinA;
    }

    if (engine->debugMode == true)
        tryMove(engine, &engine->camera.pos, newPos, playerRadius);
    updatePlayerSector(engine); // TODO
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
    engine->camera.angle = 0.0f;
    engine->camera.sector = 1;
    engine->isRunning = true;
    engine->debugMode = false;

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