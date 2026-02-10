#include "entities.h"
#include "enemy_types.h"
#include "bmp_parser.h"
#include "env.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>





static const char *enemy_sprite_paths[] = {
    
    "assets/sprites_organized/monsters/BaronOfHell/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Cacodemon/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Cuiball/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Cyberdemon/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Demon/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/DoomImp/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Gargoyle/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Harvester/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/HellRazer/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Lost Soul/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Mancubus/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Mecha Zombie/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Nazi/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Pain Elemental/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Revenant/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Scientist Zombie/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Shotgunguy/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Tyrant/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Whiplash/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Zombie Earth/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Zombie Hell/sprite_sheet.bmp",
    
    "assets/sprites_organized/monsters/Zombieman/sprite_sheet.bmp",
};

#define NUM_ENEMY_SPRITES (sizeof(enemy_sprite_paths) / sizeof(enemy_sprite_paths[0]))


int load_enemy_sprites(t_env *env)
{
    int i;
    int loaded_count;
    
    printf("Loading enemy sprites...\n");
    
    
    env->num_enemy_textures = NUM_ENEMY_SPRITES;
    env->enemy_textures = malloc(sizeof(t_texture) * env->num_enemy_textures);
    
    if (!env->enemy_textures)
    {
        printf("Erreur: allocation mémoire pour enemy_textures\n");
        return -1;
    }
    
    
    for (i = 0; i < env->num_enemy_textures; i++)
    {
        env->enemy_textures[i].pixels = NULL;
        env->enemy_textures[i].width = 0;
        env->enemy_textures[i].height = 0;
    }
    
    
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
            
        }
    }
    
    printf("✅ Loaded %d/%d enemy sprite sheets\n", loaded_count, (int)NUM_ENEMY_SPRITES);
    return 0;
}


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



int get_enemy_sprite_id(e_enemy_type type)
{
    
    
    static const int sprite_ids[] = {
        121,  
        116,  
        105,  
        104,  
        109,  
        101,  
        100,  
        108,  
        110,  
        114,  
        113,  
        103,  
        117,  
        107,  
        118,  
        106,  
        102,  
        112,  
        115,  
        119,  
        120,  
        111,  
    };
    
    if (type >= 0 && type < ENEMY_TYPE_COUNT)
        return sprite_ids[type];
    
    return 100;  
}
