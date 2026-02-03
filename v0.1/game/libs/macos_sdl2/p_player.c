#include "p_player.h"
#include <math.h>

player_t P_Init(double x, double y, double z, double angle)
{
    player_t player;
    player.dir_angle = angle;
    player.position.x = x;
    player.position.y = y;
    player.z = z;
    player.z_velocity = 0;
    player.pitch = 0;
    player.health = 100;
    player.armor = 0;
    player.is_jumping = false;
    player.is_falling = false;
    player.ground_height = 0;

    return player;
}

void P_Update(player_t *player, double delta_time)
{
    // Mise a jour de la physique
    P_UpdatePhysics(player, delta_time);
    
    // Limites de la hauteur de vue
    if (player->pitch > M_PI/3) player->pitch = M_PI/3;
    if (player->pitch < -M_PI/3) player->pitch = -M_PI/3;
    
    // Normalisation de l'angle de vue horizontal
    while (player->dir_angle < 0) player->dir_angle += 2*M_PI;
    while (player->dir_angle >= 2*M_PI) player->dir_angle -= 2*M_PI;
}

void P_Jump(player_t *player)
{
    if (!player->is_jumping && !player->is_falling)
    {
        player->z_velocity = PLAYER_JUMP_SPEED;
        player->is_jumping = true;
    }
}

void P_UpdatePhysics(player_t *player, double delta_time)
{
    // Mise a jour de la position verticale
    player->z += player->z_velocity * delta_time;
    
    // Application de la gravite
    player->z_velocity -= PLAYER_GRAVITY * delta_time;
    
    // Detection du sol
    double ground = P_GetGroundHeight(player);
    
    // Collision avec le sol
    if (player->z <= ground)
    {
        player->z = ground;
        player->z_velocity = 0;
        player->is_jumping = false;
        player->is_falling = false;
    }
    else
    {
        player->is_falling = true;
    }
}

double P_GetGroundHeight(player_t *player)
{
    if (player->current_sector)
    {
        return (double)player->current_sector->elevation;
    }
    return 0.0;
}

void P_TakeDamage(player_t *player, int damage)
{
    // L'armure absorbe 2/3 des degâts
    if (player->armor > 0)
    {
        int armor_damage = (damage * 2) / 3;
        int health_damage = damage - armor_damage;
        
        player->armor -= armor_damage;
        if (player->armor < 0)
        {
            health_damage -= player->armor;
            player->armor = 0;
        }
        
        player->health -= health_damage;
    }
    else
    {
        player->health -= damage;
    }
    
    if (player->health < 0)
        player->health = 0;
}

void P_AddArmor(player_t *player, int armor)
{
    player->armor += armor;
    if (player->armor > 100)
        player->armor = 100;
}

void P_AddHealth(player_t *player, int health)
{
    player->health += health;
    if (player->health > 100)
        player->health = 100;
}