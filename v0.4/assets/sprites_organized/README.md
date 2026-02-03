# Sprites Organises - DooM-Nukem 3D

## Structure

```
sprites_organized/
├── monsters/           # Monstres organises par type
│   ├── Cyberdemon/
│   │   ├── walk/      # Animation de marche
│   │   │   ├── front/
│   │   │   ├── front_right/
│   │   │   └── ...    # 8 directions
│   │   ├── attack/
│   │   ├── death/
│   │   └── pain/
│   └── [autres monstres]/
│
├── weapons/            # Armes (HUD first-person)
│   ├── pistol/
│   │   ├── idle/
│   │   ├── attack/
│   │   └── reload/
│   └── [autres armes]/
│
├── projectile/         # Projectiles
│   ├── plasma/
│   ├── rocket/
│   └── bfg/
│
├── effect/             # Effets visuels
│   ├── blood/
│   └── explosion/
│
├── player/             # Sprites du joueur
├── decorations/        # Objets decoratifs
├── ui/                 # Interface utilisateur
└── logo/               # Logos et splash screens
```

## Conventions de Nommage

Chaque sprite est renomme en `frame_XX.png` où XX est le numero sequentiel.

### Rotations (8 directions)

- `front` - Face au joueur (0°)
- `front_right` - 45°
- `right` - 90°
- `back_right` - 135°
- `back` - 180°
- `back_left` - 225°
- `left` - 270°
- `front_left` - 315°

### Animations Communes

**Monstres:**
- `walk` - Marche
- `idle` - Au repos
- `attack` - Attaque
- `pain` - Reaction aux degâts
- `death` - Mort
- `see` - Detection du joueur

**Armes:**
- `idle` - Au repos
- `attack` - Tir
- `reload` - Rechargement
- `raise` - equipement
- `toss` - Lancer

## Utilisation en Code

```c
// Charger une animation de monstre
load_animation("monsters/Cyberdemon/walk/front");

// Charger une arme
load_animation("weapons/pistol/attack");

// Charger un effet
load_sprite("effect/explosion/frame_01.png");
```

## Notes

- Les sprites omnidirectionnels (rotation 0) sont dans le dossier d'animation sans sous-dossier de direction
- Les frames sont triees par ordre alphabetique original (A→Z)
- Format: PNG avec transparence alpha
