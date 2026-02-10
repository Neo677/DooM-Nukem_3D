#include "loader_sectors.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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

static int parse_vertices(int fd, t_env *env)
{
    char *line;
    int count = 0;
    
    
    while (get_next_line(fd, &line) > 0)
    {
        if (strncmp(line, "Vertices", 8) == 0)
        {
            count = atoi(line + 9);
            free(line);
            break;
        }
        free(line);
    }
    
    if (count <= 0) return (-1);
    
    env->sector_map.nb_vertices = count;
    env->sector_map.vertices = calloc(count, sizeof(t_vertex));
    if (!env->sector_map.vertices)
        return (-1);
    
    for (int i = 0; i < count; i++)
    {
        if (get_next_line(fd, &line) <= 0) return (-1);
        
        int id = 0;
        double x = 0.0;
        double y = 0.0;
        int ret_scan = sscanf(line, "%d %lf %lf", &id, &x, &y);
        
        if (ret_scan == 3 && id >= 0 && id < count)
        {
            env->sector_map.vertices[id].num = id;
            env->sector_map.vertices[id].x = x;
            env->sector_map.vertices[id].y = y;
        }
        free(line);
    }
    printf("Loaded %d vertices\n", count);
    return (0);
}

static int parse_one_sector(int fd, t_sector *sector)
{
    char *line;
    
    
    if (get_next_line(fd, &line) <= 0) return (-1);
    sector->nb_vertices = atoi(line);
    free(line);
    
    
    
    sector->vertices = calloc(sector->nb_vertices, sizeof(int));
    sector->neighbors = calloc(sector->nb_vertices, sizeof(int));
    sector->wall_textures = calloc(sector->nb_vertices, sizeof(int));
    sector->upper_textures = calloc(sector->nb_vertices, sizeof(int));
    sector->lower_textures = calloc(sector->nb_vertices, sizeof(int));
    
    if (!sector->vertices || !sector->neighbors || !sector->wall_textures)
    {
        if (sector->vertices) free(sector->vertices);
        if (sector->neighbors) free(sector->neighbors);
        if (sector->wall_textures) free(sector->wall_textures);
        if (sector->upper_textures) free(sector->upper_textures);
        if (sector->lower_textures) free(sector->lower_textures);
        return (-1);
    }

    
    for (int i = 0; i < sector->nb_vertices; i++) {
        sector->neighbors[i] = -1;
    }
    
    sector->rendered = 0;
    
    
    if (get_next_line(fd, &line) <= 0) 
        return (-1);
    
    char *ptr = line;
    for(int i=0; i<sector->nb_vertices; i++) {
        sector->vertices[i] = strtod(ptr, &ptr);
    }
    free(line);    
    if (get_next_line(fd, &line) <= 0) 
        return (-1);
    ptr = line;
    for(int i=0; i<sector->nb_vertices; i++) {
        sector->neighbors[i] = strtod(ptr, &ptr);
    }
    free(line);
    if (get_next_line(fd, &line) <= 0) 
        return (-1);
    sscanf(line, "%lf %lf", &sector->floor_height, &sector->ceiling_height);
    free(line);
    /* 
       Format attendu map v2 avec Slopes:
       floor_height ceiling_height
       slope_floor slope_ceil ref_wall_floor ref_wall_ceil
    */
    sector->floor_slope = 0.0;
    sector->ceiling_slope = 0.0;
    sector->floor_slope_ref_wall = 0;
    sector->ceiling_slope_ref_wall = 0;
    char *next_line;
    if (get_next_line(fd, &next_line) <= 0) 
        return (-1);
    double sf = 0.0;
    double sc = 0.0;
    int rf = 0;
    int rc = 0;
    int items = sscanf(next_line, "%lf %lf %d %d", &sf, &sc, &rf, &rc);
    if (items == 4)
    {  
        sector->floor_slope = sf;
        sector->ceiling_slope = sc;
        sector->floor_slope_ref_wall = rf;
        sector->ceiling_slope_ref_wall = rc;
        free(next_line);   
        if (get_next_line(fd, &next_line) <= 0) 
            return (-1);
    }
    sscanf(next_line, "%d %d", &sector->floor_texture, &sector->ceiling_texture);
    free(next_line);
    if (get_next_line(fd, &line) <= 0) 
        return (-1);
    ptr = line;
    for(int i=0; i<sector->nb_vertices; i++) {
        sector->wall_textures[i] = strtod(ptr, &ptr);
    }
    free(line);
    for (int i=0; i<sector->nb_vertices; i++) {
        sector->upper_textures[i] = sector->wall_textures[i];
        sector->lower_textures[i] = sector->wall_textures[i];
    } 
    return (0);
}

static int parse_sectors(int fd, t_env *env)
{
    char *line;
    int count = 0;
    
    while (get_next_line(fd, &line) > 0)
    {
        if (strncmp(line, "Sectors", 7) == 0)
        {
            count = atoi(line + 8);
            free(line);
            break;
        }
        free(line);
    }
    
    if (count <= 0) return (-1);
    
    env->sector_map.nb_sectors = count;
    env->sector_map.sectors = calloc(count, sizeof(t_sector));
    
    int loaded = 0;
    while (loaded < count && get_next_line(fd, &line) > 0)
    {
        if (strncmp(line, "Sector", 6) == 0)
        {
            
            if (parse_one_sector(fd, &env->sector_map.sectors[loaded]) == 0)
            {
                loaded++;
            }
        }
        free(line);
    }
    
    printf("Loaded %d sectors\n", loaded);
    return (0);
}

int load_sectors(t_env *env, const char *filename)
{
    int fd;
    
    printf("Loading sectors from: %s\n", filename);
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        printf("Error: Cannot open sector file\n");
        return (-1);
    }
    
    if (parse_vertices(fd, env) != 0)
    {
        printf("Error parsing vertices\n");
        close(fd);
        return (-1);
    }
    
    
    
    
    
    if (parse_sectors(fd, env) != 0)
    {
        printf("Error parsing sectors\n");
        close(fd);
        return (-1);
    }
    
    close(fd);
    printf("Sector map loaded successfully!\n");
    return (0);
}
