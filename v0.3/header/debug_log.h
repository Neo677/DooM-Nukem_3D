#ifndef DEBUG_LOG_H
# define DEBUG_LOG_H

# include "global.h"
# include <time.h>

// Initialize and close
void debug_log_init(t_debug_log *log, const char *filename);
void debug_log_close(t_debug_log *log);

// Logging functions
void debug_log_position(t_debug_log *log, u64 frame, f64 time, const t_camera *cam);
void debug_log_collision(t_debug_log *log, u64 frame, f64 time,
                         const t_wall *wall, t_v2 player_pos, 
                         t_v2 attempted_pos, const char *result);
void debug_log_sector_change(t_debug_log *log, u64 frame, f64 time,
                             int from, int to, int portal_wall);

#endif
