#include "../header/texture.h"

t_texture_type detectTextureType(const char *filename)
{
    char lower[256];
    int i = 0;
    while (filename[i] && i < 255) {
        lower[i] = tolower(filename[i]);
        i++;
    }
    lower[i] = '\0';

    if (strstr(lower, "ceil") || strstr(lower, "ceiling"))
        return (TEXTURE_CEILING);
    if (strstr(lower, "floor") || strstr(lower, "flat") || strstr(lower, "mflr") || strstr(lower, "sflr"))
        return (TEXTURE_FLOOR);
    if (strstr(lower, "lava") || strstr(lower, "slime") || strstr(lower, "nukage") || 
        strstr(lower, "water") || strstr(lower, "fwater") || strstr(lower, "swater"))
        return (TEXTURE_LIQUID);
    if (strstr(lower, "rrock") || strstr(lower, "aqf") || strstr(lower, "cons") || 
        strstr(lower, "dem") || strstr(lower, "gate") || strstr(lower, "step"))
        return (TEXTURE_WALL);
    return (TEXTURE_WALL);
}

int loadTexture(t_render *render, const char *path, const char *name)
{
    fprintf(stderr, "[TEX] Loading: %s (name: %s)\n", path, name);
    if (global.tex_manager.count >= MAX_TEXTURES) {
        fprintf(stderr, "[TEX] ERROR: Manager full\n");
        return (-1);
    }
    
    int idx = global.tex_manager.count;
    t_texture *tex = &global.tex_manager.textures[idx];
    int width, height;

#ifdef OS_MAC
    tex->img = mlx_png_file_to_image(render->mlx, (char *)path, &width, &height);
    if (!tex->img)
        tex->img = mlx_xpm_file_to_image(render->mlx, (char *)path, &width, &height);
#else
    tex->img = mlx_xpm_file_to_image(render->mlx, (char *)path, &width, &height);
#endif

    if (!tex->img) {
        fprintf(stderr, "[TEX] ERROR: mlx_*_file_to_image failed for %s\n", path);
        return (-1);
    }

    tex->width = width;
    tex->height = height;
    tex->addr = mlx_get_data_addr(tex->img, &tex->bits_per_pixel, &tex->line_len, &tex->endian);
    if (!tex->addr) {
        fprintf(stderr, "[TEX] ERROR: mlx_get_data_addr failed for %s\n", path);
        return (-1);
    }
    tex->loaded = 1;
    tex->type = detectTextureType(path);
    strncpy(tex->name, name ? name : path, 63);
    tex->name[63] = '\0';
    global.tex_manager.count++;
    fprintf(stderr, "[TEX] SUCCESS: Loaded %s as ID %d (%dx%d)\n", name, idx, width, height);
    return (idx);
}

int findTextureByName(const char *name)
{
    fprintf(stderr, "[TEX] Searching for: %s (total loaded: %d)\n", name, global.tex_manager.count);
    for (int i = 0; i < global.tex_manager.count; i++) {
        fprintf(stderr, "[TEX]   Slot %d: %s\n", i, global.tex_manager.textures[i].name);
        if (strstr(global.tex_manager.textures[i].name, name) != NULL) {
            fprintf(stderr, "[TEX] FOUND: %s -> ID %d\n", name, i);
            return (i);
        }
    }
    fprintf(stderr, "[TEX] NOT FOUND: %s\n", name);
    return (-1);
}

int getTexturePixel(int texId, int x, int y)
{
    if (texId < 0 || texId >= global.tex_manager.count)
        return (0);
    t_texture *tex = &global.tex_manager.textures[texId];
    if (!tex->loaded)
        return (0);
    
    x = ((x % tex->width) + tex->width) % tex->width;
    y = ((y % tex->height) + tex->height) % tex->height;
    
    if (x < 0 || x >= tex->width || y < 0 || y >= tex->height)
        return (0);
    
    char *pixel = tex->addr + (y * tex->line_len + x * (tex->bits_per_pixel / 8));
    return (*(unsigned int *)pixel);
}

void initTextureManager(t_render *render)
{
    global.tex_manager.mlx = render->mlx;
    global.tex_manager.count = 0;
    loadTexture(render, "textures/wall.xpm", "wall");
    loadTexture(render, "textures/floor.xpm", "floor");
}

void assignTexturesToPolygons(void)
{
    int wallTextureId = findTextureByName("wall");
    for (int i = 0; i < MAX_POLYS; i++) {
        global.polys[i].textureId = wallTextureId;
    }
    fflush(stdout);
}

