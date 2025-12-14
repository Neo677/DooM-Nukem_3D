#ifndef CEILING_H
# define CEILING_H

# include "game.h"

# define FOV (M_PI / 3.0f)

// Fonction externe définie dans main.c
int		getSkyboxPixel(int x, int y);

// Fonction définie dans main.c - récupère un pixel de la skybox
int		getSkyboxPixel(int x, int y);

// ============================================
// RENDU CONDITIONNEL PLAFOND / SKYBOX
// ============================================

// Vérifie si le secteur utilise une skybox
int		isSkyboxCeiling(int sectorId);

// Récupère un pixel de la skybox à partir des coordonnées écran
int		getSkyPixelAt(int screenX, int screenY);

// Récupère un pixel du plafond texturé
int		getCeilingPixelAt(int screenX, int screenY, int sectorId, float rowDist);

// Rend une tranche verticale de plafond (colonne x, de yStart à yEnd)
void	renderCeilingSlice(t_render *render, int x, int yStart, int yEnd,
			int sectorId);

// Rend une tranche verticale de sol (colonne x, de yStart à yEnd)
void	renderFloorSlice(t_render *render, int x, int yStart, int yEnd,
			int sectorId);

// Initialise le Y-buffer avant chaque frame
void	initYBuffer(void);

// Applique le fog de distance à une couleur
int		applyDistanceFog(int color, float distance);

// ✅ OPTIMISATION: Précalcul des directions de rayon
void	precomputeRayDirections(void);

#endif

