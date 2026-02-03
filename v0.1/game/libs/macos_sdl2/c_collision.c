#include "c_collision.h"
#include "types.h"
#include <math.h>

// PLAYER_RADIUS et PLAYER_HEIGHT definis dans typedefs.h

// ========== FONCTIONS UTILITAIRES ==========

// Calcule la distance d'un point a une ligne
double C_pointToLineDistance(vec2_t point, vec2_t line_a, vec2_t line_b)
{
    double A = point.x - line_a.x;
    double B = point.y - line_a.y;
    double C = line_b.x - line_a.x;
    double D = line_b.y - line_a.y;

    double dot = A * C + B * D;
    double len_sq  = C * C + D * D;

    if (len_sq == 0)
        return (sqrt(A * A + B * B));
    
    double param = dot / len_sq;

    vec2_t closest;
    if (param < 0)
        closest = line_a;
    else if (param > 1)
        closest = line_b;
    else {
        closest.x = line_a.x + param * C;
        closest.y = line_a.y + param * D;
    }

    double dx = point.x - closest.x;
    double dy = point.y - closest.y;

    return (sqrt(dx * dx + dy * dy));
}

// ========== FONCTIONS DE COLLISION 3D CYLINDRE-MUR ==========

// Cree un cylindre representant le joueur
player_cylinder_t C_CreatePlayerCylinder(player_t *player)
{
    player_cylinder_t cylinder;
    cylinder.center = player->position;
    cylinder.z_bottom = player->z;
    cylinder.z_top = player->z + PLAYER_HEIGHT;
    cylinder.radius = PLAYER_RADIUS;
    
    printf("Player cylinder: center(%.1f,%.1f) z[%.1f-%.1f] radius=%.1f\n", 
           cylinder.center.x, cylinder.center.y, cylinder.z_bottom, cylinder.z_top, cylinder.radius);
    
    return cylinder;
}

// Cree un mur vertical a partir d'un wall_t et de son secteur
wall_vertical_t C_CreateWallVertical(wall_t *wall, sector_t *sector)
{
    wall_vertical_t wall_vert;
    wall_vert.a = wall->a;
    wall_vert.b = wall->b;
    wall_vert.z_floor = (double)sector->elevation;
    wall_vert.z_ceil = (double)sector->elevation + (double)sector->height;
    
    printf("Wall vertical: floor=%.1f, ceil=%.1f (elevation=%d, height=%d)\n", 
           wall_vert.z_floor, wall_vert.z_ceil, sector->elevation, sector->height);
    
    return wall_vert;
}

// Verifie la collision 2D entre cylindre et mur (projection XY)
bool C_CheckCylinderWallCollision2D(player_cylinder_t cylinder, wall_vertical_t wall)
{
    double distance = C_pointToLineDistance(cylinder.center, wall.a, wall.b);
    bool collision = (distance < cylinder.radius);
    
    if (collision)
    {
        printf("XY collision: distance=%.1f < radius=%.1f\n", distance, cylinder.radius);
    }
    
    return collision;
}

// Verifie la collision en hauteur entre cylindre et mur
bool C_CheckCylinderWallCollisionHeight(player_cylinder_t cylinder, wall_vertical_t wall)
{
    // Le cylindre chevauche-t-il la hauteur du mur ?
    bool height_overlap = !(cylinder.z_top <= wall.z_floor || cylinder.z_bottom >= wall.z_ceil);
    
    if (height_overlap)
    {
        printf("Height collision: player[%.1f-%.1f] overlaps wall[%.1f-%.1f]\n", 
               cylinder.z_bottom, cylinder.z_top, wall.z_floor, wall.z_ceil);
    }
    else
    {
        printf("No height collision: player[%.1f-%.1f] vs wall[%.1f-%.1f]\n", 
               cylinder.z_bottom, cylinder.z_top, wall.z_floor, wall.z_ceil);
    }
    
    return height_overlap;
}

// Verifie la collision 3D complete (XY + hauteur)
bool C_CheckCylinderWallCollision3D(player_cylinder_t cylinder, wall_vertical_t wall)
{
    bool xy_collision = C_CheckCylinderWallCollision2D(cylinder, wall);
    bool height_collision = C_CheckCylinderWallCollisionHeight(cylinder, wall);
    
    bool full_collision = xy_collision && height_collision;
    
    printf("3D collision result: XY=%s, Height=%s => Full=%s\n", 
           xy_collision ? "YES" : "NO", 
           height_collision ? "YES" : "NO", 
           full_collision ? "BLOCKED" : "PASS");
    
    return full_collision;
}

// Fonction de debug pour tester la logique de chevauchement vertical
void C_DebugHeightOverlap(double player_bottom, double player_top, double wall_bottom, double wall_top)
{
    printf("=== DEBUG HEIGHT OVERLAP ===\n");
    printf("Player: [%.1f - %.1f] (height=%.1f)\n", player_bottom, player_top, player_top - player_bottom);
    printf("Wall:   [%.1f - %.1f] (height=%.1f)\n", wall_bottom, wall_top, wall_top - wall_bottom);
    
    bool overlap = (player_top > wall_bottom && player_bottom < wall_top);
    bool player_above = (player_bottom >= wall_top);
    bool player_below = (player_top <= wall_bottom);
    
    printf("Conditions:\n");
    printf("- player_top (%.1f) > wall_bottom (%.1f) = %s\n", player_top, wall_bottom, (player_top > wall_bottom) ? "TRUE" : "FALSE");
    printf("- player_bottom (%.1f) < wall_top (%.1f) = %s\n", player_bottom, wall_top, (player_bottom < wall_top) ? "TRUE" : "FALSE");
    printf("- Overlap = %s\n", overlap ? "TRUE (COLLISION)" : "FALSE (NO COLLISION)");
    printf("- Player above wall = %s\n", player_above ? "TRUE" : "FALSE");
    printf("- Player below wall = %s\n", player_below ? "TRUE" : "FALSE");
    printf("============================\n");
}

// ========== FONCTIONS EXISTANTES ==========

double C_PointSide(vec2_t point, vec2_t line_a, vec2_t line_b)
{
    return (line_b.x - line_a.x) * (point.y - line_a.y) - (line_b.y - line_a.y) * (point.x - line_a.x);
}

sector_t* C_GetPlayerSector(player_t *player, sector_t *sectors, int num_sector)
{
    for (int i = 0; i < num_sector; i++)
    {
        sector_t *sector = &sectors[i];

        bool inside = true;
        for (int j = 0; j < sector->num_walls; j++)
        {
            wall_t *wall = &sector->walls[j];
            double side = C_PointSide(player->position, wall->a, wall->b);
            if (side > 0)
            {
                inside = false;
                break;
            }
        }
        if (inside)
        {
            printf("Player in sector %d\n", i);
            return (sector);
        }
    }
    printf("Player not in any sector!\n");
    return (NULL);
}

// Trouve le point le plus proche sur un segment
vec2_t C_ClosestPointOnLine(vec2_t point, vec2_t line_a, vec2_t line_b)
{
    double A = point.x - line_a.x;
    double B = point.y - line_a.y;
    double C = line_b.x - line_a.x;
    double D = line_b.y - line_a.y;

    double dot = A * C + B * D;
    double len_sq = C * C + D * D;

    if (len_sq == 0)
        return line_a;
    
    double param = dot / len_sq;

    vec2_t closest;
    if (param < 0)
        closest = line_a;
    else if (param > 1)
        closest = line_b;
    else {
        closest.x = line_a.x + param * C;
        closest.y = line_a.y + param * D;
    }

    return closest;
}

// Calcule la direction de glissement le long d'un mur
vec2_t C_CalculateSlideDirection(vec2_t player_pos, vec2_t desired_pos, wall_t *wall, double player_radius)
{
    // Point le plus proche sur le mur
    vec2_t closest_on_wall = C_ClosestPointOnLine(player_pos, wall->a, wall->b);
    
    // Direction du mur (normalisee)
    double wall_dx = wall->b.x - wall->a.x;
    double wall_dy = wall->b.y - wall->a.y;
    double wall_length = sqrt(wall_dx * wall_dx + wall_dy * wall_dy);
    
    if (wall_length == 0)
        return desired_pos; // Mur de longueur nulle
    
    // Direction unitaire du mur
    vec2_t wall_dir = {wall_dx / wall_length, wall_dy / wall_length};
    
    // Direction perpendiculaire au mur (vers l'exterieur)
    vec2_t wall_normal = {-wall_dir.y, wall_dir.x};
    
    // Verifier si on est du bon côte du mur
    vec2_t to_player = {player_pos.x - closest_on_wall.x, player_pos.y - closest_on_wall.y};
    double dot_product = to_player.x * wall_normal.x + to_player.y * wall_normal.y;
    
    if (dot_product < 0)
    {
        wall_normal.x = -wall_normal.x;
        wall_normal.y = -wall_normal.y;
    }
    
    // Position glissee le long du mur
    vec2_t slide_pos = {closest_on_wall.x + wall_normal.x * player_radius, 
                        closest_on_wall.y + wall_normal.y * player_radius};
    
    // Direction de glissement (parallele au mur)
    vec2_t slide_direction = {wall_dir.x, wall_dir.y};
    
    // Projeter le mouvement desire sur la direction de glissement
    vec2_t desired_movement = {desired_pos.x - player_pos.x, desired_pos.y - player_pos.y};
    double slide_amount = desired_movement.x * slide_direction.x + desired_movement.y * slide_direction.y;
    
    // Position finale glissee
    vec2_t final_slide_pos = {
        slide_pos.x + slide_direction.x * slide_amount,
        slide_pos.y + slide_direction.y * slide_amount
    };
    
    printf("Slide: desired(%.1f,%.1f) -> slide(%.1f,%.1f)\n", 
           desired_pos.x, desired_pos.y, final_slide_pos.x, final_slide_pos.y);
    
    return final_slide_pos;
}

// Collision 2D classique
bool C_CheckWallCollision(vec2_t player_pos, double player_radius, wall_t *wall)
{
    double distance = C_pointToLineDistance(player_pos, wall->a, wall->b);
    return (distance < player_radius);
}

// Test de collision 3D complete (position XY + hauteur Z)
collision_result_t C_TestPlayer3DCollision(player_t *player, vec2_t new_position, double new_z, sector_t *sectors, int num_sector)
{
    collision_result_t rsl;
    rsl.new_position = new_position;
    rsl.collision_detected = false;
    rsl.colliding_wall = NULL;
    rsl.height_collision = false;
    rsl.xy_collision = false;
    rsl.new_sector = player->current_sector;
    
    sector_t *current = player->current_sector;
    if (!current) return rsl;

    player_cylinder_t cylinder = C_CreatePlayerCylinder(player);
    cylinder.center = new_position; // Test at new position
    cylinder.z_bottom = new_z;
    cylinder.z_top = new_z + PLAYER_HEIGHT;

    // Check walls in current sector
    for (int i = 0; i < current->num_walls; i++)
    {
        wall_t *wall = &current->walls[i];
        wall_vertical_t wall_vert = C_CreateWallVertical(wall, current);
        
        // Check 2D collision
        if (C_CheckCylinderWallCollision2D(cylinder, wall_vert))
        {
            // Check if it's a portal
            if (wall->is_portal)
            {
                // Find neighbor sector
                sector_t *neighbor = NULL;
                for(int j=0; j<num_sector; j++) {
                    if (sectors[j].id == wall->neighbor_sector_id) {
                        neighbor = &sectors[j];
                        break;
                    }
                }
                
                if (neighbor)
                {
                    // Check if we fit in the opening (portal height)
                    // Portal opening is intersection of current and neighbor heights
                    double portal_floor = fmax(current->elevation, neighbor->elevation);
                    double portal_ceil = fmin(current->elevation + current->height, neighbor->elevation + neighbor->height);
                    
                    // Check if player fits vertically
                    // Allow stepping up if the floor difference is small (e.g. 20 units)
                    double max_step = 25.0;
                    bool can_step_up = (portal_floor <= cylinder.z_bottom + max_step);
                    
                    bool fits = can_step_up && (cylinder.z_top <= portal_ceil);
                    
                    if (fits)
                    {
                        // We can pass!
                        // Check if we actually crossed the line to update sector
                        // Simple check: center point side
                        double side_old = C_PointSide(player->position, wall->a, wall->b);
                        double side_new = C_PointSide(new_position, wall->a, wall->b);
                        
                        // If we crossed (signs different or zero)
                        // Note: C_PointSide > 0 means inside (usually), depends on winding.
                        // Assuming standard winding where > 0 is inside.
                        // If we go from inside to outside, we cross.
                        
                        // Actually, simpler: if we are "inside" the wall collision radius but allowed to pass,
                        // we might be transitioning.
                        // Let's just update the sector if we are closer to the neighbor center? No.
                        // Let's update if we cross the portal line.
                        
                        if ((side_old > 0 && side_new <= 0) || (side_old <= 0 && side_new > 0))
                        {
                            rsl.new_sector = neighbor;
                            printf("Portal crossed! Entering sector %d\n", neighbor->id);
                        }
                        continue; // No collision, allow movement
                    }
                    else
                    {
                        // Blocked by height (step too high or ceiling too low)
                        rsl.collision_detected = true;
                        rsl.height_collision = true;
                        rsl.colliding_wall = wall;
                        // Slide
                        rsl.new_position = C_CalculateSlideDirection(player->position, new_position, wall, PLAYER_RADIUS);
                    }
                }
            }
            else
            {
                // Solid wall
                rsl.collision_detected = true;
                rsl.xy_collision = true;
                rsl.colliding_wall = wall;
                // Slide
                rsl.new_position = C_CalculateSlideDirection(player->position, new_position, wall, PLAYER_RADIUS);
            }
        }
    }
    
    return rsl;
}

// Test de mouvement 3D simplifie et fonctionnel
collision_result_t C_TestPlayerMovement3D(player_t *player, vec2_t new_pos, sector_t *sectors, int num_sector)
{
    // Utiliser la fonction complete avec la hauteur actuelle du joueur
    return C_TestPlayer3DCollision(player, new_pos, player->z, sectors, num_sector);
}

void C_ApplyCollision(player_t *player, vec2_t attempted_position, sector_t *sectors, int num_sector)
{
    collision_result_t test = C_TestPlayerMovement3D(player, attempted_position, sectors, num_sector);
    player->position = test.new_position;
    if (test.new_sector != player->current_sector)
    {
        player->current_sector = test.new_sector;
    }
}

// Applique un mouvement 3D complet (XY + Z) avec collision
void C_Apply3DMovement(player_t *player, vec2_t new_position, double new_z, sector_t *sectors, int num_sector)
{
    collision_result_t test = C_TestPlayer3DCollision(player, new_position, new_z, sectors, num_sector);
    
    player->position = test.new_position;
    player->z = new_z; // TODO: Check floor/ceiling collision for Z
    
    if (test.new_sector != player->current_sector)
    {
        player->current_sector = test.new_sector;
        // Adjust Z if needed (e.g. falling) - handled by physics update
    }
    
    if (test.collision_detected)
    {
        // printf("Collision handled. Sliding to: (%.1f, %.1f)\n", player->position.x, player->position.y);
    }
}

// Fonction de debug pour verifier la position de spawn
void C_CheckSpawnPosition(player_t *player, sector_t *sectors, int num_sector)
{
    printf("=== SPAWN POSITION CHECK ===\n");
    printf("Player spawn at: (%.1f, %.1f)\n", player->position.x, player->position.y);
    
    collision_result_t test = C_TestPlayerMovement3D(player, player->position, sectors, num_sector);
    
    if (test.collision_detected)
    {
        printf("WARNING: Player spawns inside a wall!\n");
        printf("Colliding wall: (%.1f,%.1f) to (%.1f,%.1f)\n", 
               test.colliding_wall->a.x, test.colliding_wall->a.y,
               test.colliding_wall->b.x, test.colliding_wall->b.y);
    }
    else
    {
        printf("Spawn position is valid\n");
    }
    printf("===========================\n");
}

