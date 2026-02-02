# 🎨 Guide de Conversion de Textures

## Textures Converties ✅

Les textures suivantes ont été converties de PNG → BMP 64x64:

- **wall1.bmp** ← rrock01.png (pierre rouge)
- **wall2.bmp** ← flat1.png (sol beige)
- **floor1.bmp** ← floor0_1.png (sol gris)

## Résultat du Chargement

```
Chargement BMP: textures/wall1.bmp (64x64, 24bpp)
✅ BMP chargé avec succès: textures/wall1.bmp
Chargement BMP: textures/wall2.bmp (64x64, 24bpp)
✅ BMP chargé avec succès: textures/wall2.bmp
✅ Système de textures initialisé (2 textures)
```

## Ajouter Plus de Textures

### Méthode 1 : Script Python

1. Éditer `convert_textures.py` :
```python
TEXTURES = {
    "rrock01.png": "wall1.bmp",
    "flat1.png": "wall2.bmp",
    "floor0_1.png": "wall3.bmp",  # Ajouter
    "grass1.png": "wall4.bmp",     # Ajouter
}
```

2. Relancer :
```bash
python3 convert_textures.py
```

### Méthode 2 : Manuellement

Avec n'importe quel éditeur d'image (GIMP, Photoshop, Paint.NET):
1. Ouvrir une image
2. Redimensionner en 64x64
3. Exporter en BMP 24bpp
4. Sauvegarder dans `textures/wall3.bmp`

### Méthode 3 : ImageMagick (si installé)

```bash
convert sprite_selection/texture/sol_mur/grass1.png -resize 64x64 textures/wall3.bmp
```

## Utiliser les Textures dans la Map

Dans `init_map.c`, les valeurs de la grille correspondent aux textures :

```c
grid[y][x] = 1;  // Utilise wall1.bmp (rrock01 - pierre rouge)
grid[y][x] = 2;  // Utilise wall2.bmp (flat1 - beige)
grid[y][x] = 3;  // Utilise wall3.bmp (si vous l'ajoutez)
```

## Augmenter le Nombre de Textures

Dans `init_textures.c` :

```c
// Ligne 10
env->num_textures = 5;  // Au lieu de 2

// Ajouter plus de chargements :
load_bmp_texture("textures/wall3.bmp", &env->wall_textures[2]);
load_bmp_texture("textures/wall4.bmp", &env->wall_textures[3]);
load_bmp_texture("textures/wall5.bmp", &env->wall_textures[4]);
```

## Textures Disponibles dans sprite_selection

Vous avez **211 textures** dans `sprite_selection/texture/sol_mur/` :

- **Murs** : rrock01-20, flat1-23, floor0-7, etc.
- **Sols** : grass1-2, lava1-4, nukage1-3
- **Plafonds** : ceil1-5
- **Spéciaux** : blood1-3, slime01-16, fwater1-4

Toutes peuvent être converties en BMP avec le script !

## Performances

- Format BMP non compressé = chargement rapide
- 64x64 = taille optimale (cache-friendly)
- Mipmapping disponible mais pas encore activé

---

**Bravo !** Votre moteur charge maintenant des vraies textures ! 🎉
