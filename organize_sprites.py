#!/usr/bin/env python3
"""
Script d'organisation automatique des sprites DooM-Nukem
Trie tous les sprites sauf les textures, organise par type, animation, et rotation
"""

import os
import shutil
import re
from pathlib import Path
from collections import defaultdict

# Configuration
SOURCE_DIR = "sprite_selection"
OUTPUT_DIR = "v0.4/assets/sprites_organized"

# Categories a ignorer
SKIP_FOLDERS = ["texture", "skybox"]

# Patterns de nommage Doom
DOOM_PATTERN = re.compile(r'^([A-Z]{4})([A-Z])([0-8])(?:\.png)?$')  # ex: CWLKA1.png
WEAPON_PATTERN = re.compile(r'^([A-Z]+)([A-Z]+)(\d+)(?:\.png)?$')  # ex: EMGFIRE2.png

class SpriteOrganizer:
    def __init__(self, source, output):
        self.source = Path(source)
        self.output = Path(output)
        self.stats = defaultdict(int)
        
    def analyze_sprite_name(self, filename):
        """Analyse le nom d'un sprite et retourne ses composants"""
        name = filename.upper()
        
        # Format Doom classique (4 lettres + frame + rotation)
        match = DOOM_PATTERN.match(name)
        if match:
            return {
                'base': match.group(1),      # ex: CWLK
                'frame': match.group(2),     # ex: A
                'rotation': match.group(3),  # ex: 1
                'type': 'doom'
            }
        
        # Format armes (variable)
        match = WEAPON_PATTERN.match(name)
        if match:
            return {
                'base': match.group(1),      # ex: EMGFIRE
                'frame': match.group(2),     # ex: (vide ou lettre)
                'rotation': match.group(3),  # ex: 2
                'type': 'weapon'
            }
        
        return None
    
    def categorize_by_animation(self, base_name):
        """Determine le type d'animation d'apres le nom"""
        animations = {
            'walk': ['WLK', 'WALK', 'MOVE'],
            'idle': ['IDLE', 'STAN', 'STND'],
            'attack': ['FIRE', 'ATTK', 'SHOT', 'ATK', 'FIR'],
            'pain': ['PAIN', 'HIT'],
            'death': ['DEAD', 'DETH', 'DIE', 'XDTH'],
            'raise': ['RAIS', 'RAISE'],
            'see': ['SEE'],
            'melee': ['MELE', 'PUNCH'],
            'reload': ['RELO', 'LOAD'],
            'toss': ['TOSS', 'THROW']
        }
        
        base_upper = base_name.upper()
        for anim_type, keywords in animations.items():
            if any(kw in base_upper for kw in keywords):
                return anim_type
        
        return 'misc'
    
    def get_rotation_name(self, rotation):
        """Convertit le numero de rotation en nom"""
        rotations = {
            '0': 'omnidirectional',
            '1': 'front',
            '2': 'front_right',
            '3': 'right',
            '4': 'back_right',
            '5': 'back',
            '6': 'back_left',
            '7': 'left',
            '8': 'front_left'
        }
        return rotations.get(rotation, f'rot_{rotation}')
    
    def organize_monster(self, category_path, output_path):
        """Organise les sprites d'un monstre"""
        monster_name = category_path.name
        print(f"\n📦 Organisation: {monster_name}")
        
        # Grouper par animation
        animations = defaultdict(lambda: defaultdict(list))
        
        for file in category_path.iterdir():
            if not file.is_file():
                continue
                
            info = self.analyze_sprite_name(file.name)
            if not info:
                # Copier tel quel si pas de pattern reconnu
                dest = output_path / file.name
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(file, dest)
                self.stats['unrecognized'] += 1
                continue
            
            # Determiner le type d'animation
            anim_type = self.categorize_by_animation(info['base'])
            rotation = info['rotation']
            frame = info['frame']
            
            animations[anim_type][rotation].append({
                'file': file,
                'frame': frame,
                'info': info
            })
            self.stats['organized'] += 1
        
        # Creer la structure
        for anim_type, rotations in animations.items():
            for rotation, frames in rotations.items():
                # Trier par frame (A, B, C, ...)
                frames.sort(key=lambda x: x['frame'])
                
                # Creer le dossier
                if rotation == '0':
                    dest_dir = output_path / anim_type
                else:
                    rot_name = self.get_rotation_name(rotation)
                    dest_dir = output_path / anim_type / rot_name
                
                dest_dir.mkdir(parents=True, exist_ok=True)
                
                # Copier les frames
                for i, frame_data in enumerate(frames, 1):
                    file = frame_data['file']
                    # Nom avec numero sequentiel: frame_01.png, frame_02.png
                    ext = file.suffix if file.suffix else '.png'
                    new_name = f"frame_{i:02d}{ext}"
                    dest = dest_dir / new_name
                    shutil.copy2(file, dest)
                    
                print(f"  ✓ {anim_type}/{self.get_rotation_name(rotation)}: {len(frames)} frames")
    
    def organize_weapon(self, category_path, output_path):
        """Organise les sprites d'arme"""
        weapon_name = category_path.name
        print(f"\n🔫 Organisation: {weapon_name}")
        
        animations = defaultdict(list)
        unmatched = []
        
        for file in category_path.iterdir():
            if not file.is_file():
                continue
            
            # Extraire le type d'animation du nom de fichier
            filename_upper = file.stem.upper()
            anim_type = self.categorize_by_animation(filename_upper)
            
            # Extraire le numero de frame
            frame_match = re.search(r'(\d+)$', file.stem)
            frame_num = int(frame_match.group(1)) if frame_match else 0
            
            if anim_type != 'misc':
                animations[anim_type].append({
                    'file': file,
                    'frame': frame_num,
                    'name': file.stem
                })
                self.stats['organized'] += 1
            else:
                # Copier tel quel si non reconnu
                unmatched.append(file)
        
        # Creer la structure
        for anim_type, frames in animations.items():
            # Trier par numero de frame
            frames.sort(key=lambda x: x['frame'])
            
            dest_dir = output_path / anim_type
            dest_dir.mkdir(parents=True, exist_ok=True)
            
            for i, frame_data in enumerate(frames, 1):
                file = frame_data['file']
                ext = file.suffix if file.suffix else '.png'
                new_name = f"frame_{i:02d}{ext}"
                dest = dest_dir / new_name
                shutil.copy2(file, dest)
            
            print(f"  ✓ {anim_type}: {len(frames)} frames")
        
        # Copier les fichiers non tries
        if unmatched:
            misc_dir = output_path / 'misc'
            misc_dir.mkdir(parents=True, exist_ok=True)
            for file in unmatched:
                shutil.copy2(file, misc_dir / file.name)
            print(f"  ℹ️  misc: {len(unmatched)} fichiers")
    
    def organize_simple(self, category_path, output_path):
        """Organise les sprites simples (effets, items, etc.)"""
        print(f"\n📋 Organisation: {category_path.name}")
        
        output_path.mkdir(parents=True, exist_ok=True)
        count = 0
        
        for file in category_path.rglob('*'):
            if file.is_file():
                # Garder la structure relative
                rel_path = file.relative_to(category_path)
                dest = output_path / rel_path
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(file, dest)
                count += 1
                self.stats['simple_copy'] += 1
        
        print(f"  ✓ {count} fichiers copies")
    
    def organize_all(self):
        """Lance l'organisation complete"""
        print("🚀 Debut de l'organisation des sprites")
        print(f"📁 Source: {self.source}")
        print(f"📂 Destination: {self.output}")
        
        # Nettoyer le dossier de destination
        if self.output.exists():
            print("🧹 Nettoyage du dossier de destination...")
            shutil.rmtree(self.output)
        
        self.output.mkdir(parents=True, exist_ok=True)
        
        # Parcourir les categories principales
        for category in self.source.iterdir():
            if not category.is_dir():
                continue
            
            category_name = category.name.lower()
            
            # Ignorer les dossiers specifies
            if category_name in SKIP_FOLDERS:
                print(f"\n⏭️  Ignore: {category.name}")
                continue
            
            # Determiner le type de traitement
            if category_name == 'monstre':
                # Traiter chaque monstre individuellement
                for monster_folder in category.iterdir():
                    if monster_folder.is_dir():
                        output_path = self.output / 'monsters' / monster_folder.name
                        self.organize_monster(monster_folder, output_path)
            
            elif category_name == 'weapon':
                # Traiter chaque arme individuellement
                for weapon_folder in category.iterdir():
                    if weapon_folder.is_dir():
                        output_path = self.output / 'weapons' / weapon_folder.name
                        self.organize_weapon(weapon_folder, output_path)
            
            elif category_name in ['projectile', 'effect']:
                # Organiser par sous-categories
                for subcategory in category.iterdir():
                    if subcategory.is_dir():
                        output_path = self.output / category_name / subcategory.name
                        self.organize_simple(subcategory, output_path)
            
            elif category_name == 'caracter':
                # Organiser les sprites du joueur
                output_path = self.output / 'player'
                self.organize_simple(category, output_path)
            
            elif category_name == 'decorate':
                # Items decoratifs
                output_path = self.output / 'decorations'
                self.organize_simple(category, output_path)
            
            elif category_name == 'ui':
                # Interface
                output_path = self.output / 'ui'
                self.organize_simple(category, output_path)
            
            elif category_name == 'logodoom':
                # Logo
                output_path = self.output / 'logo'
                self.organize_simple(category, output_path)
        
        # Afficher les statistiques
        print("\n" + "="*60)
        print("📊 STATISTIQUES")
        print("="*60)
        print(f"✅ Sprites organises: {self.stats['organized']}")
        print(f"📋 Copies simples: {self.stats['simple_copy']}")
        print(f"❓ Non reconnus: {self.stats['unrecognized']}")
        print(f"📦 Total: {sum(self.stats.values())}")
        print("\n✨ Organisation terminee !")
        print(f"📂 Resultat disponible dans: {self.output}")

def main():
    organizer = SpriteOrganizer(SOURCE_DIR, OUTPUT_DIR)
    organizer.organize_all()

if __name__ == "__main__":
    main()
