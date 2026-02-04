#include "entities.h"
#include "enemy_types.h"
#include "bmp_parser.h"
#include "env.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>

// Chargement des sprites pour tous les ennemis
// Basé sur les dossiers disponibles dans assets/sprites_organized/monsters/

// Chemins des sprites ennemis
static const char *enemy_sprite_paths[] = {
    // ID 100 - Baron of Hell
    "assets/sprites_organized/monsters/BaronOfHell/sprite_sheet.bmp",
    // ID 101 - Cacodemon
    "assets/sprites_organized/monsters/Cacodemon/sprite_sheet.bmp",
    // ID 102 - Cuiball
    "assets/sprites_organized/monsters/Cuiball/sprite_sheet.bmp",
    // ID 103 - Cyberdemon
    "assets/sprites_organized/monsters/Cyberdemon/sprite_sheet.bmp",
    // ID 104 - Demon
    "assets/sprites_organized/monsters/Demon/sprite_sheet.bmp",
    // ID 105 - Doom Imp
    "assets/sprites_organized/monsters/DoomImp/sprite_sheet.bmp",
    // ID 106 - Gargoyle
    "assets/sprites_organized/monsters/Gargoyle/sprite_sheet.bmp",
    // ID 107 - Harvester
    "assets/sprites_organized/monsters/Harvester/sprite_sheet.bmp",
    // ID 108 - Hell Razer
    "assets/sprites_organized/monsters/HellRazer/sprite_sheet.bmp",
    // ID 109 - Lost Soul
    "assets/sprites_organized/monsters/Lost Soul/sprite_sheet.bmp",
    // ID 110 - Mancubus
    "assets/sprites_organized/monsters/Mancubus/sprite_sheet.bmp",
    // ID 111 - Mecha Zombie
    "assets/sprites_organized/monsters/Mecha Zombie/sprite_sheet.bmp",
    // ID 112 - Nazi
    "assets/sprites_organized/monsters/Nazi/sprite_sheet.bmp",
    // ID 113 - Pain Elemental
    "assets/sprites_organized/monsters/Pain Elemental/sprite_sheet.bmp",
    // ID 114 - Revenant
    "assets/sprites_organized/monsters/Revenant/sprite_sheet.bmp",
    // ID 115 - Scientist Zombie
    "assets/sprites_organized/monsters/Scientist Zombie/sprite_sheet.bmp",
    // ID 116 - Shotgun Guy
    "assets/sprites_organized/monsters/Shotgunguy/sprite_sheet.bmp",
    // ID 117 - Tyrant
    "assets/sprites_organized/monsters/Tyrant/sprite_sheet.bmp",
    // ID 118 - Whiplash
    "assets/sprites_organized/monsters/Whiplash/sprite_sheet.bmp",
    // ID 119 - Zombie Earth
    "assets/sprites_organized/monsters/Zombie Earth/sprite_sheet.bmp",
    // ID 120 - Zombie Hell
    "assets/sprites_organized/monsters/Zombie Hell/sprite_sheet.bmp",
    // ID 121 - Zombieman
    "assets/sprites_organized/monsters/Zombieman/sprite_sheet.bmp",
};

#define NUM_ENEMY_SPRITES (sizeof(enemy_sprite_paths) / sizeof(enemy_sprite_paths[0]))

// Fonction pour charger tous les sprites d'ennemis
int load_enemy_sprites(t_env *env)
{
    int i;
    int loaded_count;
    
    printf("Loading enemy sprites...\n");
    
    // Allouer le tableau de textures d'ennemis
    env->num_enemy_textures = NUM_ENEMY_SPRITES;
    env->enemy_textures = malloc(sizeof(t_texture) * env->num_enemy_textures);
    
    if (!env->enemy_textures)
    {
        printf("Erreur: allocation mémoire pour enemy_textures\n");
        return -1;
    }
    
    // Initialiser toutes les textures à NULL
    for (i = 0; i < env->num_enemy_textures; i++)
    {
        env->enemy_textures[i].pixels = NULL;
        env->enemy_textures[i].width = 0;
        env->enemy_textures[i].height = 0;
    }
    
    // Charger les sprites
    loaded_count = 0;
    for (i = 0; i < (int)NUM_ENEMY_SPRITES; i++)
    {
        VERBOSE_LOG("  Loading %s\n", enemy_sprite_paths[i]);
        
        if (load_bmp_texture(enemy_sprite_paths[i], &env->enemy_textures[i]) == 0)
        {
            loaded_count++;
        }
        else
        {
            DEBUG_LOG("Warning: Failed to load enemy sprite: %s\n", 
                     enemy_sprite_paths[i]);
            // Continuer quand même, certains sprites peuvent être manquants
        }
    }
    
    printf("✅ Loaded %d/%d enemy sprite sheets\n", loaded_count, (int)NUM_ENEMY_SPRITES);
    return 0;
}

// Fonction pour obtenir le nom d'un ennemi à partir de son type
const char *get_enemy_name(e_enemy_type type)
{
    static const char *enemy_names[] = {
        "Zombieman",
        "Shotgun Guy",
        "Doom Imp",
        "Demon",
        "Lost Soul",
        "Cacodemon",
        "Baron of Hell",
        "Hell Razer",
        "Mancubus",
        "Revenant",
        "Pain Elemental",
        "Cyberdemon",
        "Tyrant",
        "Harvester",
        "Whiplash",
        "Gargoyle",
        "Cuiball",
        "Nazi",
        "Scientist Zombie",
        "Zombie Earth",
        "Zombie Hell",
        "Mecha Zombie"
    };
    
    if (type >= 0 && type < ENEMY_TYPE_COUNT)
        return enemy_names[type];
    
    return "Unknown Enemy";
}

// Fonction pour obtenir le sprite ID à partir du type d'ennemi
// Retourne l'ID dans la plage 100-121
int get_enemy_sprite_id(e_enemy_type type)
{
    // Mapping entre le type d'ennemi et son sprite ID (100-121)
    // Ces IDs correspondent aux indices dans enemy_textures (ID - 100)
    static const int sprite_ids[] = {
        121,  // ENEMY_ZOMBIEMAN -> index 21
        116,  // ENEMY_SHOTGUN_GUY -> index 16
        105,  // ENEMY_DOOM_IMP -> index 5
        104,  // ENEMY_DEMON -> index 4
        109,  // ENEMY_LOST_SOUL -> index 9
        101,  // ENEMY_CACODEMON -> index 1
        100,  // ENEMY_BARON_OF_HELL -> index 0
        108,  // ENEMY_HELL_RAZER -> index 8
        110,  // ENEMY_MANCUBUS -> index 10
        114,  // ENEMY_REVENANT -> index 14
        113,  // ENEMY_PAIN_ELEMENTAL -> index 13
        103,  // ENEMY_CYBERDEMON -> index 3
        117,  // ENEMY_TYRANT -> index 17
        107,  // ENEMY_HARVESTER -> index 7
        118,  // ENEMY_WHIPLASH -> index 18
        106,  // ENEMY_GARGOYLE -> index 6
        102,  // ENEMY_CUIBALL -> index 2
        112,  // ENEMY_NAZI -> index 12
        115,  // ENEMY_SCIENTIST_ZOMBIE -> index 15
        119,  // ENEMY_ZOMBIE_EARTH -> index 19
        120,  // ENEMY_ZOMBIE_HELL -> index 20
        111,  // ENEMY_MECHA_ZOMBIE -> index 11
    };
    
    if (type >= 0 && type < ENEMY_TYPE_COUNT)
        return sprite_ids[type];
    
    return 100;  // Valeur par défaut
}
