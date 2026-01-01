#include "../header/global.h"
#include <errno.h>

enum scan_section { SCAN_NONE, SCAN_SECTOR, SCAN_WALL };

const char* skipWs(const char* p) {
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return (p);
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
                w->color = 0xFFFFFFFF;
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
                s->floorCol = 0xFF202020;
                s->ceilCol = 0xFF404040;
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