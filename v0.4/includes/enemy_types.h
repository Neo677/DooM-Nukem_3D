#ifndef ENEMY_TYPES_H
# define ENEMY_TYPES_H

// Enumération de tous les types d'ennemis disponibles
// Basé sur les dossiers dans assets/sprites_organized/monsters/
typedef enum {
    ENEMY_ZOMBIEMAN,            // Zombieman classique (faible, projectile)
    ENEMY_SHOTGUN_GUY,          // Shotgun Guy (moyen, shotgun)
    ENEMY_DOOM_IMP,             // Imp (faible, projectile)
    ENEMY_DEMON,                // Demon/Pinky (moyen, mêlée)
    ENEMY_LOST_SOUL,            // Lost Soul (aérien, charge)
    ENEMY_CACODEMON,            // Cacodemon (aérien, projectile)
    ENEMY_BARON_OF_HELL,        // Baron of Hell (fort, projectile)
    ENEMY_HELL_RAZER,           // Hell Razer (moyen, laser)
    ENEMY_MANCUBUS,             // Mancubus (fort, flammes)
    ENEMY_REVENANT,             // Revenant (moyen, missiles)
    ENEMY_PAIN_ELEMENTAL,       // Pain Elemental (aérien, spawner)
    ENEMY_CYBERDEMON,           // Cyberdemon (boss, rockets)
    ENEMY_TYRANT,               // Tyrant (boss+, super projectiles)
    ENEMY_HARVESTER,            // Harvester (moyen, projectiles)
    ENEMY_WHIPLASH,             // Whiplash (rapide, mêlée)
    ENEMY_GARGOYLE,             // Gargoyle (aérien, projectile)
    ENEMY_CUIBALL,              // Cube Ball (petit, rapide)
    ENEMY_NAZI,                 // Nazi (faible, pistolet)
    ENEMY_SCIENTIST_ZOMBIE,     // Scientist Zombie (très faible)
    ENEMY_ZOMBIE_EARTH,         // Zombie Earth (moyen)
    ENEMY_ZOMBIE_HELL,          // Zombie Hell (moyen+)
    ENEMY_MECHA_ZOMBIE,         // Mecha Zombie (moyen, armure)
    
    ENEMY_TYPE_COUNT            // Nombre total de types
} e_enemy_type;

// États des ennemis
typedef enum {
    ENEMY_STATE_IDLE,           // Au repos
    ENEMY_STATE_PATROL,         // Patrouille
    ENEMY_STATE_ALERT,          // Alerte (a vu/entendu le joueur)
    ENEMY_STATE_CHASE,          // Poursuite
    ENEMY_STATE_ATTACK,         // Attaque
    ENEMY_STATE_HURT,           // A pris des dégâts
    ENEMY_STATE_DYING,          // En train de mourir
    ENEMY_STATE_DEAD            // Mort
} e_enemy_state;

// Comportements d'attaque
typedef enum {
    BEHAVIOR_MELEE,             // Attaque au corps à corps
    BEHAVIOR_PROJECTILE,        // Tire des projectiles
    BEHAVIOR_HITSCAN,           // Attaque instantanée (hitscan)
    BEHAVIOR_KAMIKAZE,          // Charge et explose
    BEHAVIOR_SPAWNER            // Spawn d'autres entités
} e_enemy_behavior;

// Types de mouvement
typedef enum {
    MOVEMENT_GROUND,            // Se déplace au sol
    MOVEMENT_FLYING,            // Vol (peut changer de hauteur)
    MOVEMENT_FLOATING           // Flotte (hauteur fixe)
} e_enemy_movement;

#endif // ENEMY_TYPES_H
