// ================================================================================
// GRANDE MAP DE TEST: "DOOM TEST ARENA"
// 14 secteurs pour tester: portails, hauteurs variables, escaliers, exterieur
//
// INSTRUCTIONS:
// 1. Changer MAX_POLYS de 10 a 15 dans header/types.h
// 2. Remplacer le contenu de la fonction init() dans main.c par ce code
//
// ================================================================================
//
// LAYOUT DE LA MAP:
//
//                    ┌─────────────────────────────────────────┐
//                    │          S9: Cour exterieure            │
//                    │              (Skybox)                   │
//                    └──────────────┬──────┬───────────────────┘
//                                   │Portal│
//              ┌────────────────────┴──────┴────────────────────┐
//              │           S7: Mezzanine surelevee              │
//              │        (floorHeight eleve, vue panoramique)    │
//              └───────────────┬────────────┬───────────────────┘
//                              │   Portal   │
//    ┌─────────────────────────┴────────────┴─────────────────────┐
//    │                                                             │
//    │                  S2: GRANDE SALLE CENTRALE                  │
//    │         ┌────┐                              ┌────┐         │
//    │         │ S3 │      (4 piliers)             │ S4 │         │
//    │         └────┘                              └────┘         │
//    │         ┌────┐                              ┌────┐         │
//    │         │ S5 │                              │ S6 │         │
//    │         └────┘                              └────┘         │
//    │  S10:Alcove                                                │
//    └─────────┬─────────┬───────────────┬─────────┬──────────────┘
//              │  Portal │               │  Portal │
//         ┌────┴─────────┴────┐     ┌────┴─────────┴────┐
//         │   S1: Corridor    │     │  S8→S11→S12       │
//         │   (bas plafond)   │     │    Escaliers      │
//         └────────┬──────────┘     └───────────────────┘
//                  │ Portal
//         ┌────────┴──────────┐
//         │   S0: SPAWN ROOM  │
//         └───────────────────┘
//
// ================================================================================

// COPIE CE CODE DANS LA FONCTION init() (apres l'initialisation de la camera)
// Remplace tout depuis "// Reinitialiser tous les polygones" jusqu'a "detectPortals();"

#include "./header/game.h"
#include "./header/types.h"
#include "./header/map_test_arena.h"

void init_test_arena_map(void)
{
    // Reinitialiser tous les polygones
    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].vertCnt = 0;

    // ========================================
    // SECTEUR 0 : Spawn Room (depart joueur)
    // Hauteur: 60 unites
    // ========================================
    global.polys[0].vert[0].x = 350.00;
    global.polys[0].vert[0].y = 50.00;
    global.polys[0].vert[1].x = 450.00;
    global.polys[0].vert[1].y = 50.00;
    global.polys[0].vert[2].x = 450.00;
    global.polys[0].vert[2].y = 150.00;  // Portal → S1
    global.polys[0].vert[3].x = 350.00;
    global.polys[0].vert[3].y = 150.00;
    global.polys[0].height = 60000000;
    global.polys[0].vertCnt = 4;

    // ========================================
    // SECTEUR 1 : Couloir etroit (oppressant)
    // Hauteur: 50 unites (plafond bas mais passable)
    // ========================================
    global.polys[1].vert[0].x = 450.00;  // Portal ← S0
    global.polys[1].vert[0].y = 150.00;
    global.polys[1].vert[1].x = 350.00;
    global.polys[1].vert[1].y = 150.00;
    global.polys[1].vert[2].x = 350.00;
    global.polys[1].vert[2].y = 300.00;  // Portal → S2
    global.polys[1].vert[3].x = 450.00;
    global.polys[1].vert[3].y = 300.00;
    global.polys[1].height = 50000000;
    global.polys[1].vertCnt = 4;

    // ========================================
    // SECTEUR 2 : Grande salle centrale (Hall)
    // Hauteur: 100 unites (plafond haut)
    // ========================================
    global.polys[2].vert[0].x = 100.00;
    global.polys[2].vert[0].y = 300.00;
    global.polys[2].vert[1].x = 350.00;  // Portal ← S1
    global.polys[2].vert[1].y = 300.00;
    global.polys[2].vert[2].x = 450.00;
    global.polys[2].vert[2].y = 300.00;
    global.polys[2].vert[3].x = 700.00;
    global.polys[2].vert[3].y = 300.00;
    global.polys[2].vert[4].x = 700.00;
    global.polys[2].vert[4].y = 450.00;  // Portal → S15 (Pillar R)
    global.polys[2].vert[5].x = 700.00;
    global.polys[2].vert[5].y = 500.00;
    global.polys[2].vert[6].x = 700.00;
    global.polys[2].vert[6].y = 750.00;
    global.polys[2].vert[7].x = 100.00;
    global.polys[2].vert[7].y = 750.00;
    global.polys[2].vert[8].x = 100.00;
    global.polys[2].vert[8].y = 500.00;  // Portal → S14 (Pillar L)
    global.polys[2].vert[9].x = 100.00;
    global.polys[2].vert[9].y = 450.00;
    global.polys[2].height = 100000000;
    global.polys[2].vertCnt = 10;

    // ========================================
    // SECTEUR 3 : Pilier Nord-Ouest
    // ========================================
    global.polys[3].vert[0].x = 220.00;
    global.polys[3].vert[0].y = 380.00;
    global.polys[3].vert[1].x = 280.00;
    global.polys[3].vert[1].y = 380.00;
    global.polys[3].vert[2].x = 280.00;
    global.polys[3].vert[2].y = 440.00;
    global.polys[3].vert[3].x = 220.00;
    global.polys[3].vert[3].y = 440.00;
    global.polys[3].height = 80000000;
    global.polys[3].vertCnt = 4;

    // ========================================
    // SECTEUR 4 : Pilier Nord-Est
    // ========================================
    global.polys[4].vert[0].x = 520.00;
    global.polys[4].vert[0].y = 380.00;
    global.polys[4].vert[1].x = 580.00;
    global.polys[4].vert[1].y = 380.00;
    global.polys[4].vert[2].x = 580.00;
    global.polys[4].vert[2].y = 440.00;
    global.polys[4].vert[3].x = 520.00;
    global.polys[4].vert[3].y = 440.00;
    global.polys[4].height = 80000000;
    global.polys[4].vertCnt = 4;

    // ========================================
    // SECTEUR 5 : Pilier Sud-Ouest
    // ========================================
    global.polys[5].vert[0].x = 220.00;
    global.polys[5].vert[0].y = 560.00;
    global.polys[5].vert[1].x = 280.00;
    global.polys[5].vert[1].y = 560.00;
    global.polys[5].vert[2].x = 280.00;
    global.polys[5].vert[2].y = 620.00;
    global.polys[5].vert[3].x = 220.00;
    global.polys[5].vert[3].y = 620.00;
    global.polys[5].height = 80000000;
    global.polys[5].vertCnt = 4;

    // ========================================
    // SECTEUR 6 : Pilier Sud-Est
    // ========================================
    global.polys[6].vert[0].x = 520.00;
    global.polys[6].vert[0].y = 560.00;
    global.polys[6].vert[1].x = 580.00;
    global.polys[6].vert[1].y = 560.00;
    global.polys[6].vert[2].x = 580.00;
    global.polys[6].vert[2].y = 620.00;
    global.polys[6].vert[3].x = 520.00;
    global.polys[6].vert[3].y = 620.00;
    global.polys[6].height = 80000000;
    global.polys[6].vertCnt = 4;

    // ========================================
    // SECTEUR 7 : Mezzanine (balcon sureleve)
    // Hauteur: 3.5 unites, plateforme en hauteur
    // ========================================
    global.polys[7].vert[0].x = 700.00;  // Portal ← S2
    global.polys[7].vert[0].y = 750.00;
    global.polys[7].vert[1].x = 100.00;
    global.polys[7].vert[1].y = 750.00;
    global.polys[7].vert[2].x = 100.00;
    global.polys[7].vert[2].y = 850.00;
    global.polys[7].vert[3].x = 700.00;
    global.polys[7].vert[3].y = 850.00;  // Portal → S9
    global.polys[7].height = 80000000;
    global.polys[7].vertCnt = 4;

    // ========================================
    // SECTEUR 8 : Escalier Palier 1 (entree)
    // ========================================
    global.polys[8].vert[0].x = 550.00;  // Portal ← S2
    global.polys[8].vert[0].y = 300.00;
    global.polys[8].vert[1].x = 650.00;
    global.polys[8].vert[1].y = 300.00;
    global.polys[8].vert[2].x = 650.00;
    global.polys[8].vert[2].y = 380.00;  // Portal → S11
    global.polys[8].vert[3].x = 550.00;
    global.polys[8].vert[3].y = 380.00;
    global.polys[8].height = 80000000;
    global.polys[8].vertCnt = 4;

    // ========================================
    // SECTEUR 9 : Cour exterieure (Skybox)
    // Hauteur: tres haute (zone avec ciel)
    // ========================================
    global.polys[9].vert[0].x = 700.00;  // Portal ← S7
    global.polys[9].vert[0].y = 850.00;
    global.polys[9].vert[1].x = 100.00;
    global.polys[9].vert[1].y = 850.00;
    global.polys[9].vert[2].x = 100.00;
    global.polys[9].vert[2].y = 1050.00;
    global.polys[9].vert[3].x = 700.00;
    global.polys[9].vert[3].y = 1050.00;
    global.polys[9].height = 80000000;
    global.polys[9].vertCnt = 4;

    // ========================================
    // SECTEUR 10 : Alcove secrete (sombre)
    // Petite niche a gauche du hall
    // ========================================
    global.polys[10].vert[0].x = 100.00;  // Portal ← S2
    global.polys[10].vert[0].y = 500.00;
    global.polys[10].vert[1].x = 50.00;
    global.polys[10].vert[1].y = 500.00;
    global.polys[10].vert[2].x = 50.00;
    global.polys[10].vert[2].y = 600.00;
    global.polys[10].vert[3].x = 100.00;
    global.polys[10].vert[3].y = 600.00;
    global.polys[10].height = 80000000;
    global.polys[10].vertCnt = 4;

    // ========================================
    // SECTEUR 11 : Escalier Palier 2
    // ========================================
    global.polys[11].vert[0].x = 550.00;  // Portal ← S8
    global.polys[11].vert[0].y = 380.00;
    global.polys[11].vert[1].x = 650.00;
    global.polys[11].vert[1].y = 380.00;
    global.polys[11].vert[2].x = 650.00;
    global.polys[11].vert[2].y = 460.00;  // Portal → S12
    global.polys[11].vert[3].x = 550.00;
    global.polys[11].vert[3].y = 460.00;
    global.polys[11].height = 80000000;
    global.polys[11].vertCnt = 4;

    // ========================================
    // SECTEUR 12 : Escalier Palier 3 (haut)
    // ========================================
    global.polys[12].vert[0].x = 550.00;  // Portal ← S11
    global.polys[12].vert[0].y = 460.00;
    global.polys[12].vert[1].x = 650.00;
    global.polys[12].vert[1].y = 460.00;
    global.polys[12].vert[2].x = 650.00;
    global.polys[12].vert[2].y = 540.00;
    global.polys[12].vert[3].x = 550.00;
    global.polys[12].vert[3].y = 540.00;
    global.polys[12].height = 80000000;
    global.polys[12].vertCnt = 4;

    // ========================================
    // SECTEUR 13 : Couloir lateral Est
    // Connecte escaliers → mezzanine
    // ========================================
    global.polys[13].vert[0].x = 650.00;  // Portal ← S12
    global.polys[13].vert[0].y = 460.00;
    global.polys[13].vert[1].x = 750.00;
    global.polys[13].vert[1].y = 460.00;
    global.polys[13].vert[2].x = 750.00;
    global.polys[13].vert[2].y = 750.00;
    global.polys[13].vert[3].x = 700.00;
    global.polys[13].vert[3].y = 750.00;
    global.polys[13].vert[4].x = 700.00;
    global.polys[13].vert[4].y = 540.00;
    global.polys[13].vert[5].x = 650.00;
    global.polys[13].vert[5].y = 540.00;
    global.polys[13].height = 80000000;
    global.polys[13].vertCnt = 6;

    // ========================================
    // SECTEUR 14 : Pilier Mur Gauche (S2)
    // ========================================
    global.polys[14].vert[0].x = 100.00;  // Portal ← S2
    global.polys[14].vert[0].y = 500.00;
    global.polys[14].vert[1].x = 150.00;
    global.polys[14].vert[1].y = 500.00;
    global.polys[14].vert[2].x = 150.00;
    global.polys[14].vert[2].y = 450.00;
    global.polys[14].vert[3].x = 100.00;
    global.polys[14].vert[3].y = 450.00;
    global.polys[14].height = 80000000;
    global.polys[14].vertCnt = 4;

    // ========================================
    // SECTEUR 15 : Pilier Mur Droit (S2)
    // ========================================
    global.polys[15].vert[0].x = 700.00;  // Portal ← S2
    global.polys[15].vert[0].y = 500.00;
    global.polys[15].vert[1].x = 700.00;
    global.polys[15].vert[1].y = 450.00;
    global.polys[15].vert[2].x = 650.00;
    global.polys[15].vert[2].y = 450.00;
    global.polys[15].vert[3].x = 650.00;
    global.polys[15].vert[3].y = 500.00;
    global.polys[15].height = 80000000;
    global.polys[15].vertCnt = 4;


    // ========================================
    // INITIALISATION DES TEXTURES
    // ========================================
    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].textureId = 0;
    
    // ========================================
    // COULEURS DES SECTEURS
    // ========================================
    global.polys[0].color = 0x8B4513;   // Spawn - Brun
    global.polys[1].color = 0x4A3728;   // Couloir - Brun fonce
    global.polys[2].color = 0x5A5A5A;   // Hall - Gris pierre
    global.polys[3].color = 0x2F4F4F;   // Piliers - Ardoise
    global.polys[4].color = 0x2F4F4F;
    global.polys[5].color = 0x2F4F4F;
    global.polys[6].color = 0x2F4F4F;
    global.polys[7].color = 0x8B7355;   // Mezzanine - Bois
    global.polys[8].color = 0x696969;   // Escaliers - Gris
    global.polys[9].color = 0x228B22;   // Exterieur - Vert
    global.polys[10].color = 0x8B0000;  // Alcove - Rouge sombre
    global.polys[11].color = 0x708090;  // Escalier 2
    global.polys[12].color = 0x778899;  // Escalier 3
    global.polys[13].color = 0x6B8E23;  // Couloir Est
    global.polys[14].color = 0x2F4F4F;  // Pilier L
    global.polys[15].color = 0x2F4F4F;  // Pilier R

    convertPolysToSectors();
    detectPortals();

    int wallTexId = findTextureByName("wall");
    int floorTexId = findTextureByName("floor");
    
    for (int i = 0; i < MAX_POLYS; i++)
        global.polys[i].textureId = wallTexId;
    
    for (int i = 0; i < global.sectorCount; i++)
    {
        global.sectors[i].floorTextureId = floorTexId;
        global.sectors[i].ceilingTextureId = floorTexId;
    }
    
    // Assign textures to all walls (including portals)
    for (int i = 0; i < global.wallCount; i++)
    {
        if (global.walls[i].middleTextureId == -1 || global.walls[i].middleTextureId == 0)
            global.walls[i].middleTextureId = wallTexId;
        if (global.walls[i].upperTextureId == -1 || global.walls[i].upperTextureId == 0)
            global.walls[i].upperTextureId = wallTexId;
        if (global.walls[i].lowerTextureId == -1 || global.walls[i].lowerTextureId == 0)
            global.walls[i].lowerTextureId = wallTexId;
    }
    
    if (global.sectorCount > 9)
    {
        global.sectors[9].ceilingTextureId = -1;
    }
}
