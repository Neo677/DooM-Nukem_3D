#include "../header/ceiling.h"
#include "../header/rasterize.h"

// ============================================
// IMPLÉMENTATION DU RENDU PLAFOND / SKYBOX
// ============================================

// Applique un fog basé sur la distance
int applyDistanceFog(int color, float distance)
{
    float fogFactor = 1.0f - fminf(distance / 80.0f, 0.75f);
    if (fogFactor < 0.25f)
        fogFactor = 0.25f;
    
    int r = (int)(((color >> 16) & 0xFF) * fogFactor);
    int g = (int)(((color >> 8) & 0xFF) * fogFactor);
    int b = (int)((color & 0xFF) * fogFactor);
    
    return ((r << 16) | (g << 8) | b);
}

// Vérifie si le secteur utilise une skybox pour le plafond
int isSkyboxCeiling(int sectorId)
{
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (1);  // Par défaut: skybox
    return (global.sectors[sectorId].ceilingTextureId == -1);
}

// Vérifie si le secteur utilise une skybox pour le sol (rare mais possible)
int isSkyboxFloor(int sectorId)
{
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (0);  // Par défaut: sol texturé
    return (global.sectors[sectorId].floorTextureId == -1);
}

// ============================================
// RENDU SKYBOX
// Projection cylindrique basée sur l'angle de vue
// ============================================
int getSkyPixelAt(int screenX, int screenY)
{
    if (!global.skybox.loaded)
        return (SLY_COLOR);  // Couleur fallback (bleu ciel)
    
    // ─────────────────────────────────────────
    // U = basé sur l'angle horizontal de vue
    // ─────────────────────────────────────────
    float normalizedX = (float)screenX / screenW;
    float rayAngle = global.cam.camAngle + (normalizedX - 0.5f) * FOV;
    
    // Normaliser l'angle dans [0, 2π]
    while (rayAngle < 0)
        rayAngle += 2.0f * M_PI;
    while (rayAngle >= 2.0f * M_PI)
        rayAngle -= 2.0f * M_PI;
    
    // Mapping cylindrique horizontal
    int texU = (int)(rayAngle / (2.0f * M_PI) * global.skybox.width);
    
    // ─────────────────────────────────────────
    // V = basé sur la position verticale à l'écran
    // Prend en compte le pitch de la caméra
    // ─────────────────────────────────────────
    float horizon = screenH / 2.0f + global.cam.camPitch * (screenH / 4.0f);
    float vRatio;
    
    if (screenY < horizon)
    {
        // Au-dessus de l'horizon: partie haute de la skybox
        vRatio = (float)(horizon - screenY) / horizon;
        vRatio = 0.5f - vRatio * 0.5f;  // 0.5 (horizon) → 0.0 (top)
    }
    else
    {
        // En dessous de l'horizon (ne devrait pas arriver pour le ciel)
        vRatio = 0.5f;
    }
    
    int texV = (int)(vRatio * global.skybox.height);
    
    // Clamping sécurisé
    texU = ((texU % global.skybox.width) + global.skybox.width) % global.skybox.width;
    texV = fmax(0, fmin(texV, global.skybox.height - 1));
    
    return (getSkyboxPixel(texU, texV));
}

// ============================================
// RENDU PLAFOND TEXTURÉ
// Coordonnées UV alignées au monde (world-space)
// ============================================
int getCeilingPixelAt(int screenX, int screenY, int sectorId, float rowDist)
{
    (void)screenY;  // Non utilisé car rowDist est fourni directement
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (0x404040);
    
    t_sector *sector = &global.sectors[sectorId];
    
    // ─────────────────────────────────────────
    // Calculer la direction du rayon pour ce pixel
    // ─────────────────────────────────────────
    float rayAngle = global.cam.camAngle + 
                     ((float)(screenX - screenW / 2) / screenW) * FOV;
    float rayDirX = cos(rayAngle);
    float rayDirY = sin(rayAngle);
    
    // ─────────────────────────────────────────
    // Position monde du point touché au plafond
    // ─────────────────────────────────────────
    float worldX = global.cam.camPos.x + rayDirX * rowDist;
    float worldY = global.cam.camPos.y + rayDirY * rowDist;
    
    // ─────────────────────────────────────────
    // Récupérer la texture
    // ─────────────────────────────────────────
    int texId = sector->ceilingTextureId;
    if (texId < 0 || texId >= global.tex_manager.count)
        return (0x404040);
    
    t_texture *tex = &global.tex_manager.textures[texId];
    if (!tex || !tex->loaded)
        return (0x404040);
    
    // ─────────────────────────────────────────
    // Coordonnées UV (world-aligned)
    // ─────────────────────────────────────────
    float scale = FLOOR_TEXTURE_SCALE;
    int texX = ((int)(worldX * scale) % tex->width + tex->width) % tex->width;
    int texY = ((int)(worldY * scale) % tex->height + tex->height) % tex->height;
    
    int color = getTexturePixel(texId, texX, texY);
    
    // Appliquer le fog
    return (applyDistanceFog(color, rowDist));
}

// ============================================
// RENDU SOL TEXTURÉ
// Même principe que le plafond mais inversé
// ============================================
int getFloorPixelAt(int screenX, int screenY, int sectorId, float rowDist)
{
    (void)screenY;  // Non utilisé car rowDist est fourni directement
    if (sectorId < 0 || sectorId >= global.sectorCount)
        return (0x202020);
    
    t_sector *sector = &global.sectors[sectorId];
    
    // Direction du rayon
    float rayAngle = global.cam.camAngle + 
                     ((float)(screenX - screenW / 2) / screenW) * FOV;
    float rayDirX = cos(rayAngle);
    float rayDirY = sin(rayAngle);
    
    // Position monde
    float worldX = global.cam.camPos.x + rayDirX * rowDist;
    float worldY = global.cam.camPos.y + rayDirY * rowDist;
    
    // Récupérer la texture
    int texId = sector->floorTextureId;
    if (texId < 0 || texId >= global.tex_manager.count)
        return (0x202020);
    
    t_texture *tex = &global.tex_manager.textures[texId];
    if (!tex || !tex->loaded)
        return (0x202020);
    
    // Coordonnées UV
    float scale = FLOOR_TEXTURE_SCALE;
    int texX = ((int)(worldX * scale) % tex->width + tex->width) % tex->width;
    int texY = ((int)(worldY * scale) % tex->height + tex->height) % tex->height;
    
    int color = getTexturePixel(texId, texX, texY);
    
    return (applyDistanceFog(color, rowDist));
}

// ============================================
// RENDU D'UNE TRANCHE VERTICALE DE PLAFOND
// ============================================
void renderCeilingSlice(t_render *render, int x, int yStart, int yEnd,
                        int sectorId)
{
    if (yStart >= yEnd || x < 0 || x >= screenW)
        return;
    
    // Clamp les valeurs
    if (yStart < 0)
        yStart = 0;
    if (yEnd > screenH)
        yEnd = screenH;
    
    t_sector *sector = NULL;
    if (sectorId >= 0 && sectorId < global.sectorCount)
        sector = &global.sectors[sectorId];
    
    // Calculer l'horizon avec le pitch
    float horizon = screenH / 2.0f + global.cam.camPitch * (screenH / 4.0f);
    float waveOffset = WWAVE_MAG * sinf(global.cam.stepWave);
    horizon += waveOffset;
    
    for (int y = yStart; y < yEnd; y++)
    {
        int color;
        
        if (isSkyboxCeiling(sectorId))
        {
            // ══════════════════════════════════════
            // MODE SKYBOX
            // ══════════════════════════════════════
            color = getSkyPixelAt(x, y);
        }
        else
        {
            // ══════════════════════════════════════
            // MODE PLAFOND TEXTURÉ
            // ══════════════════════════════════════
            // Calculer la distance pour cette rangée
            float ceilHeight = sector->ceilingHeight - global.cam.camZ;
            float rowDist = ceilHeight / (horizon - y + 0.1f);
            
            if (rowDist < 0.1f)
                rowDist = 0.1f;
            if (rowDist > 1000.0f)
                rowDist = 1000.0f;
            
            color = getCeilingPixelAt(x, y, sectorId, rowDist);
        }
        
        putPixel(render, x, y, color);
    }
}

// ============================================
// RENDU D'UNE TRANCHE VERTICALE DE SOL
// ============================================
void renderFloorSlice(t_render *render, int x, int yStart, int yEnd,
                      int sectorId)
{
    if (yStart >= yEnd || x < 0 || x >= screenW)
        return;
    
    // Clamp les valeurs
    if (yStart < 0)
        yStart = 0;
    if (yEnd > screenH)
        yEnd = screenH;
    
    t_sector *sector = NULL;
    if (sectorId >= 0 && sectorId < global.sectorCount)
        sector = &global.sectors[sectorId];
    
    // Calculer l'horizon avec le pitch
    float horizon = screenH / 2.0f + global.cam.camPitch * (screenH / 4.0f);
    float waveOffset = WWAVE_MAG * sinf(global.cam.stepWave);
    horizon += waveOffset;
    
    for (int y = yStart; y < yEnd; y++)
    {
        int color;
        
        if (isSkyboxFloor(sectorId))
        {
            // Skybox au sol (effet spécial: gouffre, espace, etc.)
            color = getSkyPixelAt(x, screenH - y);  // Inverser pour effet miroir
        }
        else if (sector && sector->floorTextureId >= 0)
        {
            // ══════════════════════════════════════
            // MODE SOL TEXTURÉ
            // ══════════════════════════════════════
            float floorHeight = global.cam.camZ - sector->floorHeight;
            float rowDist = floorHeight / (y - horizon + 0.1f);
            
            if (rowDist < 0.1f)
                rowDist = 0.1f;
            if (rowDist > 1000.0f)
                rowDist = 1000.0f;
            
            color = getFloorPixelAt(x, y, sectorId, rowDist);
        }
        else
        {
            // Sol par défaut (gris dégradé)
            float depth = (float)(y - horizon) / (screenH - horizon);
            int intensity = (int)(40 * (1.0f - depth * 0.5f));
            color = (intensity << 16) | (intensity << 8) | intensity;
        }
        
        putPixel(render, x, y, color);
    }
}

// ============================================
// INITIALISATION DU Y-BUFFER
// À appeler au début de chaque frame
// ============================================
void initYBuffer(void)
{
    for (int x = 0; x < screenW && x < SCREEN_W_MAX; x++)
    {
        global.ybuffer.yTop[x] = 0;
        global.ybuffer.yBottom[x] = screenH;
        global.ybuffer.ceilingSector[x] = -1;
        global.ybuffer.floorSector[x] = -1;
    }
}

