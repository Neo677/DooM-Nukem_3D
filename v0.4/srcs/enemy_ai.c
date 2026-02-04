#include "entities.h"
#include "enemy_types.h"
#include "env.h"
#include "defines.h"

// Système d'IA basique pour les ennemis
// À implémenter plus tard avec les comportements complets

// Comportement par défaut selon le type d'ennemi
e_enemy_behavior get_enemy_behavior(e_enemy_type type)
{
    switch (type)
    {
        // Ennemis de mêlée
        case ENEMY_DEMON:
        case ENEMY_WHIPLASH:
            return BEHAVIOR_MELEE;
        
        // Ennemis à projectiles
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
        
        // Ennemis hitscan (tir instantané)
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
        
        // Ennemis kamikaze
        case ENEMY_LOST_SOUL:
        case ENEMY_CUIBALL:
            return BEHAVIOR_KAMIKAZE;
        
        default:
            return BEHAVIOR_HITSCAN;
    }
}

// Type de mouvement selon l'ennemi
e_enemy_movement get_enemy_movement(e_enemy_type type)
{
    switch (type)
    {
        // Ennemis volants
        case ENEMY_LOST_SOUL:
        case ENEMY_CACODEMON:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_GARGOYLE:
            return MOVEMENT_FLYING;
        
        // Tous les autres sont au sol
        default:
            return MOVEMENT_GROUND;
    }
}

// Vitesse de déplacement selon le type
double get_enemy_speed(e_enemy_type type)
{
    switch (type)
    {
        // Très rapides
        case ENEMY_LOST_SOUL:
        case ENEMY_WHIPLASH:
        case ENEMY_CUIBALL:
            return 0.08;
        
        // Rapides
        case ENEMY_DEMON:
        case ENEMY_GARGOYLE:
        case ENEMY_ZOMBIE_EARTH:
            return 0.06;
        
        // Moyens
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
        
        // Lents
        case ENEMY_BARON_OF_HELL:
        case ENEMY_HELL_RAZER:
        case ENEMY_MANCUBUS:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_SCIENTIST_ZOMBIE:
            return 0.03;
        
        // Très lents (boss)
        case ENEMY_CYBERDEMON:
        case ENEMY_TYRANT:
            return 0.02;
        
        default:
            return 0.04;
    }
}

// Portée de détection du joueur
double get_enemy_detection_range(e_enemy_type type)
{
    switch (type)
    {
        // Longue portée (boss et snipers)
        case ENEMY_CYBERDEMON:
        case ENEMY_TYRANT:
        case ENEMY_HELL_RAZER:
        case ENEMY_REVENANT:
            return 20.0;
        
        // Moyenne portée
        case ENEMY_BARON_OF_HELL:
        case ENEMY_CACODEMON:
        case ENEMY_MANCUBUS:
        case ENEMY_PAIN_ELEMENTAL:
        case ENEMY_HARVESTER:
            return 15.0;
        
        // Courte portée
        default:
            return 10.0;
    }
}

// Dégâts d'attaque selon le type
int get_enemy_damage(e_enemy_type type)
{
    switch (type)
    {
        // Boss - dégâts très élevés
        case ENEMY_TYRANT:
            return 100;
        case ENEMY_CYBERDEMON:
            return 80;
        
        // Ennemis forts
        case ENEMY_BARON_OF_HELL:
            return 40;
        case ENEMY_MANCUBUS:
        case ENEMY_HELL_RAZER:
            return 35;
        
        // Ennemis moyens
        case ENEMY_DEMON:
        case ENEMY_CACODEMON:
        case ENEMY_REVENANT:
        case ENEMY_WHIPLASH:
            return 25;
        case ENEMY_HARVESTER:
        case ENEMY_PAIN_ELEMENTAL:
            return 20;
        
        // Ennemis faibles
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

// Mise à jour de l'IA d'un ennemi (à appeler chaque frame)
void update_enemy_ai(t_env *env, t_entity *entity)
{
    (void)env;
    // TODO: Implémenter l'IA complète
    // Pour l'instant, juste un placeholder
    
    if (!entity || entity->type != ENTITY_ENEMY || !entity->active)
        return;
    
    // Logique basique selon l'état
    switch (entity->data.enemy.state)
    {
        case ENEMY_STATE_IDLE:
            // Chercher le joueur
            break;
        
        case ENEMY_STATE_CHASE:
            // Se déplacer vers le joueur
            break;
        
        case ENEMY_STATE_ATTACK:
            // Attaquer le joueur
            break;
        
        case ENEMY_STATE_HURT:
            // Réaction aux dégâts
            break;
        
        case ENEMY_STATE_DYING:
            // Animation de mort
            break;
        
        default:
            break;
    }
}

// Mise à jour de toutes les IA ennemies
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
