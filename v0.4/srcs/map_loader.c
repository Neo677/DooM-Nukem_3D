#include "env.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
** Format de fichier map v1:
** Map format v1
** width 10
** height 10
** 
** [DATA]
** 1 1 1 1 ...
** ...
*/

static int get_next_line(int fd, char **line)
{
    char    buf[1];
    char    *str;
    int     ret;
    int     len;

    str = NULL;
    len = 0;
    *line = NULL;
    while ((ret = read(fd, buf, 1)) > 0)
    {
        if (buf[0] == '\n')
            break;
        str = realloc(str, len + 2);
        str[len] = buf[0];
        str[len + 1] = '\0';
        len++;
    }
    if (ret < 0) return (-1);
    if (ret == 0 && len == 0) return (0);
    *line = str ? str : strdup("");
    return (1);
}

static int parse_dimensions(int fd, t_env *env)
{
    char *line;
    int w_found = 0;
    int h_found = 0;
    
    
    env->map.width = 0;
    env->map.height = 0;
    
    while (get_next_line(fd, &line) > 0)
    {
        if (strncmp(line, "width", 5) == 0)
        {
            env->map.width = atoi(line + 6);
            w_found = 1;
        }
        else if (strncmp(line, "height", 6) == 0)
        {
            env->map.height = atoi(line + 7);
            h_found = 1;
        }
        else if (strncmp(line, "Grid:", 5) == 0)
        {
            free(line);
            break; 
        }
        free(line);
        if (w_found && h_found && env->map.width > 0 && env->map.height > 0)
            break;
    }
    
    if (env->map.width <= 0 || env->map.height <= 0)
    {
        printf("Error: Invalid map dimensions\n");
        return (-1);
    }
    
    return (0);
}

static int allocate_grid(t_env *env)
{
    env->map.grid = (int **)malloc(sizeof(int *) * env->map.height);
    if (!env->map.grid)
        return (-1);
        
    for (int y = 0; y < env->map.height; y++)
    {
        env->map.grid[y] = (int *)malloc(sizeof(int) * env->map.width);
        if (!env->map.grid[y])
            return (-1);
        memset(env->map.grid[y], 0, sizeof(int) * env->map.width);
    }
    return (0);
}

static int fill_grid(int fd, t_env *env)
{
    int val;
    char buf[64];
    int buf_idx = 0;
    int x = 0;
    int y = 0;
    char c;
    
    while (read(fd, &c, 1) > 0)
    {
        if (c >= '0' && c <= '9')
        {
            buf[buf_idx++] = c;
        }
        else if (buf_idx > 0)
        {
            buf[buf_idx] = '\0';
            val = atoi(buf);
            buf_idx = 0;
            
            if (y < env->map.height && x < env->map.width)
            {
                env->map.grid[y][x] = val;
                x++;
                if (x >= env->map.width)
                {
                    x = 0;
                    y++;
                }
            }
        }
    }
    
    if (buf_idx > 0 && y < env->map.height && x < env->map.width)
    {
        buf[buf_idx] = '\0';
        env->map.grid[y][x] = atoi(buf);
    }
    
    return (0);
}

int load_map(t_env *env, const char *filename)
{
    int fd;
    
    printf("Loading map: %s\n", filename);
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        printf("Error: Cannot open map file\n");
        return (-1);
    }
    
    if (parse_dimensions(fd, env) != 0)
    {
        close(fd);
        return (-1);
    }
    
    printf("Map dimensions: %dx%d\n", env->map.width, env->map.height);
    
    if (allocate_grid(env) != 0)
    {
        close(fd);
        return (-1);
    }
    
    
    
    
    close(fd);
    fd = open(filename, O_RDONLY);
    
    
    char *line;
    while (get_next_line(fd, &line) > 0)
    {
        int is_data_start = (strncmp(line, "Grid:", 5) == 0);
        free(line);
        if (is_data_start) break;
    }
    
    if (fill_grid(fd, env) != 0)
    {
        close(fd);
        return (-1);
    }
    
    close(fd);
    printf("Map loaded successfully\n");
    return (0);
}
