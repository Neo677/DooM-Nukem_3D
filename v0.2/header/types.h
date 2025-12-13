#ifndef TYPES_H
# define TYPES_H

# define MAX_POLYS 15
# define MAX_VERTS 8
# define MAX_WALLS 256
# define MAX_SECTORS 64
# define MAX_RENDER_DEPTH 16


typedef struct Vec2_s
{
    float x;
    float y;
}        Vec2_t;

typedef struct lineSeg_s
{
    Vec2_t p1, p2;
}        lineSeg_t;

typedef struct polygon_s
{
    Vec2_t vert[MAX_VERTS];
    int vertCnt;
    float height;
    float curDist;
    int color;
    int textureId;
}        polygon_t;

typedef struct screenSpacePoly_s
{
    Vec2_t vert[MAX_VERTS];
    int vertCnt;
    float distFromCamera;
    int planeIdInPoly;
}        screenSpacePoly_t;

typedef struct Camera_s
{
    float camAngle;
    float camPitch;      // Inclinaison verticale (-1 à 1)
    float camZ;          // Hauteur de la caméra (yeux)
    float stepWave;
    Vec2_t camPos;
    Vec2_t oldCamPos;
    
    // Propriétés pour step-up/down et gravité
    float velZ;          // Vélocité verticale (gravité/saut)
    float footZ;         // Hauteur des pieds (camZ - EYE_HEIGHT)
    float targetZ;       // Hauteur cible pour interpolation smooth
    int onGround;        // 1 = au sol, 0 = en l'air
}   Camera_t;

# define EYE_HEIGHT 32.0f
# define FLOOR_TEXTURE_SCALE 0.5f

typedef struct color_s
{
    unsigned char R, G, B;
}           color_t;

typedef struct s_skybox {
    void *img;
    char *addr;
    int width;
    int height;
    int bits_per_pixel;
    int line_len;
    int endian;
    int loaded;
}          t_skybox;

typedef enum e_texture_type {
    TEXTURE_WALL,
    TEXTURE_FLOOR,
    TEXTURE_CEILING,
    TEXTURE_LIQUID,
    TEXTURE_SPECIAL,
}       t_texture_type;

typedef struct s_texture {
    void *img;
    char *addr;
    int width;
    int height;
    int bits_per_pixel;
    int line_len;
    int endian;
    int loaded;
    char name[64];
    t_texture_type type;
}   t_texture;

typedef struct s_wall {
    int id;
    Vec2_t p1, p2;

    int frontSectorId;
    int backSectorId;
    int upperTextureId;
    int lowerTextureId;
    int middleTextureId;

    int isPortal;
    int twoSided;
}           t_wall;

typedef struct s_sector {
    int id;

    float floorHeight;
    float ceilingHeight;

    int floorTextureId;
    int ceilingTextureId;
    int lightLevel;

    int wallCount;
    int wallIds[MAX_VERTS];

    int visited;

}       t_sector;

// Structure pour la pile de rendu récursif par portail
typedef struct s_render_entry {
    int sectorId;
    int clipLeft;
    int clipRight;
    int depth;
}       t_render_entry;

// Fenêtre de rendu pour le clipping horizontal des portails
typedef struct s_render_window {
    int xStart;      // Colonne de début (inclusive)
    int xEnd;        // Colonne de fin (inclusive)
}       t_render_window;

# define MAX_PORTAL_DEPTH 16

# define MAX_TEXTURES 256

// ============================================
// Y-BUFFER pour le rendu par colonne
// Permet de tracker les limites de rendu plafond/sol
// ============================================
# define SCREEN_W_MAX 1920

typedef struct s_ybuffer {
    int		yTop[SCREEN_W_MAX];      // Limite haute par colonne (init à 0)
    int		yBottom[SCREEN_W_MAX];   // Limite basse par colonne (init à screenH)
    int		ceilingSector[SCREEN_W_MAX];  // Secteur visible au plafond
    int		floorSector[SCREEN_W_MAX];    // Secteur visible au sol
}		t_ybuffer;

#endif