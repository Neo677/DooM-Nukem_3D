#ifndef P_PLAYER_H
#define P_PLAYER_H

#include "types.h"

#define PLAYER_RADIUS 5.0
#define PLAYER_HEIGHT 32.0
#define PLAYER_EYE_HEIGHT 28.0
#define PLAYER_SPEED 3.0
#define PLAYER_TURN_SPEED 0.04
#define PLAYER_JUMP_SPEED 8.0
#define PLAYER_GRAVITY 0.6
#define PLAYER_MAX_SLOPE 0.7 // Maximum slope the player can climb (in radians)

typedef struct _player
{
    vec2_t position;     // Position 2D (x,y)
    double z;            // Hauteur
    double z_velocity;   // Vitesse verticale
    double dir_angle;    // Angle de vue horizontal
    double pitch;        // Angle de vue vertical
    int health;         // Points de vie
    int armor;          // Points d'armure
    bool is_jumping;    // etat de saut
    bool is_falling;    // etat de chute
    double ground_height; // Hauteur du sol sous le joueur
    struct _sector *current_sector; // Secteur actuel
} player_t;

player_t P_Init(double x, double y, double z, double angle);
void P_Update(player_t *player, double delta_time);
void P_Jump(player_t *player);
void P_UpdatePhysics(player_t *player, double delta_time);
double P_GetGroundHeight(player_t *player);
void P_TakeDamage(player_t *player, int damage);
void P_AddArmor(player_t *player, int armor);
void P_AddHealth(player_t *player, int health);

#endif /* P_PLAYER_H */
