#!/usr/bin/env python3
"""
Convertisseur PNG → BMP 64x64 pour DooM-Nukem
Usage: python3 convert_textures.py
"""

from PIL import Image
import os

# Dossier source
SRC_DIR = "../sprite_selection/texture/sol_mur"
# Dossier destination
DEST_DIR = "textures"

# Textures à convertir (nom source → nom destination)
TEXTURES = {
    "rrock01.png": "wall1.bmp",
    "flat1.png": "wall2.bmp",
    "floor0_1.png": "floor1.bmp",
}

def convert_texture(src_path, dest_path, size=(64, 64)):
    """Convertit une PNG en BMP 64x64"""
    try:
        img = Image.open(src_path)
        # Redimensionner en 64x64 (compatible avec vieilles versions PIL)
        img_resized = img.resize(size, Image.LANCZOS)
        # Convertir en RGB (pas d'alpha)
        if img_resized.mode != 'RGB':
            img_resized = img_resized.convert('RGB')
        # Sauvegarder en BMP
        img_resized.save(dest_path, 'BMP')
        print(f"✅ {src_path} → {dest_path}")
        return True
    except Exception as e:
        print(f"❌ Erreur: {e}")
        return False

if __name__ == "__main__":
    print("🎨 Conversion PNG → BMP pour DooM-Nukem\n")
    
    # Créer dossier destination si nécessaire
    os.makedirs(DEST_DIR, exist_ok=True)
    
    success_count = 0
    for src_name, dest_name in TEXTURES.items():
        src_path = os.path.join(SRC_DIR, src_name)
        dest_path = os.path.join(DEST_DIR, dest_name)
        
        if convert_texture(src_path, dest_path):
            success_count += 1
    
    print(f"\n✅ {success_count}/{len(TEXTURES)} textures converties !")
    print(f"📁 Fichiers dans {DEST_DIR}/")
