# Doom-Nukem 3D

Un FPS rétro-moderne inspiré de Doom 4, utilisant un moteur raycasting personnalisé.

## 🎮 Caractéristiques

- Moteur raycasting avancé sans GPU
- Sprites réalistes inspirés de Doom 4
- Système d'éclairage dynamique
- Effets de particules et interactions
- Éditeur de niveaux personnalisé
- Musique metal dynamique
- Narration environnementale

## 🛠️ Technologies

- MinilibX pour le rendu graphique
- Moteur raycasting personnalisé
- Système de parsing de maps (.map/.json)
- Support audio (.wav/.ogg)

## 📁 Structure du Projet

```
.
├── src/                    # Code source
│   ├── engine/            # Moteur raycasting
│   ├── game/              # Logique du jeu
│   ├── editor/            # Éditeur de niveaux
│   └── utils/             # Utilitaires
├── assets/                # Ressources
│   ├── textures/          # Textures et sprites
│   ├── maps/             # Fichiers de niveaux
│   ├── sounds/           # Effets sonores
│   └── music/            # Musiques
├── include/              # Headers
└── lib/                  # Bibliothèques externes
```

## 🚀 Installation

```bash
# Cloner le repository
git clone https://github.com/votre-username/DooM-Nukem_3D.git

# Compiler le projet
make

# Lancer le jeu
./doom-nukem
```

## 🎯 Objectifs

- [ ] Implémentation du moteur raycasting de base
- [ ] Système de rendu des sprites
- [ ] Gestion des collisions
- [ ] Système d'éclairage dynamique
- [ ] Éditeur de niveaux
- [ ] Système audio
- [ ] Effets de particules
- [ ] Narration environnementale

## 📝 Licence

Ce projet est sous licence MIT.

---

## 🚧 État Actuel du Développement

### ✅ **Infrastructure de Base Implémentée**

#### **1. Architecture du Projet**
- ✅ Structure modulaire avec séparation des responsabilités
- ✅ Système de compilation cross-platform (Windows/Linux/macOS)
- ✅ Configuration SDL2 pour développement et tests
- ✅ Prêt pour transition vers MinilibX

#### **2. Gestion des Fenêtres (`window.c/h`)**
- ✅ Initialisation SDL2 avec gestion d'erreurs
- ✅ Création de fenêtre centrée et configurable
- ✅ Gestion propre du cycle de vie (init/shutdown)
- ✅ Interface abstraite pour faciliter la transition vers MinilibX

#### **3. Système de Jeu (`game_state.c/h`)**
- ✅ Gestion des états du jeu (running, paused, debug)
- ✅ Contrôle de FPS avec frame timing précis
- ✅ Calcul automatique du delta time
- ✅ Système de debug mode toggle
- ✅ Gestion des dimensions d'écran

#### **4. Système de Joueur (`player.c/h`)**
- ✅ Structure de données pour position 3D (x, y, z)
- ✅ Gestion de l'orientation (angle de direction)
- ✅ Initialisation avec paramètres configurables
- ✅ Prêt pour implémentation des mouvements

#### **5. Système d'Entrées (`keyboard.c/h`)**
- ✅ Mapping complet des touches (WASD, QE, Space, Ctrl)
- ✅ Gestion des états de touches (pressed/released)
- ✅ Mouvements du joueur :
  - ✅ Avancer/Reculer (W/S)
  - ✅ Rotation gauche/droite (A/D)
  - ✅ Strafe gauche/droite (Q/E)
  - ✅ Monter/Descendre (Space/Ctrl)
- ✅ Contrôles spéciaux :
  - ✅ Quitter (Escape)
  - ✅ Toggle map (M)
  - ✅ Debug mode (O)

#### **6. Moteur de Rendu (`renderer.c/h`)**
- ✅ Initialisation du contexte de rendu SDL2
- ✅ Gestion du buffer d'écran personnalisé
- ✅ Système de double buffering avec textures
- ✅ Fonctions de dessin de base :
  - ✅ `R_DrawPoint()` - Dessin de pixels
  - ✅ `R_DrawLine()` - Algorithme de Bresenham
  - ✅ `R_ClearScreenBuffer()` - Nettoyage d'écran
- ✅ Architecture pour raycasting :
  - ✅ Structures de données pour secteurs et murs
  - ✅ Système de queue de secteurs
  - ✅ Calculs de transformation 3D (rotation, projection)
  - ✅ Gestion des portails et élévations

#### **7. Utilitaires (`utils.c/h`)**
- ✅ Fonction de génération de nombres aléatoires
- ✅ Structure pour extensions futures

### 🔧 **Configuration Technique**

#### **Compilation**
- ✅ Makefile cross-platform fonctionnel
- ✅ Support ARM64 pour Mac Apple Silicon
- ✅ Liens symboliques corrects vers SDL2
- ✅ Compilation sans erreurs ni warnings

#### **Structure de Données**
```c
// Joueur avec position 3D et orientation
typedef struct _player {
    vec2_t position;    // Position 2D (x, y)
    double z;           // Hauteur/élévation
    double dir_angles;  // Angle de direction
} player_t;

// Secteur avec murs et propriétés
typedef struct _sector {
    int id;
    wall_t walls[10];   // Murs du secteur
    int num_walls;
    int height;         // Hauteur du secteur
    int elevation;      // Élévation du sol
    double dist;
    unsigned int color;
    unsigned int floor_clr;
    unsigned int ceil_clr;
} sector_t;
```

### 🎮 **Fonctionnalités Implémentées**

#### **Contrôles Actifs**
- **Mouvement** : WASD pour avancer/reculer/rotation
- **Strafe** : Q/E pour déplacement latéral
- **Élévation** : Space/Ctrl pour monter/descendre
- **Interface** : Escape pour quitter, M pour map, O pour debug

#### **Rendu de Base**
- ✅ Fenêtre graphique fonctionnelle
- ✅ Buffer d'écran personnalisé
- ✅ Système de double buffering
- ✅ Fonctions de dessin de base

### 🔄 **Prochaines Étapes**

#### **Phase 1 : Rendu 3D Basique**
- [ ] Implémentation complète du raycasting
- [ ] Rendu des murs avec perspective
- [ ] Gestion des textures de base
- [ ] Système de collision simple

#### **Phase 2 : Environnement**
- [ ] Chargement de maps depuis fichiers
- [ ] Système de secteurs complet
- [ ] Gestion des portails entre secteurs
- [ ] Éclairage de base

#### **Phase 3 : Gameplay**
- [ ] Système d'armes
- [ ] Ennemis basiques
- [ ] Système de santé
- [ ] Interface utilisateur

### 📊 **Métriques de Code**
- **Lignes de code** : ~800 lignes
- **Fichiers sources** : 8 fichiers (.c/.h)
- **Architecture** : Modulaire et extensible
- **Documentation** : Commentaires détaillés en français

### 🛠️ **Environnement de Développement**
- **OS** : macOS (Apple Silicon)
- **Compilateur** : GCC avec support ARM64
- **Bibliothèque** : SDL2 (prêt pour MinilibX)
- **IDE** : Compatible VSCode avec configuration

---

*Dernière mise à jour : Juillet 2024*
