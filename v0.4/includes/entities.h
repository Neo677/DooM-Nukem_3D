#ifndef ENTITIES_H
# define ENTITIES_H

# include "env.h"

typedef enum {
    ENTITY_ENEMY,
    ENTITY_PICKUP,
    ENTITY_DECORATION
}       e_entity_type;

typedef enum {
    ENEMY_ZOMBIE,
    ENEMY_IMP,
    ENEMY_DEMON
}   e_enemy_type;

typedef enum {
    PICKUP_HEALTH_SMALL,
    PICKUP_HEALTH_LARGE,
    PICKUP_AMMO_BULLETS,
    PICKUP_AMMO_SHELLS,
    PICKUP_ARMOR,
    PICKUP_WEAPON_SHOTGUN,
    PICKUP_WEAPON_CHAINGUN
}   e_pickup_type;

typedef struct s_entity {
    e_entity_type   type;
    int             id;
    double          x;
    double          y;
    double          z;
    int             sector;
    int             sprite_id;
    double          scale;
    int             active;
    union {
        struct  {
            e_enemy_type    enemy_type;
            int             health;
            double          angle;
            int             state;
        }       enemy;
        struct {
            e_pickup_type   pickup_type;
            int             value;
        } pickup;
    } data;
    struct s_entity     *next;
}       t_entity;

// Functions
int         load_entities(t_env *env, const char *filename);
void        free_entities(t_entity_manager *mgr);
t_entity    *spawn_entity(t_entity_manager *mgr, e_entity_type type, double x, double y, int sector);

#endif