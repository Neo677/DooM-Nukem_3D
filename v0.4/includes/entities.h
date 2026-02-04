#ifndef ENTITIES_H
# define ENTITIES_H

# include "env.h"
# include "enemy_types.h"

typedef enum {
    ENTITY_ENEMY,
    ENTITY_PICKUP,
    ENTITY_DECORATION
}       e_entity_type;

// L'énumération e_enemy_type est maintenant dans enemy_types.h

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

// Enemy system functions
int         load_enemy_sprites(t_env *env);
void        render_enemies(t_env *env);
void        update_all_enemies_ai(t_env *env);
void        init_enemy_data(t_entity *entity, e_enemy_type type);
const char  *get_enemy_name(e_enemy_type type);
int         get_enemy_sprite_id(e_enemy_type type);

#endif