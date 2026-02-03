#ifndef BMP_PARSER_H
# define BMP_PARSER_H

# include <stdint.h>
# include "texture.h"

typedef struct s_bmp_header {
    uint16_t    type;           // "BM"
    uint32_t    size;
    uint32_t    offset;         // Debut des pixels
    uint32_t    dib_header_size;
    int32_t     width;
    int32_t     height;
    uint16_t    bpp;            // Bits per pixel
    uint32_t    compression;
}   t_bmp_header;

// Fonction principale
int load_bmp_texture(const char *filename, t_texture *tex);

#endif
