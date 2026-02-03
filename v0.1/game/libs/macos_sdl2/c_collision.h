#ifndef C_COLLISION_H
#define C_COLLISION_H

#include "types.h"
#include "p_player.h"
#include "r_renderer.h"

// Structure pour le resultat de collision 3D
typedef struct _collision_result
{
    vec2_t new_position;
    bool collision_detected;
    wall_t *colliding_wall;
    bool height_collision; // Nouvelle : collision en hauteur
    bool xy_collision;     // Nouvelle : collision en XY
    struct _sector *new_sector; // Nouveau secteur si transition
} collision_result_t;

// Structure pour modeliser le cylindre du joueur
typedef struct _player_cylinder
{
    vec2_t center;    // Position XY du centre
    double z_bottom;  // Hauteur du bas du cylindre
    double z_top;     // Hauteur du haut du cylindre
    double radius;    // Rayon du cylindre
} player_cylinder_t;

// Structure pour modeliser un mur vertical
typedef struct _wall_vertical
{
    vec2_t a, b;      // Points XY du segment
    double z_floor;   // Hauteur du sol du mur
    double z_ceil;    // Hauteur du plafond du mur
} wall_vertical_t;

// Fonctions de collision 3D cylindre-mur
player_cylinder_t C_CreatePlayerCylinder(player_t *player);
wall_vertical_t C_CreateWallVertical(wall_t *wall, sector_t *sector);
bool C_CheckCylinderWallCollision2D(player_cylinder_t cylinder, wall_vertical_t wall);
bool C_CheckCylinderWallCollisionHeight(player_cylinder_t cylinder, wall_vertical_t wall);
bool C_CheckCylinderWallCollision3D(player_cylinder_t cylinder, wall_vertical_t wall);
collision_result_t C_TestPlayer3DCollision(player_t *player, vec2_t new_position, double new_z, sector_t *sectors, int num_sectors);

// Fonctions de collision 2D classique avec glissement (conservees)
bool C_CheckWallCollision(vec2_t player_pos, double player_radius, wall_t *wall);
vec2_t C_ClosestPointOnLine(vec2_t point, vec2_t line_a, vec2_t line_b);
vec2_t C_CalculateSlideDirection(vec2_t player_pos, vec2_t desired_pos, wall_t *wall, double player_radius);
collision_result_t C_TestPlayerMovement3D(player_t *player, vec2_t new_position, sector_t *sectors, int num_sectors);
void C_ApplyCollision(player_t *player, vec2_t attempted_position, sector_t *sectors, int num_sector);
void C_Apply3DMovement(player_t *player, vec2_t new_position, double new_z, sector_t *sectors, int num_sector);
sector_t* C_GetPlayerSector(player_t *player, sector_t *sectors, int num_sector);

#endif