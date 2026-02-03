#include "env.h"

// Generer mipmaps (LOD) - downsampling simple
void generate_mipmaps(t_texture *base, t_texture_mipmap *mip)
{
    mip->levels[0] = *base;  // Niveau 0 = texture originale
    mip->num_levels = 1;
    
    for (int level = 1; level < 4; level++)
    {
        int prev_w = mip->levels[level-1].width;
        int prev_h = mip->levels[level-1].height;
        
        if (prev_w < 2 || prev_h < 2)
            break;
        
        int new_w = prev_w / 2;
        int new_h = prev_h / 2;
        
        mip->levels[level].width = new_w;
        mip->levels[level].height = new_h;
        mip->levels[level].pixels = (Uint32*)malloc(new_w * new_h * sizeof(Uint32));
        
        if (!mip->levels[level].pixels)
            break;
        
        // Downsampling - moyenne de 4 pixels
        for (int y = 0; y < new_h; y++)
        {
            for (int x = 0; x < new_w; x++)
            {
                Uint32 p1 = mip->levels[level-1].pixels[(y*2) * prev_w + (x*2)];
                Uint32 p2 = mip->levels[level-1].pixels[(y*2) * prev_w + (x*2+1)];
                Uint32 p3 = mip->levels[level-1].pixels[(y*2+1) * prev_w + (x*2)];
                Uint32 p4 = mip->levels[level-1].pixels[(y*2+1) * prev_w + (x*2+1)];
                
                // Moyenne des composantes RGB
                unsigned char r = (((p1>>16)&0xFF) + ((p2>>16)&0xFF) + 
                                   ((p3>>16)&0xFF) + ((p4>>16)&0xFF)) / 4;
                unsigned char g = (((p1>>8)&0xFF) + ((p2>>8)&0xFF) + 
                                   ((p3>>8)&0xFF) + ((p4>>8)&0xFF)) / 4;
                unsigned char b = ((p1&0xFF) + (p2&0xFF) + 
                                   (p3&0xFF) + (p4&0xFF)) / 4;
                
                mip->levels[level].pixels[y * new_w + x] = 
                    0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }
        
        mip->num_levels++;
    }
}

// Selectionner le bon niveau de mipmap selon distance
t_texture* select_mip_level(t_texture_mipmap *mip, double distance)
{
    int level = 0;
    
    if (distance > 10.0) 
        level = 3;
    else if (distance > 5.0) 
        level = 2;
    else if (distance > 2.0) 
        level = 1;
    
    if (level >= mip->num_levels)
        level = mip->num_levels - 1;
    
    return &mip->levels[level];
}

// Liberer une texture
void free_texture(t_texture *tex)
{
    if (tex->pixels)
    {
        free(tex->pixels);
        tex->pixels = NULL;
    }
}
