#include "entities.h"
#include "enemy_types.h"
#include "env.h"
#include "defines.h"





e_enemy_behavior get_enemy_behavior(e_enemy_type type)
{
    switch (type)
    {
        
        case ENEMY_DEMON:
        case ENEMY_WHIPLASH:
            return BEHAVIOR_MELEE;
        
        
        case ENEMY_DOOM_IMP:
        case ENEMY_CACODEMON:
        case ENEMY_BARON_OF_HELL:
        case ENEMY_HELL_RAZER:
        case ENEMY_MANCUBUS:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_CYBERDEMON:
        case ENEMY_TYRANT:
        case ENEMY_HARVESTER:
            return BEHAVIOR_PROJECTILE;
        
        
        case ENEMY_ZOMBIEMAN:
        case ENEMY_SHOTGUN_GUY:
        case ENEMY_NAZI:
        case ENEMY_SCIENTIST_ZOMBIE:
        case ENEMY_ZOMBIE_EARTH:
        case ENEMY_ZOMBIE_HELL:
        case ENEMY_MECHA_ZOMBIE:
        case ENEMY_GARGOYLE:
        case ENEMY_REVENANT:
            return BEHAVIOR_HITSCAN;
        
        
        case ENEMY_LOST_SOUL:
        case ENEMY_CUIBALL:
            return BEHAVIOR_KAMIKAZE;
        
        default:
            return BEHAVIOR_HITSCAN;
    }
}


e_enemy_movement get_enemy_movement(e_enemy_type type)
{
    switch (type)
    {
        
        case ENEMY_LOST_SOUL:
        case ENEMY_CACODEMON:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_GARGOYLE:
            return MOVEMENT_FLYING;
        
        
        default:
            return MOVEMENT_GROUND;
    }
}


double get_enemy_speed(e_enemy_type type)
{
    switch (type)
    {
        
        case ENEMY_LOST_SOUL:
        case ENEMY_WHIPLASH:
        case ENEMY_CUIBALL:
            return 0.08;
        
        
        case ENEMY_DEMON:
        case ENEMY_GARGOYLE:
        case ENEMY_ZOMBIE_EARTH:
            return 0.06;
        
        
        case ENEMY_DOOM_IMP:
        case ENEMY_CACODEMON:
        case ENEMY_HARVESTER:
        case ENEMY_REVENANT:
        case ENEMY_ZOMBIEMAN:
        case ENEMY_SHOTGUN_GUY:
        case ENEMY_NAZI:
        case ENEMY_MECHA_ZOMBIE:
        case ENEMY_ZOMBIE_HELL:
            return 0.04;
        
        
        case ENEMY_BARON_OF_HELL:
        case ENEMY_HELL_RAZER:
        case ENEMY_MANCUBUS:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_SCIENTIST_ZOMBIE:
            return 0.03;
        
        
        case ENEMY_CYBERDEMON:
        case ENEMY_TYRANT:
            return 0.02;
        
        default:
            return 0.04;
    }
}


double get_enemy_detection_range(e_enemy_type type)
{
    switch (type)
    {
        
        case ENEMY_CYBERDEMON:
        case ENEMY_TYRANT:
        case ENEMY_HELL_RAZER:
        case ENEMY_REVENANT:
            return 20.0;
        
        
        case ENEMY_BARON_OF_HELL:
        case ENEMY_CACODEMON:
        case ENEMY_MANCUBUS:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_HARVESTER:
            return 15.0;
        
        
        default:
            return 10.0;
    }
}


int get_enemy_damage(e_enemy_type type)
{
    switch (type)
    {
        
        case ENEMY_TYRANT:
            return 100;
        case ENEMY_CYBERDEMON:
            return 80;
        
        
        case ENEMY_BARON_OF_HELL:
            return 40;
        case ENEMY_MANCUBUS:
        case ENEMY_HELL_RAZER:
            return 35;
        
        
        case ENEMY_DEMON:
        case ENEMY_CACODEMON:
        case ENEMY_REVENANT:
        case ENEMY_WHIPLASH:
            return 25;
        case ENEMY_HARVESTER:
        case ENEMY_PAIN_ELEMENTAL:
            return 20;
        
        
        case ENEMY_DOOM_IMP:
        case ENEMY_SHOTGUN_GUY:
        case ENEMY_MECHA_ZOMBIE:
        case ENEMY_ZOMBIE_HELL:
            return 15;
        case ENEMY_LOST_SOUL:
        case ENEMY_GARGOYLE:
        case ENEMY_ZOMBIE_EARTH:
            return 10;
        case ENEMY_ZOMBIEMAN:
        case ENEMY_NAZI:
        case ENEMY_CUIBALL:
            return 8;
        case ENEMY_SCIENTIST_ZOMBIE:
            return 5;
        
        default:
            return 10;
    }
}


void update_enemy_ai(t_env *env, t_entity *entity)
{
    (void)env;
    
    
    
    if (!entity || entity->type != ENTITY_ENEMY || !entity->active)
        return;
    
    
    switch (entity->data.enemy.state)
    {
        case ENEMY_STATE_IDLE:
            
            break;
        
        case ENEMY_STATE_CHASE:
            
            break;
        
        case ENEMY_STATE_ATTACK:
            
            break;
        
        case ENEMY_STATE_HURT:
            
            break;
        
        case ENEMY_STATE_DYING:
            
            break;
        
        default:
            break;
    }
}


void update_all_enemies_ai(t_env *env)
{
    t_entity *current;
    
    if (!env || !env->entity_mgr.entities)
        return;
    
    current = env->entity_mgr.entities;
    
    while (current)
    {
        if (current->type == ENTITY_ENEMY && current->active)
        {
            update_enemy_ai(env, current);
        }
        current = current->next;
    }
}
