#include "bmp_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parser BMP simplifié - supporte 24bpp et 32bpp
int load_bmp_texture(const char *filename, t_texture *tex)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return -1;
    }
    
    // Lire header (54 bytes)
    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54)
    {
        printf("Erreur: header BMP invalide\n");
        fclose(f);
        return -1;
    }
    
    // Vérifier signature "BM"
    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Erreur: pas un fichier BMP valide\n");
        fclose(f);
        return -1;
    }
    
    // Extraire informations
    int offset = *(int*)&header[10];           // Début des pixels
    tex->width = *(int*)&header[18];            // Largeur
    tex->height = *(int*)&header[22];           // Hauteur
    int bpp = *(short*)&header[28];             // Bits per pixel
    
    printf("Chargement BMP: %s (%dx%d, %dbpp)\n", filename, tex->width, tex->height, bpp);
    
    // Vérifier format supporté
    if (bpp != 24 && bpp != 32)
    {
        printf("Erreur: seulement 24bpp et 32bpp supportés (reçu %dbpp)\n", bpp);
        fclose(f);
        return -1;
    }
    
    // Allouer mémoire pour pixels
    tex->pixels = (Uint32*)malloc(tex->width * tex->height * sizeof(Uint32));
    if (!tex->pixels)
    {
        printf("Erreur: allocation mémoire échouée\n");
        fclose(f);
        return -1;
    }
    
    // Calculer padding (les lignes sont alignées sur 4 bytes)
    int row_size = ((bpp * tex->width + 31) / 32) * 4;
    unsigned char *row = (unsigned char*)malloc(row_size);
    if (!row)
    {
        printf("Erreur: allocation row échouée\n");
        free(tex->pixels);
        fclose(f);
        return -1;
    }
    
    // Positionner au début des pixels
    fseek(f, offset, SEEK_SET);
    
    // Lire les pixels (BMP est stocké de bas en haut)
    for (int y = tex->height - 1; y >= 0; y--)
    {
        if (fread(row, 1, row_size, f) != (size_t)row_size)
        {
            printf("Erreur: lecture pixels échouée\n");
            free(row);
            free(tex->pixels);
            fclose(f);
            return -1;
        }
        
        for (int x = 0; x < tex->width; x++)
        {
            int pixel_idx = y * tex->width + x;
            
            if (bpp == 24)
            {
                // Format BGR
                unsigned char b = row[x * 3 + 0];
                unsigned char g = row[x * 3 + 1];
                unsigned char r = row[x * 3 + 2];
                
                // Convertir en ARGB8888
                tex->pixels[pixel_idx] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
            else  // 32 bpp
            {
                // Format BGRA
                unsigned char b = row[x * 4 + 0];
                unsigned char g = row[x * 4 + 1];
                unsigned char r = row[x * 4 + 2];
                unsigned char a = row[x * 4 + 3];
                
                // Convertir en ARGB8888
                tex->pixels[pixel_idx] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
    
    free(row);
    fclose(f);
    
    printf("✅ BMP chargé avec succès: %s\n", filename);
    return 0;
}
