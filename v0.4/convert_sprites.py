#!/usr/bin/env python3
import os
import sys
try:
    from PIL import Image
except ImportError:
    print("Error: PIL (Pillow) library is missing. Please install it with: pip install Pillow")
    sys.exit(1)

# Chemin vers les monstres 
# Adaptez si nécessaire, nous supposons l'exécution depuis la racine du projet v0.4
MONSTERS_DIR = "assets/sprites_organized/monsters"

# Couleur de transparence (Magenta)
COLOR_KEY = (255, 0, 255)

def process_folder(folder_path):
    print(f"Checking {folder_path}...")
    
    # Chercher un fichier candidat
    candidate = None
    
    if not os.path.exists(folder_path):
         print(f"  ❌ Folder does not exist: {folder_path}")
         return False
    
    # Priorité 1: *A1.png (Vue de face frame 1) - ex: POSCA1.png
    # Priorité 2: *A0.png
    # Priorité 3: N'importe quel PNG
    
    files = os.listdir(folder_path)
    png_files = [f for f in files if f.lower().endswith('.png')]
    png_files.sort()
    
    if not png_files:
        print(f"  ⚠️  No PNG files found in {folder_path}")
        return False
        
    # Recherche Prioritaire pour Doom sprites (ex: POSSA1)
    # A1 = Frame A, Angle 1 (angle 1 is usually Front/South-East depending on ref, but usually 1 is Face in Doom internal, wait: 
    # In Doom: 1=East, 2=SouthEast, 3=South, 4=SouthWest, 5=West, 6=NorthWest, 7=North, 8=NorthEast.
    # Ah ! 1 is NOT Front? Front/South is usually 3? 
    # Let's check visually later. But usually A1 is the first angle. 
    # Often for simple sprites, they only have A0 or A1.
    # Let's pick 'A1' or 'PosA1' if exists.
    
    # Let's look for "A1" anywhere in the name as a strong heuristic.
    for f in png_files:
        if "A1.png" in f or "a1.png" in f:
            candidate = f
            break
            
    if not candidate:
        for f in png_files:
            if "A0.png" in f or "a0.png" in f:
                candidate = f
                break
                
    # Fallack: try to find anything with 'A' and '1'
    if not candidate:
        for f in png_files:
            # Check characters at specific positions if strict naming
            if len(f) >= 6 and f[-5] == '1': # ends with 1.png
                 candidate = f
                 break

    if not candidate:
        candidate = png_files[0]
        
    print(f"  Selected sprite: {candidate}")
    
    try:
        # Ouvrir l'image
        src_path = os.path.join(folder_path, candidate)
        dst_path = os.path.join(folder_path, "sprite_sheet.bmp")
        
        with Image.open(src_path) as img:
            # Convertir en RGBA pour gérer la transparence
            img = img.convert("RGBA")
            
            # Créer une image fond Magenta
            bg = Image.new("RGB", img.size, COLOR_KEY)
            
            # Composter l'image sur le fond (utilise l'alpha pour mixer)
            # L'alpha channel est utilisé comme masque.
            # Tout ce qui est transparent (alpha=0) laissera voir le fond Magenta.
            # Tout ce qui est opaque (alpha=255) sera copié.
            # Tout ce qui est semi-transparent sera mélangé (ce qu'on veut éviter pour du mask color key pur, mais bon)
            
            # Pixel art hack: binariser l'alpha pour éviter les bordures roses floues
            # Si alpha < 128 -> 0, sinon 255
            # Non, PIL paste gère ça. Mais faisons simple.
            
            bg.paste(img, mask=img.split()[3]) 
            
            # Sauvegarder en BMP
            bg.save(dst_path)
            print(f"  ✅ Generated {dst_path}")
            return True
            
    except Exception as e:
        print(f"  ❌ Error converting {candidate}: {e}")
        return False

def main():
    if not os.path.isdir(MONSTERS_DIR):
        print(f"Directory {MONSTERS_DIR} not found!")
        # Try relative path check
        print(f"Current dir: {os.getcwd()}")
        return
        
    count = 0
    success = 0
    
    for entry in os.listdir(MONSTERS_DIR):
        full_path = os.path.join(MONSTERS_DIR, entry)
        if os.path.isdir(full_path):
            count += 1
            if process_folder(full_path):
                success += 1
                
    print(f"\nConversion finished: {success}/{count} folders processed.")

if __name__ == "__main__":
    main()
