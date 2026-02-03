# Format Sector Map v1.0

## Vue d'ensemble

Ce document decrit le format utilise pour les fichiers de map a base de secteurs (`.dn`).

## Structure Generale

```
# Commentaires
Vertices N
[vertex data]

Sectors N
[sector data]
```

## Section Vertices

```
Vertices N
id x y
id x y
...
```

- `N` : Nombre total de vertices
- `id` : Identifiant unique (0, 1, 2, ...)
- `x, y` : Coordonnees 2D (floats)

**Exemple:**
```
Vertices 4
0 0.0 0.0
1 10.0 0.0
2 10.0 10.0
3 0.0 10.0
```

## Section Sectors

Chaque secteur suit ce format:

```
Sector [id]
nb_vertices
v0 v1 v2 v3 ...
n0 n1 n2 n3 ...
floor_height ceiling_height
floor_texture ceiling_texture
wall_tex0 wall_tex1 wall_tex2 ...
```

### Details des lignes

1. **`Sector [id]`** : Identifiant du secteur
2. **`nb_vertices`** : Nombre de sommets (int)
3. **`v0 v1 v2 ...`** : Indices des vertices (reference a la section Vertices)
4. **`n0 n1 n2 ...`** : Voisins pour chaque mur
   - `-1` = mur solide
   - `≥ 0` = portail vers le secteur N
5. **`floor_height ceiling_height`** : Hauteurs absolues (floats)
6. **`floor_texture ceiling_texture`** : IDs de textures (ints)
7. **`wall_tex0 wall_tex1 ...`** : Textures des murs (un par mur, ints)

### Exemple Complet

```
Sector 0
4
0 1 2 3
-1 1 2 -1
0.0 3.0
1 2
0 0 0 0
```

**Signification:**
- Secteur avec 4 sommets (quadrilatere)
- Utilise les vertices 0, 1, 2, 3
- Mur 0: solide, Mur 1: portail vers secteur 1, Mur 2: portail vers secteur 2, Mur 3: solide
- Sol a hauteur 0.0, plafond a 3.0
- Texture 1 pour le sol, texture 2 pour le plafond
- Tous les murs utilisent la texture 0

## Format Futur (v2.0) - Non Implemente

Une version future pourrait inclure:

```
[apres ceiling_texture]
floor_slope ceiling_slope ref_wall_floor ref_wall_ceiling
```

Pour supporter les pentes (slopes). **Actuellement non utilise dans v1.0.**

## Validation

### Regles Strictes

1. ✅ Tous les secteurs doivent avoir **exactement 7 lignes de donnees**
2. ✅ Le nombre de neighbors = le nombre de vertices
3. ✅ Le nombre de wall_textures = le nombre de vertices
4. ✅ `ceiling_height` doit être `>= floor_height`
5. ✅ Les IDs de vertices doivent exister dans la section Vertices
6. ✅ Les IDs de neighbors doivent être valides (`-1` ou referencer un secteur existant)

### Erreurs Communes

❌ **Ligne ambiguë avec 4 valeurs** (ex: `0.2 0.0 0 0` apres les heights)
- Cause confusion entre slopes et textures
- **Solution:** Utiliser format fixe v1.0 (7 lignes par secteur)

❌ **Nombre incorrect de wall_textures**
- Il faut exactement `nb_vertices` valeurs
- **Solution:** Compter les murs et fournir une texture par mur

❌ **Neighbors invalides**
- Referencer un secteur qui n'existe pas
- **Solution:** Verifier que tous les IDs ≥ 0 sont dans `[0, nb_sectors-1]`

## Outils

### Verification

Pour verifier votre map:
```bash
./doom-nukem
# Le programme affiche les logs de chargement
```

Logs attendus:
```
Loading sectors from: maps/sectors.dn
Loaded 8 vertices
Loaded 3 sectors
Sector map loaded successfully!
```

### Debugging

En cas d'erreur de parsing:
1. Verifier que chaque secteur a **exactement 7 lignes**
2. Compter les valeurs sur chaque ligne
3. Verifier les types (int vs float)
4. Verifier que `0 <= vertex_id < nb_vertices`

## Historique

- **v1.0 (2026-02-03)** : Format simple sans slopes ✅
- **v0.x** : Formats experimentaux avec ambiguïtes ❌
