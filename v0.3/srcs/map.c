#include "../header/global.h"
#include <errno.h>

enum scan_section { SCAN_NONE, SCAN_SECTOR, SCAN_WALL };

const char* skipWs(const char* p) {
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return (p);
}

// Fix sector winding order: ensure all sectors are CCW (counter-clockwise)
void fixSectorWinding(t_engine *engine)
{
    for (usize s = 1; s < engine->nSectors; s++) {
        t_sector *sector = &engine->sectors[s];
        
        // Calculate signed area using shoelace formula
        f32 signedArea = 0.0f;
        for (usize i = 0; i < sector->nWalls; i++) {
            t_wall *wall = &engine->walls[sector->firstWall + i];
            signedArea += (f32)(wall->b.x - wall->a.x) * (f32)(wall->b.y + wall->a.y);
        }
        
        // If area > 0, sector is clockwise → reverse all walls
        if (signedArea > 0.0f) {
            printf("Sector %d is CW (area=%.2f), reversing to CCW\n", sector->id, signedArea);
            for (usize i = 0; i < sector->nWalls; i++) {
                t_wall *wall = &engine->walls[sector->firstWall + i];
                // Swap a and b
                t_v2i tmp = wall->a;
                wall->a = wall->b;
                wall->b = tmp;
            }
        } else {
            printf("Sector %d is CCW (area=%.2f), OK\n", sector->id, signedArea);
        }
    }
}
 
int load_sectors(t_engine *engine, const char *path)
{
    engine->nSectors = 1;
    engine->nWalls = 0;

    FILE *f = fopen(path, "r");
    if (!f)
        return (-1);
        
    enum scan_section ss = SCAN_NONE;
    char line[1024];
    int retVal = 0;

    while (fgets(line, sizeof(line), f)) {
        const char *p = skipWs(line);
        if (!*p || *p == '#')
            continue;
        if (*p == '[') {
            const char *end = strchr(p, ']');
            if (!end) {
                retVal = -1;
                break;
            }
            size_t len = (size_t) (end - (p + 1));
            char section[32];
            if (len >= sizeof(section)) {
                retVal = -3;
                break;
            }
            memcpy(section, p + 1, len);
            section[len] = '\0';
            
            if (strcmp(section, "SECTOR") == 0)
                ss = SCAN_SECTOR;
            else if (strcmp(section, "WALL") == 0)
                ss = SCAN_WALL;
            else {
                retVal = -4;
                break;
            }
            continue;
        }
        switch (ss) {
            case SCAN_WALL: {
                if (engine->nWalls >= sizeof(engine->walls) / sizeof(engine->walls[0])) {
                    retVal = -10; 
                    break;
                }
                t_wall *w = &engine->walls[engine->nWalls++];
                w->color = 0x00FFFFFF;
                if (sscanf(p, "%d %d %d %d %d", &w->a.x, &w->a.y, &w->b.x, &w->b.y, &w->portal) != 5) {
                    retVal = -5; 
                    break;
                }
            } 
            break;
            case SCAN_SECTOR : {
                if (engine->nSectors >= SECTOR_MAX) {
                    retVal = -11;
                    break;
                }
                t_sector *s = &engine->sectors[engine->nSectors++];
                s->floorCol = 0x00222222;  // Gris tres fonce
                s->ceilCol = 0x00404040; 
                if (sscanf(p, "%d %zu %zu %f %f", &s->id, &s->firstWall, &s->nWalls, &s->zFloor, &s->zCeil) != 5) {
                    retVal = -6;
                    break;
                }
            }
            break;
            default: {
                retVal = -7;
                break;
            }
        }
        if (retVal)
            break;
    }
    if (!retVal && ferror(f))
        retVal = -128;
    fclose(f);

    if (!retVal) {
        // Fix winding order before validation
        fixSectorWinding(engine);
        
        for (usize i = 1; i < engine->nSectors; i++) {
            const t_sector *s = &engine->sectors[i];
            if ((s->firstWall + s->nWalls) > engine->nWalls) {
                retVal = -12;
                break;
            }
            if (s->zFloor > s->zCeil) {
                retVal = -13;
                break;
            }
        }
    }
    return (retVal);
}