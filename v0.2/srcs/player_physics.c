#include "../header/game.h"

void CameraTranslate(double deltaTime)
{
    int isMoving;
    Vec2_t newPos;
    Vec2_t movement;
    float moveDist;
    float maxSafeMove;
    int substeps;
    float stepX;
    float stepY;
    int i;

    isMoving = 0;
    global.cam.oldCamPos = global.cam.camPos;
    newPos = global.cam.camPos;
    movement.x = 0;
    movement.y = 0;
    if (global.g_keys[W])
    {
        movement.x += MOV_SPEED * cos(global.cam.camAngle) * deltaTime;
        movement.y += MOV_SPEED * sin(global.cam.camAngle) * deltaTime;
        isMoving = 1;
    }
    else if (global.g_keys[S])
    {
        movement.x -= MOV_SPEED * cos(global.cam.camAngle) * deltaTime;
        movement.y -= MOV_SPEED * sin(global.cam.camAngle) * deltaTime;
        isMoving = 1;
    }
    if (global.flyMode)
    {
        newPos.x += movement.x;
        newPos.y += movement.y;
        global.cam.camPos = newPos;
        if (global.g_keys[SPACE])
        {
            global.cam.camZ += MOV_SPEED * deltaTime;
            isMoving = 1;
        }
        if (global.g_keys[LSHIFT])
        {
            global.cam.camZ -= MOV_SPEED * deltaTime;
            isMoving = 1;
        }
    }
    else
    {
        moveDist = sqrt(movement.x * movement.x + movement.y * movement.y);
        maxSafeMove = CAMERA_RADIUS * 0.5f;
        if (moveDist > maxSafeMove)
        {
            substeps = (int)(moveDist / maxSafeMove) + 1;
            stepX = movement.x / substeps;
            stepY = movement.y / substeps;
            i = 0;
            while (i < substeps)
            {
                newPos.x += stepX;
                newPos.y += stepY;
                newPos = resolveCollision(newPos);
                global.cam.camPos = newPos;
                i++;
            }
        }
        else
        {
            newPos.x += movement.x;
            newPos.y += movement.y;
            global.cam.camPos = resolveCollision(newPos);
        }
    }
    if (global.g_keys[A])
        global.cam.camAngle -= ROT_SPEED * deltaTime;
    else if (global.g_keys[D])
        global.cam.camAngle += ROT_SPEED * deltaTime;
    if (isMoving)
        global.cam.stepWave += 3 * deltaTime;
    if (global.cam.stepWave > M_PI * 2)
        global.cam.stepWave = 0;
}

void handleJump(void)
{
    if (global.flyMode)
        return;
    if (global.g_keys[SPACE] && global.cam.onGround)
    {
        global.cam.velZ = JUMP_VELOCITY;
        global.cam.onGround = 0;
    }
}

void updatePlayerZ(double deltaTime)
{
    t_sector *currentSector;
    float targetFloorZ;
    float ceilingLimit;
    float diff;
    float step;

    if (global.flyMode)
        return;
    if (global.currentSectorId < 0)
        return;
    currentSector = &global.sectors[global.currentSectorId];
    targetFloorZ = currentSector->floorHeight;
    if (!global.cam.onGround)
    {
        global.cam.velZ -= GRAVITY * deltaTime;
        if (global.cam.velZ < -MAX_FALL_VELOCITY)
            global.cam.velZ = -MAX_FALL_VELOCITY;
        global.cam.camZ += global.cam.velZ * deltaTime;
    }
    global.cam.footZ = global.cam.camZ - EYE_HEIGHT;
    if (global.cam.footZ <= targetFloorZ)
    {
        global.cam.footZ = targetFloorZ;
        global.cam.camZ = targetFloorZ + EYE_HEIGHT;
        global.cam.velZ = 0;
        global.cam.onGround = 1;
    }
    ceilingLimit = currentSector->ceilingHeight - 5.0f;
    if (global.cam.camZ > ceilingLimit)
    {
        global.cam.camZ = ceilingLimit;
        if (global.cam.velZ > 0)
            global.cam.velZ = 0;
    }
    if (global.cam.onGround && global.cam.targetZ != global.cam.camZ)
    {
        diff = global.cam.targetZ - global.cam.camZ;
        step = STEP_SMOOTH_SPEED * deltaTime;
        if (fabs(diff) < step)
            global.cam.camZ = global.cam.targetZ;
        else if (diff > 0)
            global.cam.camZ += step;
        else
            global.cam.camZ -= step;
        global.cam.footZ = global.cam.camZ - EYE_HEIGHT;
    }
}
