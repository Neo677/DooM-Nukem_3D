#include "entities.h"
#include "env.h"
#include "defines.h"
#include <stdlib.h>

// Initialisation des données pour tous les types d'ennemis
// Baseé sur les sprites disponibles dans assets/sprites_organized/monsters/

// Baron of Hell
void init_baron_of_hell(t_entity *entity)
{
    entity->sprite_id = 100; // ID sprite à ajuster
    entity->scale = 1.5;
    entity->data.enemy.health = 1000;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0; // IDLE
}

// Cacodemon
void init_cacodemon(t_entity *entity)
{
    entity->sprite_id = 101;
    entity->scale = 1.3;
    entity->data.enemy.health = 400;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Cuiball (Cube Ball)
void init_cuiball(t_entity *entity)
{
    entity->sprite_id = 102;
    entity->scale = 0.8;
    entity->data.enemy.health = 150;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Cyberdemon
void init_cyberdemon(t_entity *entity)
{
    entity->sprite_id = 103;
    entity->scale = 2.0;
    entity->data.enemy.health = 4000;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Demon (Pinky)
void init_demon(t_entity *entity)
{
    entity->sprite_id = 104;
    entity->scale = 1.2;
    entity->data.enemy.health = 150;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Doom Imp
void init_doom_imp(t_entity *entity)
{
    entity->sprite_id = 105;
    entity->scale = 1.0;
    entity->data.enemy.health = 60;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Gargoyle
void init_gargoyle(t_entity *entity)
{
    entity->sprite_id = 106;
    entity->scale = 1.1;
    entity->data.enemy.health = 80;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Harvester
void init_harvester(t_entity *entity)
{
    entity->sprite_id = 107;
    entity->scale = 1.4;
    entity->data.enemy.health = 300;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Hell Razer
void init_hell_razer(t_entity *entity)
{
    entity->sprite_id = 108;
    entity->scale = 1.3;
    entity->data.enemy.health = 200;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Lost Soul
void init_lost_soul(t_entity *entity)
{
    entity->sprite_id = 109;
    entity->scale = 0.7;
    entity->data.enemy.health = 100;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Mancubus
void init_mancubus(t_entity *entity)
{
    entity->sprite_id = 110;
    entity->scale = 1.6;
    entity->data.enemy.health = 600;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Mecha Zombie
void init_mecha_zombie(t_entity *entity)
{
    entity->sprite_id = 111;
    entity->scale = 1.2;
    entity->data.enemy.health = 120;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Nazi
void init_nazi(t_entity *entity)
{
    entity->sprite_id = 112;
    entity->scale = 1.0;
    entity->data.enemy.health = 50;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Pain Elemental
void init_pain_elemental(t_entity *entity)
{
    entity->sprite_id = 113;
    entity->scale = 1.4;
    entity->data.enemy.health = 400;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Revenant
void init_revenant(t_entity *entity)
{
    entity->sprite_id = 114;
    entity->scale = 1.3;
    entity->data.enemy.health = 300;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Scientist Zombie
void init_scientist_zombie(t_entity *entity)
{
    entity->sprite_id = 115;
    entity->scale = 1.0;
    entity->data.enemy.health = 20;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Shotgun Guy
void init_shotgun_guy(t_entity *entity)
{
    entity->sprite_id = 116;
    entity->scale = 1.0;
    entity->data.enemy.health = 30;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Tyrant
void init_tyrant(t_entity *entity)
{
    entity->sprite_id = 117;
    entity->scale = 2.5;
    entity->data.enemy.health = 5000;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Whiplash
void init_whiplash(t_entity *entity)
{
    entity->sprite_id = 118;
    entity->scale = 1.2;
    entity->data.enemy.health = 250;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Zombie Earth
void init_zombie_earth(t_entity *entity)
{
    entity->sprite_id = 119;
    entity->scale = 1.0;
    entity->data.enemy.health = 100;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Zombie Hell
void init_zombie_hell(t_entity *entity)
{
    entity->sprite_id = 120;
    entity->scale = 1.1;
    entity->data.enemy.health = 150;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Zombieman (classic)
void init_zombieman(t_entity *entity)
{
    entity->sprite_id = 121;
    entity->scale = 1.0;
    entity->data.enemy.health = 20;
    entity->data.enemy.angle = 0.0;
    entity->data.enemy.state = 0;
}

// Fonction principale d'initialisation des données ennemies
void init_enemy_data(t_entity *entity, e_enemy_type type)
{
    if (!entity)
        return;
    
    entity->type = ENTITY_ENEMY;
    entity->active = 1;
    entity->data.enemy.enemy_type = type;
    
    switch (type)
    {
        case ENEMY_BARON_OF_HELL:
            init_baron_of_hell(entity);
            break;
        case ENEMY_CACODEMON:
            init_cacodemon(entity);
            break;
        case ENEMY_CUIBALL:
            init_cuiball(entity);
            break;
        case ENEMY_CYBERDEMON:
            init_cyberdemon(entity);
            break;
        case ENEMY_DEMON:
            init_demon(entity);
            break;
        case ENEMY_DOOM_IMP:
            init_doom_imp(entity);
            break;
        case ENEMY_GARGOYLE:
            init_gargoyle(entity);
            break;
        case ENEMY_HARVESTER:
            init_harvester(entity);
            break;
        case ENEMY_HELL_RAZER:
            init_hell_razer(entity);
            break;
        case ENEMY_LOST_SOUL:
            init_lost_soul(entity);
            break;
        case ENEMY_MANCUBUS:
            init_mancubus(entity);
            break;
        case ENEMY_MECHA_ZOMBIE:
            init_mecha_zombie(entity);
            break;
        case ENEMY_NAZI:
            init_nazi(entity);
            break;
        case ENEMY_PAIN_ELEMENTAL:
            init_pain_elemental(entity);
            break;
        case ENEMY_REVENANT:
            init_revenant(entity);
            break;
        case ENEMY_SCIENTIST_ZOMBIE:
            init_scientist_zombie(entity);
            break;
        case ENEMY_SHOTGUN_GUY:
            init_shotgun_guy(entity);
            break;
        case ENEMY_TYRANT:
            init_tyrant(entity);
            break;
        case ENEMY_WHIPLASH:
            init_whiplash(entity);
            break;
        case ENEMY_ZOMBIE_EARTH:
            init_zombie_earth(entity);
            break;
        case ENEMY_ZOMBIE_HELL:
            init_zombie_hell(entity);
            break;
        case ENEMY_ZOMBIEMAN:
            init_zombieman(entity);
            break;
        default:
            // Type par défaut (Zombieman)
            init_zombieman(entity);
            break;
    }
}
