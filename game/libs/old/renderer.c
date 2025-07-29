#include "renderer.h"

#define IS_WALL 0
#define IS_CEIL 1
#define IS_FLOOR 2

#define CEIL_CLR 0x3ac960
#define FLOOR_CLR 0x1a572a

SDL_Window* window;
SDL_Renderer* sdl_renderer;
SDL_Texture* screen_texture;
unsigned int scrnw, scrnh;

bool is_debug_mode = false;
unsigned int *screen_buffer = NULL;
int screen_buffer_size = 0;

sector_queue_t sectors_queue; 

typedef struct r_quad
{
    int ax, bx; // X coordonate of points A & B
    int at, ab; // A Top & B bottom coordinates
    int bt, bb; // B Top & B bottom coordinates
} rquad_t;

void R_ShutdownScreen()
{
    if (screen_texture)
        SDL_DestroyTexture(screen_texture);
    if (screen_buffer != NULL)
        free(screen_buffer);
}

void R_Shutdown()
{
    R_ShutdownScreen();
    SDL_DestroyRenderer(sdl_renderer);
}

void R_updateScreen()
{
    SDL_UpdateTexture(screen_texture, NULL, screen_buffer, scrnw * sizeof(unsigned int));
    SDL_RenderCopy(sdl_renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
}

void R_IniScreen(int w, int h)
{
    screen_buffer_size = sizeof(unsigned int) * w * h;
    screen_buffer = (unsigned int*)malloc(screen_buffer_size);
    if (screen_buffer == NULL) 
    {
        screen_buffer_size = -1;
        printf("Error initializing screen buffer!\n");
        R_Shutdown();
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

void R_Init(SDL_Window *main_win, game_state_t *game_state)
{
    window = main_win;
    scrnw = game_state->scrn_w;
    scrnh = game_state->scrn_h;

    sdl_renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    R_IniScreen(scrnw, scrnh);
    SDL_RenderSetLogicalSize(sdl_renderer, scrnw, scrnh);

}

void R_DrawPoint(int x, int y, unsigned int colors)
{
    bool is_out_of_bounds = (x < 0 || x > scrnw || y < 0 || y >= scrnh);
    bool is_outside_men_buff = (scrnw * y + x) >= (scrnw * scrnh);
    if (is_out_of_bounds || is_outside_men_buff)
        return ;
    screen_buffer[scrnw * y + x] = colors;

}

void R_DrawLine(int x0, int y0, int x1, int y1, unsigned int color)
{
    int dx;
    if (x1 > x0)
        dx = x1 - x0;
    else
        dx = x0 - x1;
    
    int dy;
    if (y1 > y0)
        dy = y1 - y0;
    else
        dy = y0 - y1;
    
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        R_DrawPoint(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }

    if (is_debug_mode)
    {
        R_updateScreen();
        SDL_Delay(10);
    }
}


void R_ClipBehingPlayer(double *ax, double *ay, double bx, double by)
{
    // Proper clipping: interpolate between the two points
    double t = (0.1 - *ay) / (by - *ay);
    *ax = *ax + t * (bx - *ax);
    *ay = 0.1;
}

void R_ClearScreenBuffer()
{
    memset(screen_buffer, 0, sizeof(uint32_t) * scrnw * scrnh);
}

void R_SwapSquadPoint(rquad_t *q)
{
    int t = q->bx;
    q->bx = q->ax;
    q->ax = t;

    t = q->bt;
    q->bt = q->at;
    q->at = t;

    t = q->bb;
    q->bb = q->ab;
    q->ab = t;

}

void R_CalculInterpolationFactor(rquad_t q, double *delta_height, double *delta_elevation)
{
    int width = abs(q.ax - q.bx);
    if (width == 0)
    {
        *delta_height = -1;
        *delta_elevation = -1;
        return;
    }
    int a_height = q.ab - q.at;
    int b_height = q.bb - q.bt;
    
    // calc height increment
    *delta_height = (double)(b_height -  a_height) / (double)width;
    
    // get player's view from the floor
    int y_center_a = (q.ab + q.at) / 2;
    int y_center_b = (q.bb + q.bt) / 2;

    *delta_elevation = (y_center_b - y_center_a) / (double)width;

}

int R_CapToScreenH(int val)
{
    if (val < 0)
        val = 0;
    if (val > scrnw)
        val = scrnw - 1;
    return (val);
}

int R_CapToScreenW(int val)
{
    if (val < 0)
        val = 0;
    if (val > scrnw)
        val = scrnw - 1;
    return (val);
}


void R_Rasterize(rquad_t q, uint32_t color, int ceil_floor_wall, plane_lut_t *xy_lut)
{
    // if backfacing wall then do not rasterize
    if (ceil_floor_wall == IS_WALL)
        return;
    bool is_back_wall = false;
    if ((ceil_floor_wall != IS_WALL) && q.ax > q.bx)
    {
        // swap quad point
        R_SwapSquadPoint(&q);
        is_back_wall = true;
    }

    double delta_height, delta_elevation;

    // calcul interpolation factors 
    R_CalculInterpolationFactor(q, &delta_height, &delta_elevation);
    if (delta_height == -1 && delta_elevation == -1) 
        return;
    for (int x = q.ax, i = 0; x < q.bx; x++, i++)
    {
         if (x < 0 || x > scrnw - 1)
            continue;
        double dh = delta_height * i;
        double dy_player_elevaion = delta_elevation * i;
        
        int y1 = q.at - (dh / 2) + dy_player_elevaion;
        int y2 = q.ab - (dh / 2) + dy_player_elevaion;

        // cap to screen height
        y1 = R_CapToScreenH(y1);
        y2 = R_CapToScreenH(y2);

        if (ceil_floor_wall == IS_CEIL)
        {
            // save the ceilling Y coordinates for each X coordinates
            if (!is_back_wall)
            {
                xy_lut->t[x] = y1;
            }
            else 
            {
                xy_lut->b[x] = y1;
            }
        }
        else if (ceil_floor_wall == IS_FLOOR)
        {
            // save tthje floor's Y coordinates for each X coordinates
            if (!is_back_wall)
            {
                xy_lut->t[x] = y2;
            }
            else 
            {
                xy_lut->b[x] = y2;
            }
        }
        else 
        {
            // rasterize

            R_DrawLine(x, y1, x, y2, color);
        }
    }
}

rquad_t R_CreateRenderableQuad(int ax, int bx, int ab, int at, int bt, int bb)
{
    rquad_t quad = 
    {
        .ax = ax, .bx = bx,
        .at = at, .ab = ab,
        .bt = bt, .bb = bb
    };
    return (quad);
}

void R_RenderSectors(player_t *player, game_state_t *game_state)
{
    double scrn_half_w = scrnw / 2;
    double scrn_half_h = scrnh / 2;
    double fov = 200;

    // clear screen
    R_ClearScreenBuffer();
    
    for (int i = 0; i < sectors_queue.num_sectors; i++)
    {
        sector_t *s = &sectors_queue.sectors[i];
        int sector_h = s->height;

        for (int k = 0; k < s->num_walls; k++)
        {
            wall_t *w = &s->walls[k];
            
            // displace the world based on player's position
            double dx1 = w->a.x - player->position.x;
            double dy1 = w->a.y - player->position.y;
            double dx2 = w->b.x - player->position.x;
            double dy2 = w->b.y - player->position.y;

            // rotate the world around the player
            double SN = sin(player->dir_angles);
            double CN = cos(player->dir_angles);
            double wx1 = dx1 * CN - dy1 * SN;
            double wz1 = dx1 * SN + dy1 * CN;
            double wx2 = dx2 * CN - dy2 * SN;
            double wz2 = dx2 * SN + dy2 * CN;

            // skip walls behind player
            if (wz1 < 0.1 && wz2 < 0.1)
                continue;

            // clip walls behind player
            if (wz1 < 0.1)
            {
                double t = (0.1 - wz1) / (wz2 - wz1);
                wx1 = wx1 + t * (wx2 - wx1);
                wz1 = 0.1;
            }
            if (wz2 < 0.1)
            {
                double t = (0.1 - wz2) / (wz1 - wz2);
                wx2 = wx2 + t * (wx1 - wx2);
                wz2 = 0.1;
            }

            // project to screen space
            double sx1 = (wx1 / wz1) * fov + scrn_half_w;
            double sx2 = (wx2 / wz2) * fov + scrn_half_w;
            
            // calculate wall heights
            double wh1 = (sector_h / wz1) * fov;
            double wh2 = (sector_h / wz2) * fov;
            
            // calculate wall Y positions
            double sy1 = scrn_half_h - (wh1 / 2);
            double sy2 = scrn_half_h - (wh2 / 2);

            // draw wall with sector-specific colors
            unsigned int wall_color = s->color;
            unsigned int ceil_color = s->ceil_clr;
            unsigned int floor_color = s->floor_clr;
            
            // draw ceiling line (top of wall)
            R_DrawLine(sx1, sy1, sx2, sy2, ceil_color);
            
            // draw floor line (bottom of wall)
            R_DrawLine(sx1, sy1 + wh1, sx2, sy2 + wh2, floor_color);
            
            // draw wall edges with wall color
            R_DrawLine(sx1, sy1, sx1, sy1 + wh1, wall_color);
            R_DrawLine(sx2, sy2, sx2, sy2 + wh2, wall_color);
            
            // fill wall area with wall color (optional - creates filled walls)
            for (int y = sy1; y < sy1 + wh1; y++)
            {
                if (y >= 0 && y < scrnh)
                {
                    R_DrawPoint(sx1, y, wall_color);
                    R_DrawPoint(sx2, y, wall_color);
                }
            }
        }
    }
}

void R_Render(player_t *player, game_state_t *game_state)
{
    is_debug_mode = game_state->is_debug_mode;
    // draw walls
    R_RenderSectors(player, game_state);
    
    R_updateScreen();
}

void R_DrawWalls(player_t *player, game_state_t *game_state)
{

}

sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int ceil_clr, unsigned int floor_clr)
{
    static int sector_id = 0;
    sector_t sector = {0};
    sector.num_walls = 0;
    sector.height = height;
    sector.elevation = elevation;
    sector.color = color;
    sector.ceil_clr = ceil_clr;
    sector.floor_clr = floor_clr;
    sector.id = ++sector_id;
    
    return (sector);
}

void R_SectorAddWall(sector_t *sector, wall_t vertices)
{
    sector->walls[sector->num_walls] = vertices;
    sector->num_walls++;
}

void R_AddSectorToQueue(sector_t *sector)
{
    sectors_queue.sectors[sectors_queue.num_sectors] = *sector;
    sectors_queue.num_sectors++;
}

wall_t R_CreateWWall(int ax, int ay, int bx, int by)
{
    wall_t w;
    
    w.a.x = ax;
    w.a.y = ay;
    w.b.x = bx;
    w.b.y = by;
    w.is_portal = false;
    return (w);
}

wall_t R_CreatePortal(int ax, int ay, int bx, int by, int th, int bh)
{
    wall_t w;
    
    w.a.x = ax;
    w.a.y = ay;
    w.b.x = bx;
    w.b.y = by;
    w.is_portal = true;
    w.portal_top_height = th;
    w.portal_bot_height = bh;
    return (w);
}


