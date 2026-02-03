#include "bmp_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_bmp_texture(const char *filename, t_texture *tex)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        return (printf("Erreur: impossible d'ouvrir %s\n", filename), -1);
    
    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54)
        return (printf("Erreur: header BMP invalide\n"), fclose(f), -1);
    
    if (header[0] != 'B' || header[1] != 'M')
        return (printf("Erreur: pas un fichier BMP valide\n"), fclose(f), -1);
    
    int offset = *(int*)&header[10];           // Debut des pixels
    tex->width = *(int*)&header[18];            // Largeur
    tex->height = *(int*)&header[22];           // Hauteur
    int bpp = *(short*)&header[28];             // Bits per pixel
    
    // printf("[DEBUG] Chargement BMP: %s (%dx%d, %dbpp)\n", filename, tex->width, tex->height, bpp);
    // Verifier format supporte
    if (bpp != 24 && bpp != 32)
        return (printf("Erreur: seulement 24bpp et 32bpp\n"), fclose(f), -1);
    tex->pixels = (Uint32*)malloc(tex->width * tex->height * sizeof(Uint32));
    if (!tex->pixels)
        return (printf("Erreur: allocation memoire echouee\n"), fclose(f), -1);
    
    int row_size = ((bpp * tex->width + 31) / 32) * 4;
    unsigned char *row = (unsigned char*)malloc(row_size);
    if (!row)
        return (printf("Erreur: allocation row failed\n"), free(tex->pixels), fclose(f), -1);
    fseek(f, offset, SEEK_SET); // Positionner au debut des pixels
    for (int y = tex->height - 1; y >= 0; y--) // Lire les pixels (BMP est stocke de bas en haut)
    {
        if (fread(row, 1, row_size, f) != (size_t)row_size)
            return (printf("Erreur: lecture pixels failed\n"), free(row), free(tex->pixels), fclose(f), -1);
        for (int x = 0; x < tex->width; x++) {
            int pixel_idx = y * tex->width + x;
            if (bpp == 24) {  // Format BGR
                unsigned char b = row[x * 3 + 0];
                unsigned char g = row[x * 3 + 1];
                unsigned char r = row[x * 3 + 2];
                tex->pixels[pixel_idx] = 0xFF000000 | (r << 16) | (g << 8) | b; // Convertir en ARGB8888
            } else { // 32 bpp 
                unsigned char b = row[x * 4 + 0]; // Format BGRA
                unsigned char g = row[x * 4 + 1];
                unsigned char r = row[x * 4 + 2];
                unsigned char a = row[x * 4 + 3];
                tex->pixels[pixel_idx] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
    return (free(row), fclose(f), printf("BMP sucess: %s\n", filename), 0);
}
