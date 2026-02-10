#!/bin/bash

SOURCE_DIR="../sprite_selection/texture/sol_mur"
DEST_DIR="textures"
mkdir -p "$DEST_DIR"

echo "Converting textures from '$SOURCE_DIR' to '$DEST_DIR'..."

count=0
for img in "$SOURCE_DIR"/*.png "$SOURCE_DIR"/*.bmp; do
    [ -e "$img" ] || continue
    
    filename=$(basename "$img")
    name="${filename%.*}"
    dest="$DEST_DIR/$name.bmp"
    
    # Convert to BMP 24-bit (TrueColor)
    # -type TrueColor forces 24-bit output
    # -alpha off ensures no alpha channel (compatibility)
    convert "$img" -type TrueColor -alpha off "$dest"
    
    if [ $? -eq 0 ]; then
        echo "Converted: $filename -> $name.bmp"
        ((count++))
    else
        echo "Error converting: $filename"
    fi
done

echo "Done! Converted $count textures."
