# 📦 Organisation des Sprites - DooM-Nukem 3D v0.4

## ✅ Resume de l'Organisation

**Total de sprites traites : 3,697**
- ✅ Sprites organises automatiquement : 274
- 📋 Sprites copies (structure preservee) : 527
- ❓ Sprites non reconnus (besoin revision manuelle) : 2,896

---

## 📂 Structure Finale

```
v0.4/assets/sprites_organized/
├── monsters/           # 22 types de monstres (3,086 sprites)
├── weapons/            # 11 armes (220 sprites)
├── projectile/         # 4 types (96 sprites)
├── effect/             # Effets visuels (38 sprites)
├── player/             # Sprites joueur (157 sprites)
├── decorations/        # Objets deco (11 sprites)
├── ui/                 # Interface (219 sprites)
└── logo/               # Logos (6 sprites)
```

---

## 🎮 Monstres Disponibles (22 types)

### Monstres Bien Organises ✨

#### Cyberdemon (334 sprites) 🏆
Le plus complet avec animations 8 directions :
```
Cyberdemon/
├── walk/               # 8 directions × 4 frames = 32 sprites
│   ├── front/          # face au joueur
│   ├── front_right/
│   ├── right/
│   ├── back_right/
│   ├── back/
│   ├── back_left/
│   ├── left/
│   └── front_left/
└── misc/               # Autres animations (attack, death, pain...)
    ├── omnidirectional/ # 37 frames (death, gibs, etc.)
    ├── front/          # 8 frames
    ├── front_right/    # 8 frames
    └── ...             # (8 directions)
```

**Utilisation recommandee** : Boss de fin de niveau, necessite calcul d'angle pour sprite rotation.

#### Nazi / SS Trooper (65 sprites) ⭐ SIMPLE
Le plus simple a implementer en premier :
```
Nazi/
└── misc/
    ├── omnidirectional/  # 9 frames (death, pain)
    ├── front/            # 6 frames (walk, attack)
    ├── front_right/      # 6 frames
    └── ...               # (8 directions)
```

**Utilisation recommandee** : Premier ennemi a implementer pour tester le systeme.

### Autres Monstres Disponibles

| Monstre | Sprites | Complexite | Notes |
|---------|---------|------------|-------|
| **BaronOfHell** | 193 | Moyenne | Ennemi tank, attaque mêlee |
| **Cacodemon** | 110 | Moyenne | Volant, projectiles |
| **Cuiball** | 96 | Simple | Petite creature |
| **Demon** | 140 | Moyenne | Mêlee rapide |
| **DoomImp** | 150 | Moyenne | Basique, projectiles |
| **Gargoyle** | 158 | Haute | Volant complexe |
| **Harvester** | 87 | Moyenne | Ennemi unique |
| **HellRazer** | 126 | Haute | Laser special |
| **Lost Soul** | 30 | Simple | Charge suicide |
| **Mancubus** | 134 | Haute | Boss intermediaire |
| **Mecha Zombie** | 142 | Moyenne | Soldat ameliore |
| **Pain Elemental** | 105 | Haute | Spawn Lost Souls |
| **Revenant** | 158 | Haute | Missiles guides |
| **Scientist Zombie** | 126 | Simple | Variante zombie |
| **Shotgunguy** | 142 | Simple | Shotgun basique |
| **Tyrant** | 182 | Haute | Boss puissant |
| **Whiplash** | 214 | Haute | Tres mobile |
| **Zombie Earth** | 126 | Simple | Zombie standard |
| **Zombie Hell** | 126 | Simple | Variante zombie |
| **Zombieman** | 142 | Simple | Ennemi de base |

---

## 🔫 Armes Disponibles (11 types)

### Armes Bien Organisees ✨

#### Pistol (28 sprites)
```
pistol/
├── attack/     # 6 frames - Animation de tir
├── idle/       # 3 frames - Au repos
├── raise/      # 8 frames - equipement
├── toss/       # 6 frames - Lancer arme
└── misc/       # 5 autres
```

#### BFG (28 sprites)
```
BFG/
├── attack/     # 9 frames - Charge et tir
├── idle/       # 1 frame
└── misc/       # 18 autres
```

#### Rocket Launcher (26 sprites)
```
rocketlauncher/
├── attack/     # 5 frames
├── idle/       # 1 frame
└── misc/       # 20 autres
```

### Autres Armes

| Arme | Sprites | Animations |
|------|---------|------------|
| **Super Shotgun** | 20 | Attack (reload double canon) |
| **Shotgun** | 16 | Attack basique |
| **Chainsaw** | 15 | Attack continu |
| **Canon Bolt** | 13 | Attack electrique |
| **Pickupweapon** | 10 | Ramassage au sol |
| **Plasma Rifle** | 5 | Attack plasma |
| **Heavy Assault Rifle** | 4 | Attack auto |
| **Touret Machine** | 55 | Tourelle statique (non joueur) |

---

## 💥 Projectiles (96 sprites)

### Types Disponibles

```
projectile/
├── plasma/              # 32 sprites - Boules energie bleue
├── rocket/              # 20 sprites - Roquettes explosives
├── bfg/                 # 25 sprites - Projectile BFG ultime
└── externe_projectile/  # 19 sprites - Projectiles ennemis
```

**Utilisation** : Billboarding simple (omnidirectionnel), animation en loop.

---

## 🎨 Effets Visuels (38 sprites)

```
effect/
├── blood/      # 12 sprites - Impacts sang (BSPLBIG3)
└── explosion/  # 26 sprites - Explosions roquettes
```

**Utilisation** : Particules ephemeres, z-buffer prioritaire.

---

## 👤 Sprites Joueur (157 sprites)

```
player/
├── caracter full/
│   ├── accroupi/   # Crouching
│   ├── death/      # Mort
│   ├── debout/     # Standing
│   └── shooting/   # Tir
└── interactionarmor2/  # Pickup armure
```

**Utilisation** : Multijoueur ou cutscenes uniquement (FPS = pas visible).

---

## 🎯 Decorations & Items (11 sprites)

```
decorations/
├── barrels/    # Barils explosifs
├── lamps/      # Lampes
└── misc/       # Divers objets statiques
```

**Utilisation** : Objets statiques du niveau, rotation 0 (omnidirectionnels).

---

## 🖥️ Interface UI (219 sprites)

```
ui/
├── alerte/     # ecrans d'alerte
├── BFGXHR      # Crosshair BFG
├── CHGXHR      # Crosshair chaingun
├── GAUSXHR     # Crosshair gauss
└── ...         # Autres elements HUD
```

---

## 📋 Guide d'Implementation Recommande

### Phase 1 : Objets Statiques (1-2 jours)
1. Charger 1 sprite PNG (baril deco)
2. Implementer billboarding basique
3. Afficher avec distance sorting

**Fichiers a utiliser** :
- `decorations/barrel.png` (exemple)

### Phase 2 : Ennemi Simple (3-4 jours)
1. Charger Nazi (sprites simples)
2. Animation walk (1 direction pour debuter)
3. Calcul angle joueur → ennemi
4. Selection sprite selon angle

**Fichiers a utiliser** :
- `monsters/Nazi/misc/front/frame_*.png`

### Phase 3 : Ennemi Complet 8 Directions (1 semaine)
1. Implementer selection des 8 rotations
2. Animations multiples (walk, attack, pain, death)
3. State machine IA basique

**Fichiers a utiliser** :
- `monsters/Nazi/misc/*/frame_*.png` (toutes directions)

### Phase 4 : Arme FPS (3-5 jours)
1. Afficher arme en overlay (2D fixe)
2. Animation idle
3. Animation attack
4. Synchroniser avec tir

**Fichiers a utiliser** :
- `weapons/pistol/idle/frame_01.png`
- `weapons/pistol/attack/frame_*.png`

### Phase 5 : Projectiles (2-3 jours)
1. Billboarding projectile
2. Animation en loop
3. Collision detection

**Fichiers a utiliser** :
- `projectile/plasma/frame_*.png`

### Phase 6 : Effets (2 jours)
1. Particules sang (impact)
2. Explosions
3. Particules ephemeres (disparaissent apres N frames)

**Fichiers a utiliser** :
- `effect/blood/*.png`
- `effect/explosion/*.png`

### Phase 7 : Boss Cyberdemon (1 semaine)
1. Charger toutes les animations
2. Systeme de rotation 8 directions complexe
3. IA avancee
4. Projectiles speciaux

**Fichiers a utiliser** :
- `monsters/Cyberdemon/**/*.png` (334 sprites)

---

## 🛠️ Exemple de Code (Chargement)

### C - Charger une animation

```c
typedef struct s_anim {
    SDL_Texture **frames;
    int frame_count;
    int current_frame;
    double frame_time;
} t_anim;

// Charger une animation
t_anim *load_animation(SDL_Renderer *renderer, const char *path) {
    t_anim *anim = malloc(sizeof(t_anim));
    
    // Exemple: "assets/sprites_organized/monsters/Nazi/misc/front"
    DIR *dir = opendir(path);
    struct dirent *entry;
    int count = 0;
    
    // Compter les frames
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "frame_") && strstr(entry->d_name, ".png"))
            count++;
    }
    
    anim->frame_count = count;
    anim->frames = malloc(sizeof(SDL_Texture*) * count);
    anim->current_frame = 0;
    anim->frame_time = 0.1; // 100ms par frame
    
    // Charger chaque frame
    rewinddir(dir);
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "frame_")) {
            char filepath[256];
            snprintf(filepath, 256, "%s/%s", path, entry->d_name);
            
            SDL_Surface *surface = IMG_Load(filepath);
            anim->frames[i++] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
    }
    
    closedir(dir);
    return anim;
}

// Mettre a jour l'animation
void update_animation(t_anim *anim, double delta_time) {
    anim->frame_time += delta_time;
    
    if (anim->frame_time >= 0.1) {  // 10 FPS
        anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
        anim->frame_time = 0;
    }
}

// Obtenir la frame actuelle
SDL_Texture *get_current_frame(t_anim *anim) {
    return anim->frames[anim->current_frame];
}
```

### Calcul de rotation (8 directions)

```c
// Calcul angle joueur → ennemi
typedef enum {
    ROT_FRONT = 0,      // 0° (face)
    ROT_FRONT_RIGHT,    // 45°
    ROT_RIGHT,          // 90°
    ROT_BACK_RIGHT,     // 135°
    ROT_BACK,           // 180°
    ROT_BACK_LEFT,      // 225°
    ROT_LEFT,           // 270°
    ROT_FRONT_LEFT      // 315°
} t_rotation;

t_rotation get_sprite_rotation(t_v2 enemy_pos, t_v2 player_pos, double player_angle) {
    // Vecteur ennemi → joueur
    double dx = player_pos.x - enemy_pos.x;
    double dy = player_pos.y - enemy_pos.y;
    
    // Angle vers le joueur
    double angle_to_player = atan2(dy, dx);
    
    // Difference avec l'angle de vue du joueur
    double relative_angle = angle_to_player - player_angle;
    
    // Normaliser [-PI, PI]
    while (relative_angle > M_PI) relative_angle -= 2 * M_PI;
    while (relative_angle < -M_PI) relative_angle += 2 * M_PI;
    
    // Convertir en index 0-7
    double sector = (relative_angle + M_PI) / (M_PI / 4);
    return (t_rotation)((int)(sector + 0.5) % 8);
}

// Charger les 8 rotations
const char *rotation_names[] = {
    "front", "front_right", "right", "back_right",
    "back", "back_left", "left", "front_left"
};

t_anim *rotations[8];
for (int i = 0; i < 8; i++) {
    char path[256];
    snprintf(path, 256, "assets/sprites_organized/monsters/Nazi/misc/%s", rotation_names[i]);
    rotations[i] = load_animation(renderer, path);
}

// Utilisation
t_rotation rot = get_sprite_rotation(enemy.pos, player.pos, player.angle);
SDL_Texture *sprite = get_current_frame(rotations[rot]);
```

---

## 📝 Notes Importantes

### Sprites Non Reconnus (2,896)
Beaucoup de sprites n'ont pas ete automatiquement classes car :
- Noms non standard
- Manque de metadata
- Variations specifiques du jeu original

**Action recommandee** : Trier manuellement au fur et a mesure des besoins.

### Format des Fichiers
- **Tous les sprites sont en PNG**
- **Transparence alpha preservee**
- **Noms standardises** : `frame_01.png`, `frame_02.png`, etc.

### Performance
- **3,697 sprites au total** → Utiliser un atlas de textures
- Precharger uniquement les sprites necessaires par niveau
- Implementer un cache LRU pour les textures

---

## 🎬 Ordre d'Implementation Optimal

```
✅ Phase 1 (FAIT) : Organisation sprites
⬜ Phase 2 : Billboarding basique (objets statiques)
⬜ Phase 3 : Ennemi simple (Nazi, 1 direction)
⬜ Phase 4 : Systeme 8 rotations
⬜ Phase 5 : Arme FPS (pistol)
⬜ Phase 6 : Animations ennemis (walk, attack, death)
⬜ Phase 7 : Projectiles
⬜ Phase 8 : Effets visuels
⬜ Phase 9 : Z-buffer & tri des sprites
⬜ Phase 10 : Boss Cyberdemon
```

---

## 📚 Ressources

- **Sprites organises** : `v0.4/assets/sprites_organized/`
- **README detaille** : `v0.4/assets/sprites_organized/README.md`
- **Script d'organisation** : `organize_sprites.py` (reutilisable)
- **Textures** : `sprite_selection/texture/` (non touche)
- **Skybox** : `sprite_selection/skybox/` (non touche)

---

**🎮 Bon courage pour l'implementation !**
