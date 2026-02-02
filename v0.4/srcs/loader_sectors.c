#include "loader_sectors.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Helper pour lire ligne par ligne (simplifié)
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
    
    // Lire jusqu'à trouver "Vertices N"
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
    env->sector_map.vertices = malloc(sizeof(t_vertex) * count);
    
    for (int i = 0; i < count; i++)
    {
        if (get_next_line(fd, &line) <= 0) return (-1);
        
        int id;
        double x, y;
        sscanf(line, "%d %lf %lf", &id, &x, &y);
        
        if (id >= 0 && id < count)
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
    
    // 1. Nb vertices
    if (get_next_line(fd, &line) <= 0) return (-1);
    sector->nb_vertices = atoi(line);
    free(line);
    
    // Allocations
    sector->vertices = malloc(sizeof(int) * sector->nb_vertices);
    sector->neighbors = malloc(sizeof(int) * sector->nb_vertices);
    sector->wall_textures = malloc(sizeof(int) * sector->nb_vertices);
    sector->rendered = 0;
    
    // 2. Vertex indices
    if (get_next_line(fd, &line) <= 0) return (-1);
    // Parse line manually or using strtok
    char *ptr = line;
    for(int i=0; i<sector->nb_vertices; i++) {
        sector->vertices[i] = strtod(ptr, &ptr);
    }
    free(line);

    // 3. Neighbors
    if (get_next_line(fd, &line) <= 0) return (-1);
    ptr = line;
    for(int i=0; i<sector->nb_vertices; i++) {
        sector->neighbors[i] = strtod(ptr, &ptr);
    }
    free(line);
    
    // 4. Heights
    if (get_next_line(fd, &line) <= 0) return (-1);
    sscanf(line, "%lf %lf", &sector->floor_height, &sector->ceiling_height);
    free(line);
    
    // 5. Textures (floor ceiling)
    if (get_next_line(fd, &line) <= 0) return (-1);
    sscanf(line, "%d %d", &sector->floor_texture, &sector->ceiling_texture);
    free(line);
    
    // 6. Wall textures
    if (get_next_line(fd, &line) <= 0) return (-1);
    ptr = line;
    for(int i=0; i<sector->nb_vertices; i++) {
        sector->wall_textures[i] = strtod(ptr, &ptr);
    }
    free(line);
    
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
    env->sector_map.sectors = malloc(sizeof(t_sector) * count);
    
    int loaded = 0;
    while (loaded < count && get_next_line(fd, &line) > 0)
    {
        if (strncmp(line, "Sector", 6) == 0)
        {
            // int id = atoi(line + 7); // unused for now
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
    
    // Reset fd or reopen? Basic implementation assumes efficient sequential read
    // But parse_vertices stops at end of vertices section? 
    // No, it stops at count.
    
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
