#include "debug.h"
#include <stdarg.h>

void    log_msg(int level, const char *format, ...)
{
    va_list args;
    
    // Prefix color
    if (level == LOG_INFO) printf("%s[INFO] ", C_BLUE);
    else if (level == LOG_WARN) printf("%s[WARN] ", C_YELLOW);
    else if (level == LOG_ERROR) printf("%s[ERROR] ", C_RED);
    
    // Print message
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    // Reset color + newline
    printf("%s\n", C_RESET);
}

void    print_vec2(const char *name, double x, double y)
{
    printf("%s: [%.2f, %.2f]\n", name, x, y);
}
