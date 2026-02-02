#ifndef SECTOR_H
# define SECTOR_H

// Structure vertex 2D
typedef struct s_vertex {
    double x;
    double y;
    int num;
} t_vertex;

// Structure secteur (simplifié Phase 3)
typedef struct s_sector {
    int     *vertices;          // Indices vers vertices globaux
    int     nb_vertices;
    int     *neighbors;         // -1=mur, >=0=portal vers secteur N
    
    // Hauteurs
    double  floor_height;
    double  ceiling_height;
    
    // Textures
    int     floor_texture;
    int     ceiling_texture;
    int     *wall_textures;     // Texture par mur
    
    // Rendering
    int     rendered;           // Flag pour portal rendering
} t_sector;

// Map avec secteurs
typedef struct s_map_sectors {
    t_vertex    *vertices;
    int         nb_vertices;
    
    t_sector    *sectors;
    int         nb_sectors;
} t_map_sectors;

#endif
