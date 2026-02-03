#ifndef CEILING_H
# define CEILING_H

# include "game.h"

# define FOV (M_PI / 3.0f)

// Fonction externe definie dans main.c
int		getSkyboxPixel(int x, int y);

// Fonction definie dans main.c - recupere un pixel de la skybox
int		getSkyboxPixel(int x, int y);

// ============================================
// RENDU CONDITIONNEL PLAFOND / SKYBOX
// ============================================

// Verifie si le secteur utilise une skybox
int		isSkyboxCeiling(int sectorId);

// Recupere un pixel de la skybox a partir des coordonnees ecran
int		getSkyPixelAt(int screenX, int screenY);

// Recupere un pixel du plafond texture
int		getCeilingPixelAt(int screenX, int screenY, int sectorId, float rowDist);

// Rend une tranche verticale de plafond (colonne x, de yStart a yEnd)
void	renderCeilingSlice(t_render *render, int x, int yStart, int yEnd,
			int sectorId);

// Rend une tranche verticale de sol (colonne x, de yStart a yEnd)
void	renderFloorSlice(t_render *render, int x, int yStart, int yEnd,
			int sectorId);

// Initialise le Y-buffer avant chaque frame
void	initYBuffer(void);

// Applique le fog de distance a une couleur
int		applyDistanceFog(int color, float distance);

// ✅ OPTIMISATION: Precalcul des directions de rayon
void	precomputeRayDirections(void);

#endif

