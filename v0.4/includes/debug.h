#ifndef DEBUG_H
# define DEBUG_H

# include <stdio.h>

// Debug levels
# define LOG_INFO  0
# define LOG_WARN  1
# define LOG_ERROR 2

// Colors
# define C_RESET  "\033[0m"
# define C_RED    "\033[31m"
# define C_GREEN  "\033[32m"
# define C_YELLOW "\033[33m"
# define C_BLUE   "\033[34m"

void    log_msg(int level, const char *format, ...);
void    print_vec2(const char *name, double x, double y);

#endif
