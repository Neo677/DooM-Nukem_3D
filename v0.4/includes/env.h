#ifndef ENV_H
# define ENV_H

# include "utils.h"
# include "texture.h"
# include "bmp_parser.h"
# include "sector.h"
# include "geometry.h"
# include "collision.h"
# include "debug.h"
# include "defines.h"

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define FOV 60.0
#define MOVE_SPEED 0.05
#define ROT_SPEED 0.03

// Forward declaration for entities
typedef struct s_entity t_entity;

// Entity manager structure
typedef struct {
    t_entity    *entities;
    int         count;
} t_entity_manager;

typedef struct s_sld {
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Texture     *texture;
    Uint32          *texture_pixels;
}           t_sdl;

// Player/Camera
typedef struct {
    t_v2    pos;            // Position 2D (x, y)
    double  angle;          // Angle de vue (radians)
    double  height;         // Hauteur des yeux (absolue, pour le rendu)
    double  velocity_z;     // Vitesse verticale
    int     is_falling;     // Flag chute
    int     current_sector; // Secteur actuel du joueur
} t_player;

// Map dynamique
typedef struct {
    int     **grid;         // Grille 2D dynamique
    int     width;          // Largeur de la map
    int     height;         // Hauteur de la map
} t_map;

// Ray hit info (pour DDA raycasting)
typedef struct s_ray_hit {
    double  distance;       // Distance perpendiculaire
    int     side;           // 0 = vertical (X), 1 = horizontal (Y)
    int     wall_type;      // Type de mur touche
    double  wall_x;         // Position exacte du hit (0.0-1.0)
    int     map_x;          // Coordonnees map
    int     map_y;
} t_ray_hit;

// Mode de rendu
typedef enum {
    MODE_3D,      // Vue FPS normale
    MODE_2D,      // Vue top-down
    MODE_BOTH     // Split-screen (avance)
} t_render_mode;

// Vue 2D
typedef struct {
    double  zoom;           // Pixels par unite (ex: 50.0)
    t_v2    offset;         // Decalage camera (pour pan)
    int     show_rays;      // Afficher les rayons ?
    int     show_grid;      // Afficher la grille ?
    int     show_minimap;   // Afficher minimap en overlay ?
} t_view_2d;


// NOUVEAU : Skybox Vertex (pour la box 3D)
typedef struct {
    // Coordonnées après rotation (camera space)
    double vx;
    double vz;
    
    // Coordonnées après clipping Z-near
    double clipped_vx1;
    double clipped_vz1;
    double clipped_vx2;
    double clipped_vz2;
    
    // Coordonnées écran projetées (X)
    double clipped_x1;
    double clipped_x2;
    double x;
    
    // Hauteurs plafond/sol (screen Y)
    double c1, c2;  // ceiling
    double f1, f2;  // floor
    
    // Facteurs de perspective
    double scale1;
    double scale2;
    
    // Ranges pour interpolation
    double xrange;
    double floor_range;
    double ceiling_range;
    double zrange;
    double zcomb;
    
    // Texture mapping helpers (perspective-correct)
    double x0z1, x1z0, xzrange;
    double y0z1, y1z0, yzrange;
    
    // Flags
    int draw;  // Ce segment est-il visible?
} t_skybox_vertex;

// NOUVEAU : Skybox
typedef struct {
    t_texture       *textures;      // Array of loaded skybox textures
    int             num_textures;   // Number of loaded textures
    int             current_id;     // ID of the currently active skybox
    int             enabled;        // 1 = skybox visible, 0 = black background
    double          offset;         // Optional: rotation offset
    
    // NOUVEAU : Vertices pour la box 3D (4 coins + 1 wrap)
    t_skybox_vertex vertices[5];    // [0-3] = 4 corners, [4] = wrap of [0]
    double          box_size;       // Taille de la boîte (ex: 10.0)
    int             computed;       // Flag: skybox precomputed this frame?
} t_skybox;

typedef struct {
    t_sdl       sdl;
    t_player    player;
    t_map       map;
    
    // NOUVEAU : Phase 3 Sectors
    t_map_sectors sector_map;
    
    int         w;              // Largeur fenêtre
    int         h;              // Hauteur fenêtre
    int         running;        // Boucle principale active
    
    // FPS et timing
    Uint32      frame_timer;
    Uint32      last_time;
    int         fps;
    int         fps_count;
    
    // Rendu 3D
    double      *zbuffer;       // Buffer de profondeur (largeur ecran)
    int         *ytop_buffer;   // Buffer clipping top (pre-alloue)
    int         *ybottom_buffer; // Buffer clipping bottom (pre-alloue)
    t_v2        *collision_buffer; // Buffer collision polygone (pre-alloue)
    int         collision_buffer_size; // Taille du buffer collision
    
    // Textures
    t_texture   *wall_textures;
    int         num_textures;
    
    // NOUVEAU : Textures de sol/plafond
    t_texture   floor_texture;      // Texture du sol
    t_texture   ceiling_texture;    // Texture du plafond
    
    // NOUVEAU : Skybox system
    t_skybox        skybox;
    
    // NOUVEAU : Mode 2D
    t_render_mode   render_mode;    // Mode actuel
    t_view_2d       view_2d;         // Parametres vue 2D
    
    // NOUVEAU : Capture de souris
    int             mouse_captured;  // 1 si souris capturee, 0 sinon

    t_entity_manager    entity_mgr;
} t_env;


// Fonctions existantes
int     init_sdl(t_env *env);
int     init_game(int ac, char **av);
void    clear_image(t_env *env, Uint32 color);
void    draw_line(t_point c1, t_point c2, t_env *env, Uint32 color);
void    draw_rectangle(t_env *env, t_rectangle r, t_point pos, t_point size);
void    draw_circle(t_circle circle, t_env *env);
void    draw_fps(t_env *env);
void    limit_fps(t_env *env, Uint32 frame_start);
void    game_loop(t_env *env);

// Menu
int     show_menu(t_env *env);

// Nouvelles fonctions Phase 2
void    init_map(t_env *env);
void    init_player(t_env *env);
void    handle_input(t_env *env);
void    render_3d(t_env *env);
void    render_2d(t_env *env);  // NOUVEAU
void    draw_minimap(t_env *env);  // NOUVEAU minimap overlay
void    render_floor_ceiling(t_env *env);  // NOUVEAU floor/ceiling
void    draw_wall_slice(t_env *env, int x, double wall_dist, int wall_type);

// DDA Raycasting optimise
t_ray_hit cast_ray_dda(t_env *env, double ray_angle);
void    draw_wall_slice_textured(t_env *env, int x, t_ray_hit *hit);

// Textures
int     init_textures(t_env *env);
void    free_textures(t_env *env);

// Sectors (Phase 3)
int     init_sectors(t_env *env);
void    free_sectors(t_env *env);
void    print_sector_info(t_sector *s);
int     find_sector(t_env *env, double x, double y);
void    player_move(t_env *env, double dx, double dy);
void    update_player_physics(t_env *env); // NOUVEAU: Gravite
double  get_sector_floor_height(t_env *env, int sector_id, double x, double y); // NOUVEAU: Pentes

// Bitmap font
void    draw_text(t_env *env, const char *text, int x, int y, Uint32 color);
void    draw_fps_on_screen(t_env *env);

// Skybox
int     init_skybox(t_env *env);
void    render_skybox(t_env *env);         // Legacy (cylindrique)
void    precompute_skybox(t_env *env);     // NOUVEAU: Precompute 3D box
void    draw_skybox_column(t_env *env, int x, int y1, int y2, double ray_angle); // NOUVEAU
void    switch_skybox(t_env *env, int id);
void    toggle_skybox(t_env *env);
void    free_skybox(t_env *env);

// Helpers
t_rectangle new_rectangle(Uint32 inside_color, Uint32 line_color, int filled, int line_size);
t_circle    new_circle(Uint32 line_color, Uint32 color, t_point center, int radius);

#endif 