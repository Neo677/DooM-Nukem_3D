#include "debug.h"
#include <stdarg.h>

void    log_msg(int level, const char *format, ...)
{
    va_list args;
    
    if (level == LOG_INFO) printf("%s[INFO] ", C_BLUE);
    else if (level == LOG_WARN) printf("%s[WARN] ", C_YELLOW);
    else if (level == LOG_ERROR) printf("%s[ERROR] ", C_RED);
    
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("%s\n", C_RESET);
}

void    print_vec2(const char *name, double x, double y)
{
    printf("%s: [%.2f, %.2f]\n", name, x, y);
}
