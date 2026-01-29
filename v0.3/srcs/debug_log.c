#include "../header/debug_log.h"
#include <string.h>

void debug_log_init(t_debug_log *log, const char *filename)
{
    memset(log, 0, sizeof(t_debug_log));
    log->enabled = true;  // Enable by default
    log->first_event = true;
    
    log->file = fopen(filename, "w");
    if (!log->file) {
        fprintf(stderr, "Failed to open debug log: %s\n", filename);
        return;
    }
    
    // Write JSON header
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%S", tm_info);
    
    fprintf(log->file, "{\n");
    fprintf(log->file, "  \"session_start\": \"%s\",\n", time_str);
    fprintf(log->file, "  \"events\": [\n");
    fflush(log->file);
}

void debug_log_close(t_debug_log *log)
{
    if (!log->file)
        return;
    
    // Close JSON array and object
    fprintf(log->file, "\n  ]\n}\n");
    fclose(log->file);
    log->file = NULL;
}

void debug_log_position(t_debug_log *log, u64 frame, f64 time, const t_camera *cam)
{
    if (!log->enabled || !log->file)
        return;
    
    // Add comma if not first event
    if (!log->first_event)
        fprintf(log->file, ",\n");
    log->first_event = false;
    
    fprintf(log->file, "    {\n");
    fprintf(log->file, "      \"frame\": %lu,\n", frame);
    fprintf(log->file, "      \"time\": %.3f,\n", time);
    fprintf(log->file, "      \"type\": \"position\",\n");
    fprintf(log->file, "      \"data\": {\n");
    fprintf(log->file, "        \"x\": %.3f,\n", cam->pos.x);
    fprintf(log->file, "        \"y\": %.3f,\n", cam->pos.y);
    fprintf(log->file, "        \"angle\": %.3f,\n", cam->angle);
    fprintf(log->file, "        \"sector\": %d\n", cam->sector);
    fprintf(log->file, "      }\n");
    fprintf(log->file, "    }");
    fflush(log->file);
}

void debug_log_collision(t_debug_log *log, u64 frame, f64 time,
                         const t_wall *wall, t_v2 player_pos, 
                         t_v2 attempted_pos, const char *result)
{
    if (!log->enabled || !log->file)
        return;
    
    // Add comma if not first event
    if (!log->first_event)
        fprintf(log->file, ",\n");
    log->first_event = false;
    
    fprintf(log->file, "    {\n");
    fprintf(log->file, "      \"frame\": %lu,\n", frame);
    fprintf(log->file, "      \"time\": %.3f,\n", time);
    fprintf(log->file, "      \"type\": \"collision\",\n");
    fprintf(log->file, "      \"data\": {\n");
    fprintf(log->file, "        \"wall_a\": {\"x\": %d, \"y\": %d},\n", wall->a.x, wall->a.y);
    fprintf(log->file, "        \"wall_b\": {\"x\": %d, \"y\": %d},\n", wall->b.x, wall->b.y);
    fprintf(log->file, "        \"player_pos\": {\"x\": %.3f, \"y\": %.3f},\n", player_pos.x, player_pos.y);
    fprintf(log->file, "        \"attempted_pos\": {\"x\": %.3f, \"y\": %.3f},\n", attempted_pos.x, attempted_pos.y);
    fprintf(log->file, "        \"result\": \"%s\"\n", result);
    fprintf(log->file, "      }\n");
    fprintf(log->file, "    }");
    fflush(log->file);
}

void debug_log_sector_change(t_debug_log *log, u64 frame, f64 time,
                             int from, int to, int portal_wall)
{
    if (!log->enabled || !log->file)
        return;
    
    // Add comma if not first event
    if (!log->first_event)
        fprintf(log->file, ",\n");
    log->first_event = false;
    
    fprintf(log->file, "    {\n");
    fprintf(log->file, "      \"frame\": %lu,\n", frame);
    fprintf(log->file, "      \"time\": %.3f,\n", time);
    fprintf(log->file, "      \"type\": \"sector_change\",\n");
    fprintf(log->file, "      \"data\": {\n");
    fprintf(log->file, "        \"from\": %d,\n", from);
    fprintf(log->file, "        \"to\": %d,\n", to);
    fprintf(log->file, "        \"portal_wall\": %d\n", portal_wall);
    fprintf(log->file, "      }\n");
    fprintf(log->file, "    }");
    fflush(log->file);
}
