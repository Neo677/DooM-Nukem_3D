#ifndef ENEMY_RENDER_H
# define ENEMY_RENDER_H

# include "entities.h"
# include "env.h"

// Fonctions de rendu des ennemis
void    render_enemies(t_env *env);
void    sort_enemies_by_distance(t_env *env);

// Fonctions de chargement des sprites
int     load_enemy_sprites(t_env *env);
const char  *get_enemy_name(e_enemy_type type);
int     get_enemy_sprite_id(e_enemy_type type);

// Fonctions d'initialisation des données
void    init_enemy_data(t_entity *entity, e_enemy_type type);

// Fonctions d'IA
void    update_enemy_ai(t_env *env, t_entity *entity);
void    update_all_enemies_ai(t_env *env);
e_enemy_behavior    get_enemy_behavior(e_enemy_type type);
e_enemy_movement    get_enemy_movement(e_enemy_type type);
double  get_enemy_speed(e_enemy_type type);
double  get_enemy_detection_range(e_enemy_type type);
int     get_enemy_damage(e_enemy_type type);

#endif // ENEMY_RENDER_H
