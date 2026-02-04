#include "entities.h"
#include "env.h"
#include "defines.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Réutiliser get_next_line
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
        if (!str)
            return (-1);
        str[len] = buf[0];
        str[len + 1] = '\0';
        len++;
    }
    if (ret < 0) return (-1);
    if (ret == 0 && len == 0) return (0);
    *line = str ? str : strdup("");
    return (1);
}

// Parse une ligne d'ennemi
static t_entity *parse_enemy(char *line, int id)
{
    t_entity *entity;
    char type_str[32];
    double x, y;
    int sector;
    e_enemy_type enemy_type;
    
    if (sscanf(line, "ENEMY %s %lf %lf %d", type_str, &x, &y, &sector) != 4)
        return (NULL);
    
    entity = malloc(sizeof(t_entity));
    if (!entity) return (NULL);
    
    // Déterminer le type d'ennemi par son nom
    enemy_type = ENEMY_ZOMBIEMAN; // Par défaut
    
    if (strcmp(type_str, "zombieman") == 0)
        enemy_type = ENEMY_ZOMBIEMAN;
    else if (strcmp(type_str, "shotgun_guy") == 0 || strcmp(type_str, "shotgunguy") == 0)
        enemy_type = ENEMY_SHOTGUN_GUY;
    else if (strcmp(type_str, "imp") == 0 || strcmp(type_str, "doom_imp") == 0)
        enemy_type = ENEMY_DOOM_IMP;
    else if (strcmp(type_str, "demon") == 0 || strcmp(type_str, "pinky") == 0)
        enemy_type = ENEMY_DEMON;
    else if (strcmp(type_str, "lost_soul") == 0 || strcmp(type_str, "lostsoul") == 0)
        enemy_type = ENEMY_LOST_SOUL;
    else if (strcmp(type_str, "cacodemon") == 0)
        enemy_type = ENEMY_CACODEMON;
    else if (strcmp(type_str, "baron") == 0 || strcmp(type_str, "baron_of_hell") == 0)
        enemy_type = ENEMY_BARON_OF_HELL;
    else if (strcmp(type_str, "hell_razer") == 0 || strcmp(type_str, "hellrazer") == 0)
        enemy_type = ENEMY_HELL_RAZER;
    else if (strcmp(type_str, "mancubus") == 0)
        enemy_type = ENEMY_MANCUBUS;
    else if (strcmp(type_str, "revenant") == 0)
        enemy_type = ENEMY_REVENANT;
    else if (strcmp(type_str, "pain_elemental") == 0 || strcmp(type_str, "painelemental") == 0)
        enemy_type = ENEMY_PAIN_ELEMENTAL;
    else if (strcmp(type_str, "cyberdemon") == 0)
        enemy_type = ENEMY_CYBERDEMON;
    else if (strcmp(type_str, "tyrant") == 0)
        enemy_type = ENEMY_TYRANT;
    else if (strcmp(type_str, "harvester") == 0)
        enemy_type = ENEMY_HARVESTER;
    else if (strcmp(type_str, "whiplash") == 0)
        enemy_type = ENEMY_WHIPLASH;
    else if (strcmp(type_str, "gargoyle") == 0)
        enemy_type = ENEMY_GARGOYLE;
    else if (strcmp(type_str, "cuiball") == 0)
        enemy_type = ENEMY_CUIBALL;
    else if (strcmp(type_str, "nazi") == 0)
        enemy_type = ENEMY_NAZI;
    else if (strcmp(type_str, "scientist") == 0 || strcmp(type_str, "scientist_zombie") == 0)
        enemy_type = ENEMY_SCIENTIST_ZOMBIE;
    else if (strcmp(type_str, "zombie_earth") == 0)
        enemy_type = ENEMY_ZOMBIE_EARTH;
    else if (strcmp(type_str, "zombie_hell") == 0)
        enemy_type = ENEMY_ZOMBIE_HELL;
    else if (strcmp(type_str, "mecha_zombie") == 0 || strcmp(type_str, "mechazombie") == 0)
        enemy_type = ENEMY_MECHA_ZOMBIE;
    else
    {
        DEBUG_LOG("Warning: Unknown enemy type '%s', using zombieman\n", type_str);
    }
    
    // Initialiser les données de base
    entity->type = ENTITY_ENEMY;
    entity->id = id;
    entity->x = x;
    entity->y = y;
    entity->z = 0.0;
    entity->sector = sector;
    entity->active = 1;
    entity->next = NULL;
    
    // Initialiser les données de l'ennemi avec la fonction dédiée
    init_enemy_data(entity, enemy_type);
    
    // Le sprite_id a été configuré par init_enemy_data
    
    VERBOSE_LOG("  Loaded enemy: %s (%s) at (%.1f, %.1f) sector %d, sprite_id=%d\n", 
           type_str, get_enemy_name(enemy_type), x, y, sector, entity->sprite_id);
    
    return (entity);
}

// Parse une ligne d'objet
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
    else if (strcmp(type_str, "health_large") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_HEALTH_LARGE;
        entity->data.pickup.value = 50;
        entity->sprite_id = 21;
    }
    else if (strcmp(type_str, "ammo_bullets") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_AMMO_BULLETS;
        entity->data.pickup.value = 20;
        entity->sprite_id = 22;
    }
    else if (strcmp(type_str, "ammo_shells") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_AMMO_SHELLS;
        entity->data.pickup.value = 8;
        entity->sprite_id = 23;
    }
    else if (strcmp(type_str, "armor") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_ARMOR;
        entity->data.pickup.value = 50;
        entity->sprite_id = 24;
    }
    else if (strcmp(type_str, "weapon_shotgun") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_WEAPON_SHOTGUN;
        entity->data.pickup.value = 1;
        entity->sprite_id = 25;
    }
    else if (strcmp(type_str, "weapon_chaingun") == 0)
    {
        entity->data.pickup.pickup_type = PICKUP_WEAPON_CHAINGUN;
        entity->data.pickup.value = 1;
        entity->sprite_id = 26;
    }
    
    VERBOSE_LOG("  Loaded pickup: %s at (%.1f, %.1f) sector %d\n", 
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
    
    VERBOSE_LOG("Loading entities from: %s\n", filename);
    
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        DEBUG_LOG("Warning: No entity file found (optional)\n");
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
    entity->scale = 1.0;
    entity->sprite_id = 0;
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
