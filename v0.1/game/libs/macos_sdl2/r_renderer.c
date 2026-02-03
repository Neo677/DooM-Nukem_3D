#include "r_renderer.h"

SDL_Window* window;
SDL_Renderer* sdl_renderer;
SDL_Texture* screen_texture;
unsigned int scrnw, scrnh;
unsigned int *screen_buffer = NULL;
int screen_buffer_size = 0;
sectors_store_t map_sectors = {0};
render_queue_t render_queue = {0};

// Forward declarations
static void R_DrawVerticalWall(rquad_t *quad, unsigned int color);
static void UpdateLookupTables(rquad_t *quad, sector_t *sector);
static void R_RenderFlatsForSector(sector_t *s, player_t *player, double fov, double dist_factor);

static unsigned int R_ApplyFog(unsigned int color, double dist_factor)
{
    dist_factor = fmax(0.0, fmin(1.0, dist_factor));
    
    unsigned char r = (color >> 16) & 0xFF;
    unsigned char g = (color >> 8) & 0xFF;
    unsigned char b = color & 0xFF;

    r = (unsigned char)(r * dist_factor);
    g = (unsigned char)(g * dist_factor);
    b = (unsigned char)(b * dist_factor);

    return (r << 16) | (g << 8) | b;
}

void R_DrawVerticalLine(int x, int y1, int y2, unsigned int color)
{
    if (x < 0 || x >= scrnw)
        return;

    y1 = fmax(0, fmin(y1, scrnh - 1));
    y2 = fmax(0, fmin(y2, scrnh - 1));

    for (int y = y1; y <= y2; y++)
    {
        screen_buffer[scrnw * y + x] = color;
    }
}

static void R_ClipBehindPlayer(double *ax, double *ay, double bx, double by)
{
    double denom = (*ay - by);
    if (fabs(denom) > 0.0001)
    {
        double t = *ay / denom;
        *ax = *ax + t * (bx - *ax);
        *ay = *ay + t * (by - *ay);
    }
}

// Helper functions from r_renderer_utils.c
static void R_DrawVerticalWall(rquad_t *quad, unsigned int color)
{
    if (quad->ax > quad->bx)
        return;  // Mur dos a la camera

    for (int x = quad->ax; x <= quad->bx; x++)
    {
        if (x < 0 || x >= scrnw)
            continue;

        // Interpolation des hauteurs
        float t = (float)(x - quad->ax) / (quad->bx - quad->ax);
        int y1 = quad->at + t * (quad->bt - quad->at);
        int y2 = quad->ab + t * (quad->bb - quad->ab);

        R_DrawVerticalLine(x, y1, y2, color);
    }
}

static void UpdateLookupTables(rquad_t *quad, sector_t *sector)
{
    if (quad->ax > quad->bx)
        return;

    for (int x = quad->ax; x <= quad->bx; x++)
    {
        if (x < 0 || x >= scrnw)
            continue;

        float t = (float)(x - quad->ax) / (quad->bx - quad->ax);
        int ceiling_y = quad->at + t * (quad->bt - quad->at);
        int floor_y = quad->ab + t * (quad->bb - quad->ab);

        // Mise a jour des lookup tables pour le plafond
        if (ceiling_y < sector->ceilx_ylut.t[x] || sector->ceilx_ylut.t[x] == 0)
            sector->ceilx_ylut.t[x] = ceiling_y;
        if (ceiling_y > sector->ceilx_ylut.b[x])
            sector->ceilx_ylut.b[x] = ceiling_y;

        // Mise a jour des lookup tables pour le sol
        if (floor_y < sector->floorx_ylut.t[x] || sector->floorx_ylut.t[x] == 0)
            sector->floorx_ylut.t[x] = floor_y;
        if (floor_y > sector->floorx_ylut.b[x])
            sector->floorx_ylut.b[x] = floor_y;
    }
}

static void R_RenderFlatsForSector(sector_t *s, player_t *player, double fov, double dist_factor)
{
    for (int x = 0; x < scrnw; x++)
    {
        // Rendu du plafond
        int ceil_y1 = s->ceilx_ylut.t[x];
        int ceil_y2 = s->ceilx_ylut.b[x];
        if (ceil_y1 < ceil_y2 && player->z < s->elevation + s->height)
        {
            unsigned int ceil_color = R_ApplyFog(s->ceil_clr, dist_factor);
            R_DrawVerticalLine(x, ceil_y1, ceil_y2, ceil_color);
        }

        // Rendu du sol
        int floor_y1 = s->floorx_ylut.t[x];
        int floor_y2 = s->floorx_ylut.b[x];
        if (floor_y1 < floor_y2 && player->z > s->elevation)
        {
            unsigned int floor_color = R_ApplyFog(s->floor_clr, dist_factor);
            R_DrawVerticalLine(x, floor_y1, floor_y2, floor_color);
        }
    }
}

void R_Init(SDL_Window *main_win, game_state_t *game_state)
{
    window = main_win;
    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl_renderer)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return;
    }
    
    R_InitScreen(game_state->scrn_w, game_state->scrn_h);
    SDL_RenderSetLogicalSize(sdl_renderer, game_state->scrn_w, game_state->scrn_h);
}

void R_InitScreen(int w, int h)
{
    scrnw = w;
    scrnh = h;
    screen_buffer_size = sizeof(unsigned int) * w * h;
    screen_buffer = (unsigned int*)malloc(screen_buffer_size);
    if (screen_buffer == NULL)
    {
        printf("Error initializing screen buffer!\n");
        R_Shutdown();
        return;
    }

    memset(screen_buffer, 0, screen_buffer_size);

    screen_texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        w, h
    );

    if (screen_texture == NULL)
    {
        printf("Error creating screen texture!\n");
        R_Shutdown();
    }
}

void R_Shutdown()
{
    if (screen_texture)
        SDL_DestroyTexture(screen_texture);
    if (screen_buffer)
        free(screen_buffer);
    if (sdl_renderer)
        SDL_DestroyRenderer(sdl_renderer);
}

void R_UpdateScreen()
{
    SDL_UpdateTexture(screen_texture, NULL, screen_buffer, scrnw * sizeof(unsigned int));
    SDL_RenderCopy(sdl_renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
}

void R_ClearScreenBuffer()
{
    memset(screen_buffer, 0, screen_buffer_size);
}



void R_AddSectorToMap(sector_t *sector)
{
    if (map_sectors.num_sectors < 1024)
    {
        map_sectors.sectors[map_sectors.num_sectors++] = *sector;
    }
}

sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int floor_clr, unsigned int ceil_clr)
{
    sector_t s = {0};
    s.height = height;
    s.elevation = elevation;
    s.color = color;
    s.floor_clr = floor_clr;
    s.ceil_clr = ceil_clr;
    s.num_walls = 0;
    return s;
}

wall_t R_CreateWall(int x1, int y1, int x2, int y2)
{
    wall_t w = {0};
    w.a.x = x1; w.a.y = y1;
    w.b.x = x2; w.b.y = y2;
    w.is_portal = false;
    return w;
}

void R_SectorAddWall(sector_t *sector, wall_t wall)
{
    if (sector->num_walls < 64)
    {
        sector->walls[sector->num_walls++] = wall;
    }
}

void R_Render(player_t *player, game_state_t *game_state)
{
    double scrn_half_w = scrnw / 2.0;
    double scrn_half_h = scrnh / 2.0;
    double fov = (scrnw / 2.0) / tan(FOV_ANGLE / 2.0);
    
    R_ClearScreenBuffer();

    // Initialiser la queue
    render_queue.head = 0;
    render_queue.tail = 0;

    // Ajouter le secteur du joueur
    sector_t *start_sector = player->current_sector;
    if (!start_sector) 
    {
        // Fallback: trouver le secteur du joueur si non defini
        // Pour l'instant, on prend le premier secteur ou on cherche
        if (map_sectors.num_sectors > 0) {
             start_sector = &map_sectors.sectors[0]; // Fallback temporaire
             printf("DEBUG: Player sector NULL, using fallback sector %d\n", start_sector->id);
        }
        else {
             printf("DEBUG: No sectors in map!\n");
             return;
        }
    }
    else {
        printf("DEBUG: Starting render at sector %d\n", start_sector->id);
    }

    render_queue.items[render_queue.tail++] = (render_item_t){
        .sector_id = start_sector->id,
        .min_x = 0,
        .max_x = scrnw - 1
    };

    // Traitement de la queue
    while (render_queue.head != render_queue.tail)
    {
        render_item_t item = render_queue.items[render_queue.head++];
        sector_t *current_sector = NULL;
        
        // Trouver le secteur par ID
        for(int i=0; i<map_sectors.num_sectors; i++) {
            if(map_sectors.sectors[i].id == item.sector_id) {
                current_sector = &map_sectors.sectors[i];
                break;
            }
        }
        
        if (!current_sector) continue;

        // Initialiser les lookup tables pour ce secteur si necessaire
        // (Simplification: on reset juste pour le rendu actuel)
        for(int x=0; x<scrnw; x++) {
            current_sector->ceilx_ylut.t[x] = 0; current_sector->ceilx_ylut.b[x] = scrnh;
            current_sector->floorx_ylut.t[x] = 0; current_sector->floorx_ylut.b[x] = scrnh;
        }

        // Pour chaque mur du secteur
        // printf("DEBUG: Processing sector %d with %d walls\n", current_sector->id, current_sector->num_walls);
        for (int i = 0; i < current_sector->num_walls; i++)
        {
            wall_t *wall = &current_sector->walls[i];
            
            // Transformation camera
            double dx1 = wall->a.x - player->position.x;
            double dy1 = wall->a.y - player->position.y;
            double dx2 = wall->b.x - player->position.x;
            double dy2 = wall->b.y - player->position.y;

            // Rotation camera
            double cs = cos(player->dir_angle);
            double sn = sin(player->dir_angle);
            
            double rx1 = dx1 * cs - dy1 * sn;
            double rz1 = dx1 * sn + dy1 * cs;
            double rx2 = dx2 * cs - dy2 * sn;
            double rz2 = dx2 * sn + dy2 * cs;

            // Clipping
            if (rz1 <= NEAR_PLANE && rz2 <= NEAR_PLANE)
                continue;

            if (rz1 <= NEAR_PLANE)
                R_ClipBehindPlayer(&rx1, &rz1, rx2, rz2);
            else if (rz2 <= NEAR_PLANE)
                R_ClipBehindPlayer(&rx2, &rz2, rx1, rz1);

            // Projection perspective
            double sx1 = (rx1 * fov / rz1) + scrn_half_w;
            double sx2 = (rx2 * fov / rz2) + scrn_half_w;
            
            // Clipping horizontal (X)
            if (sx1 >= sx2 || sx2 < item.min_x || sx1 > item.max_x)
                continue;

            // Calcul des hauteurs
            double sy1a = ((current_sector->elevation - player->z) * fov / rz1) + scrn_half_h;
            double sy1b = ((current_sector->elevation + current_sector->height - player->z) * fov / rz1) + scrn_half_h;
            double sy2a = ((current_sector->elevation - player->z) * fov / rz2) + scrn_half_h;
            double sy2b = ((current_sector->elevation + current_sector->height - player->z) * fov / rz2) + scrn_half_h;

            // Clamp X range
            int x1 = fmax(item.min_x, sx1);
            int x2 = fmin(item.max_x, sx2);

            rquad_t quad = {
                .ax = x1, .bx = x2,
                .at = sy1b, .ab = sy1a, // Top/Bottom inverses car Y vers le bas ? Non, Y augmente vers le bas.
                                        // sy1a = sol (bas), sy1b = plafond (haut).
                                        // En SDL Y=0 est en haut. Donc plafond < sol.
                                        // sy1b devrait être plus petit que sy1a si height > 0.
                                        // Verifions: z_ceil > z_floor.
                                        // (z_ceil - pz) / z. Si z_ceil > pz, c'est positif.
                                        // + scrn_half_h.
                                        // Attends, Y screen: 0 = haut, H = bas.
                                        // Un point haut dans le monde (Z grand) doit avoir un Y petit.
                                        // Formule: Y = H/2 - (Z * fov / depth).
                                        // Ici on a: Y = (Z * fov / depth) + H/2.
                                        // C'est l'inverse. Si Z est positif (au dessus des yeux), Y augmente (vers le bas).
                                        // Donc Z positif = bas de l'ecran.
                                        // C'est bizarre.
                                        // Supposons que le code original etait correct.
                                        // sy1a = elevation (sol). sy1b = elevation + height (plafond).
                                        // Si elevation < player.z (sol sous les pieds), (elev - pz) est negatif.
                                        // Donc sy1a < H/2 (vers le haut).
                                        // C'est l'inverse de ce qu'on veut.
                                        // On veut sol en bas (Y grand).
                                        // Donc on doit inverser le signe Z.
                                        // Y = H/2 - (Z * fov / depth).
            };
            
            // Correction de la projection Y
            sy1a = scrn_half_h - ((current_sector->elevation - player->z) * fov / rz1);
            sy1b = scrn_half_h - ((current_sector->elevation + current_sector->height - player->z) * fov / rz1);
            sy2a = scrn_half_h - ((current_sector->elevation - player->z) * fov / rz2);
            sy2b = scrn_half_h - ((current_sector->elevation + current_sector->height - player->z) * fov / rz2);

            quad.at = sy1b; // Plafond (haut, Y petit)
            quad.ab = sy1a; // Sol (bas, Y grand)
            quad.bt = sy2b;
            quad.bb = sy2a;

            // Calcul de la distance pour le brouillard
            double dist = (rz1 + rz2) / 2.0;
            double dist_factor = 1.0 - (dist / FAR_PLANE);

            if (wall->is_portal)
            {
                // Ajouter le secteur voisin a la queue
                if (render_queue.tail < 1024)
                {
                    render_queue.items[render_queue.tail++] = (render_item_t){
                        .sector_id = wall->neighbor_sector_id,
                        .min_x = x1,
                        .max_x = x2
                    };
                }
                
                // Dessiner les parties haut/bas du portail (steps)
                
                // Trouver le secteur voisin pour connaitre ses hauteurs
                sector_t *neighbor = NULL;
                for(int k=0; k<map_sectors.num_sectors; k++) {
                    if(map_sectors.sectors[k].id == wall->neighbor_sector_id) {
                        neighbor = &map_sectors.sectors[k];
                        break;
                    }
                }

                if (neighbor)
                {
                    // Step du bas (si le voisin est plus haut)
                    // On dessine du sol actuel jusqu'au sol du voisin
                    if (neighbor->elevation > current_sector->elevation)
                    {
                        // Calculer Y ecran pour le sol du voisin
                        double neighbor_floor_z = neighbor->elevation;
                        double sy_step_floor = scrn_half_h - ((neighbor_floor_z - player->z) * fov / rz1);
                        double sy_step_floor2 = scrn_half_h - ((neighbor_floor_z - player->z) * fov / rz2);
                        
                        rquad_t step_quad = quad;
                        step_quad.at = sy_step_floor; // Haut du step (sol voisin)
                        step_quad.bt = sy_step_floor2;
                        step_quad.ab = quad.ab;       // Bas du step (sol actuel)
                        step_quad.bb = quad.bb;
                        
                        // Couleur un peu plus sombre pour le step
                        unsigned int step_color = R_ApplyFog(0x505050, dist_factor);
                        R_DrawVerticalWall(&step_quad, step_color);
                        
                        // Mettre a jour le bas du portail pour la suite (on ne voit pas a travers le step)
                        quad.ab = sy_step_floor;
                        quad.bb = sy_step_floor2;
                    }
                    
                    // Step du haut (si le voisin est plus bas de plafond)
                    // On dessine du plafond actuel jusqu'au plafond du voisin
                    double current_ceil_z = current_sector->elevation + current_sector->height;
                    double neighbor_ceil_z = neighbor->elevation + neighbor->height;
                    
                    if (neighbor_ceil_z < current_ceil_z)
                    {
                        // Calculer Y ecran pour le plafond du voisin
                        double sy_step_ceil = scrn_half_h - ((neighbor_ceil_z - player->z) * fov / rz1);
                        double sy_step_ceil2 = scrn_half_h - ((neighbor_ceil_z - player->z) * fov / rz2);
                        
                        rquad_t step_quad = quad;
                        step_quad.at = quad.at;       // Haut du step (plafond actuel)
                        step_quad.bt = quad.bt;
                        step_quad.ab = sy_step_ceil;  // Bas du step (plafond voisin)
                        step_quad.bb = sy_step_ceil2;
                        
                        unsigned int step_color = R_ApplyFog(0x505050, dist_factor);
                        R_DrawVerticalWall(&step_quad, step_color);
                        
                        // Mettre a jour le haut du portail
                        quad.at = sy_step_ceil;
                        quad.bt = sy_step_ceil2;
                    }
                }
            }
            else
            {
                // Mur plein
                unsigned int wall_color = R_ApplyFog(current_sector->color, dist_factor);
                // printf("DEBUG: Drawing wall at x range %d-%d\n", quad.ax, quad.bx);
                R_DrawVerticalWall(&quad, wall_color);
            }
            
            // Mettre a jour les tables pour le sol/plafond
            UpdateLookupTables(&quad, current_sector);
        }
        
        // Rendu des sols et plafonds
        // Note: C'est une simplification, idealement on le fait apres avoir tout dessine ou par scanline
        // Mais pour ce moteur "Doom-like" simple, on peut le faire ici
        double dist_factor = 1.0; // Simplifie pour le sol/plafond
        R_RenderFlatsForSector(current_sector, player, fov, dist_factor);
    }

    R_UpdateScreen();
}