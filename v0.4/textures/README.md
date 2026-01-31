# Dossier Textures

Ce dossier contient les textures murales au format BMP.

## Format requis

- **Format**: BMP 24bpp ou 32bpp
- **Taille recommandée**: 64x64 pixels (ou puissances de 2: 32x32, 128x128, etc.)
- **Convention de nommage**: `wall1.bmp`, `wall2.bmp`, etc.

## Mapping

Les fichiers de textures correspondent aux types de murs dans la map :

- `wall1.bmp` → wall_type = 1 (murs rouges dans la grid)
- `wall2.bmp` → wall_type = 2 (murs verts dans la grid)
- `wall3.bmp` → wall_type = 3
- etc.

## Si aucune texture n'est trouvée

Le jeu utilisera des couleurs unies par défaut (rouge pour type 1, vert pour type 2).

## Création de textures de test

Vous pouvez utiliser GIMP, Photoshop, ou même MS Paint pour créer des BMPs 64x64.

Exemples de patterns simples :
- Briques
- Pierre
- Métal
- Bois
- Etc.

## Note

Les textures sont chargées au démarrage du jeu. Si vous modifiez une texture, relancez le jeu pour voir les changements.
