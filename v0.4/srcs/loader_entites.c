#include "entities.h"
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


static t_entity *parse_enemy(char *line, int id)
{
    t_entity *entity;
    char type_str[32];
    double x, y;
    int sector;
    
    
    if (sscanf(line, "ENEMY %s %lf %lf %d", type_str, &x, &y, &sector) != 4)
        return (NULL);
    
    entity = malloc(sizeof(t_entity));
    if (!entity) return (NULL);
    
    entity->type = ENTITY_ENEMY;
    entity->id = id;
    entity->x = x;
    entity->y = y;
    entity->z = 0.0;
    entity->sector = sector;
    entity->sprite_id = 0;  
    entity->scale = 1.0;
    entity->active = 1;
    entity->next = NULL;
    
    
    if (strcmp(type_str, "zombie") == 0)
    {
        entity->data.enemy.enemy_type = ENEMY_ZOMBIE;
        entity->data.enemy.health = 100;
        entity->sprite_id = 10; 
    }
    else if (strcmp(type_str, "imp") == 0)
    {
        entity->data.enemy.enemy_type = ENEMY_IMP;
        entity->data.enemy.health = 60;
        entity->sprite_id = 11;
    }
    
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0; 
    
    printf("  Loaded enemy: %s at (%.1f, %.1f) sector %d\n", 
           type_str, x, y, sector);
    
    return (entity);
}


static t_entity *parse_pickup(char *line, int id)
{
    t_entity *entity;
    char type_str[32];
    double x, y;
    int sector;
    
    
    if (sscanf(line, "PICKUP %s %lf %lf %d", type_str, &x, &y, &sector) != 4)
        return (NULL);
    
    entity = malloc(sizeof(t_entity));
    if (!entity) return (NULL);
    
    entity->type = ENTITY_PICKUP;
    entity->id = id;
    entity->x = x;
    entity->y = y;
    entity->z = 0.0;
    entity->sector = sector;
    entity->scale = 0.5;
    entity->active = 1;
    entity->next = NULL;
    
    
    if (strcmp(type_str, "health_small") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_HEALTH_SMALL;
        entity->data.pickup.value = 25;
        entity->sprite_id = 20;
    }
    else if (strcmp(type_str, "ammo_bullets") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_AMMO_BULLETS;
        entity->data.pickup.value = 20;
        entity->sprite_id = 21;
    }
    
    printf("  Loaded pickup: %s at (%.1f, %.1f) sector %d\n", 
           type_str, x, y, sector);
    
    return (entity);
}


int load_entities(t_env *env, const char *filename)
{
    int fd;
    char *line;
    t_entity *entity;
    t_entity *tail;
    int count;
    
    printf("Loading entities from: %s\n", filename);
    
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        printf("Warning: No entity file found (optional)\n");
        env->entity_mgr.entities = NULL;
        env->entity_mgr.count = 0;
        return (0); 
    }
    
    env->entity_mgr.entities = NULL;
    env->entity_mgr.count = 0;
    tail = NULL;
    count = 0;
    
    while (get_next_line(fd, &line) > 0)
    {
        if (line[0] == '#' || line[0] == '\0')
        {
            free(line);
            continue; 
        }
        
        entity = NULL;
        
        if (strncmp(line, "ENEMY", 5) == 0)
            entity = parse_enemy(line, count);
        else if (strncmp(line, "PICKUP", 6) == 0)
            entity = parse_pickup(line, count);
        
        free(line);
        
        if (entity)
        {
            
            if (!env->entity_mgr.entities)
                env->entity_mgr.entities = entity;
            else
                tail->next = entity;
            
            tail = entity;
            count++;
        }
    }
    
    close(fd);
    env->entity_mgr.count = count;
    
    printf("✅ Loaded %d entities\n", count);
    return (0);
}


void free_entities(t_entity_manager *mgr)
{
    t_entity *current;
    t_entity *next;
    
    current = mgr->entities;
    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
    
    mgr->entities = NULL;
    mgr->count = 0;
}


t_entity *spawn_entity(t_entity_manager *mgr, e_entity_type type, 
                       double x, double y, int sector)
{
    t_entity *entity;
    t_entity *tail;
    
    entity = malloc(sizeof(t_entity));
    if (!entity) return (NULL);
    
    entity->type = type;
    entity->id = mgr->count;
    entity->x = x;
    entity->y = y;
    entity->z = 0.0;
    entity->sector = sector;
    entity->active = 1;
    entity->next = NULL;
    
    
    if (!mgr->entities)
        mgr->entities = entity;
    else
    {
        tail = mgr->entities;
        while (tail->next)
            tail = tail->next;
        tail->next = entity;
    }
    
    mgr->count++;
    return (entity);
}