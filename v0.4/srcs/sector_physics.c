#include "env.h"
#include <stdio.h>
#include <math.h>

void update_sectors(t_env *env) {
    if (env->sector_map.nb_sectors == 0)
        return;
    for (int i = 0; i < env->sector_map.nb_sectors; i++) {
        t_sector *sect = &env->sector_map.sectors[i];
        if (sect->state == 1) { 
            int floor_moved = 0;
            int ceil_moved = 0; 
            if (sect->floor_height < sect->target_floor) {
                sect->floor_height += sect->speed;
                if (sect->floor_height > sect->target_floor) sect->floor_height = sect->target_floor;
                floor_moved = 1;
            } else if (sect->floor_height > sect->target_floor) {
                sect->floor_height -= sect->speed;
                if (sect->floor_height < sect->target_floor) sect->floor_height = sect->target_floor;
                floor_moved = -1;
            }  if (sect->ceiling_height < sect->target_ceil) {
                sect->ceiling_height += sect->speed;
                if (sect->ceiling_height > sect->target_ceil) sect->ceiling_height = sect->target_ceil;
                ceil_moved = 1;
            } else if (sect->ceiling_height > sect->target_ceil) {
                sect->ceiling_height -= sect->speed;
                if (sect->ceiling_height < sect->target_ceil) sect->ceiling_height = sect->target_ceil;
                ceil_moved = -1;
            } if (fabs(sect->floor_height - sect->target_floor) < 0.001 &&
                fabs(sect->ceiling_height - sect->target_ceil) < 0.001) {
                sect->state = 0; 
                if (sect->trigger_id == 999) {
                    if (sect->target_floor > 0.1) sect->target_floor = 0.0;
                    else sect->target_floor = 3.0;
                    sect->state = 1; 
                }
            } if (env->player.current_sector == i) {
                double headroom = sect->ceiling_height - sect->floor_height;
                if (headroom < PLAYER_EYE_HEIGHT + 0.1) { 
                    DEBUG_LOG("[CRUSH] Sector %d stopping execution (Player inside)\n", i);
                    sect->state = 0;      
                    if (ceil_moved == -1)
                        sect->ceiling_height += sect->speed;
                    if (floor_moved == 1)
                        sect->floor_height -= sect->speed;
                } if (floor_moved == 1 && env->player.height <= sect->floor_height + PLAYER_EYE_HEIGHT + 0.1) {
                     env->player.height += sect->speed;
                     env->player.pos.x += 0; 
                }
            }
        }
    }
}
