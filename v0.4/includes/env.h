#ifndef ENV_H
# define ENV_H

# include "utils.h"
# include "texture.h"
# include "bmp_parser.h"

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define FOV 60.0
#define MOVE_SPEED 0.05
#define ROT_SPEED 0.03

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
    double  height;         // Hauteur des yeux (pour le rendu)
} t_player;

// Map simple (grid)
typedef struct {
    int     grid[MAP_HEIGHT][MAP_WIDTH];  // 0 = vide, 1+ = mur
} t_map;

// Ray hit info (pour DDA raycasting)
typedef struct s_ray_hit {
    double  distance;       // Distance perpendiculaire
    int     side;           // 0 = vertical (X), 1 = horizontal (Y)
    int     wall_type;      // Type de mur touché
    double  wall_x;         // Position exacte du hit (0.0-1.0)
    int     map_x;          // Coordonnées map
    int     map_y;
} t_ray_hit;


typedef struct {
    t_sdl       sdl;
    t_player    player;
    t_map       map;
    
    int         w;              // Largeur fenêtre
    int         h;              // Hauteur fenêtre
    int         running;        // Boucle principale active
    
    // FPS et timing
    Uint32      frame_timer;
    Uint32      last_time;
    int         fps;
    int         fps_count;
    
    // Rendu 3D
    double      *zbuffer;       // Buffer de profondeur (largeur écran)
    
    // Textures
    t_texture   *wall_textures;
    int         num_textures;
    
    // NOUVEAU : Textures de sol/plafond
    t_texture   floor_texture;      // Texture du sol
    t_texture   ceiling_texture;    // Texture du plafond
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

// Nouvelles fonctions Phase 2
void    init_map(t_env *env);
void    init_player(t_env *env);
void    handle_input(t_env *env);
void    render_3d(t_env *env);
void    draw_wall_slice(t_env *env, int x, double wall_dist, int wall_type);

// DDA Raycasting optimisé
t_ray_hit cast_ray_dda(t_env *env, double ray_angle);
void    draw_wall_slice_textured(t_env *env, int x, t_ray_hit *hit);

// Textures
int     init_textures(t_env *env);
void    free_textures(t_env *env);

// Bitmap font
void    draw_text(t_env *env, const char *text, int x, int y, Uint32 color);
void    draw_fps_on_screen(t_env *env);

// Menu
int     show_menu(t_env *env);

// Helpers
t_rectangle new_rectangle(Uint32 inside_color, Uint32 line_color, int filled, int line_size);
t_circle    new_circle(Uint32 line_color, Uint32 color, t_point center, int radius);


#endif 