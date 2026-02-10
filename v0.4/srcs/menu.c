#include "env.h"


typedef enum {
    MENU_MAIN,
    MENU_IN_GAME
} e_menu_state;


typedef struct {
    t_rectangle rect;
    t_point     pos;
    t_point     size;
    const char  *text;
    int         hovered;
} t_button;


static void draw_button(t_env *env, t_button *btn)
{
    
    Uint32 text_color = btn->hovered ? 0xFFFFFFFF : 0xFFCCCCCC;  
    Uint32 border_color = btn->hovered ? 0xFF00FF00 : 0xFF666666;  
    Uint32 bg = btn->hovered ? 0xFF2A2A2A : 0xFF1A1A1A;  
    
    
    t_rectangle r = new_rectangle(bg, border_color, 1, 2);
    
    
    draw_rectangle(env, r, btn->pos, btn->size);
    
    
    int text_len = 0;
    while (btn->text[text_len])
        text_len++;
    
    int text_x = btn->pos.x + (btn->size.x - text_len * 8) / 2;
    int text_y = btn->pos.y + (btn->size.y - 8) / 2;
    
    draw_text(env, btn->text, text_x, text_y, text_color);
}


static int is_button_hovered(t_button *btn, int mouse_x, int mouse_y)
{
    return (mouse_x >= btn->pos.x && mouse_x <= btn->pos.x + btn->size.x &&
            mouse_y >= btn->pos.y && mouse_y <= btn->pos.y + btn->size.y);
}


int show_menu(t_env *env)
{
    
    t_button start_btn = {
        .pos = new_point(env->w / 2 - 100, env->h / 2 - 60),
        .size = new_point(200, 50),
        .text = "START GAME",
        .hovered = 0
    };
    
    t_button quit_btn = {
        .pos = new_point(env->w / 2 - 100, env->h / 2 + 10),
        .size = new_point(200, 50),
        .text = "QUIT",
        .hovered = 0
    };
    
    SDL_Event event;
    int running = 1;
    int start_game = 0;
    int mouse_x, mouse_y;
    
    while (running)
    {
        
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 0;  
            
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                return 0;
            
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (is_button_hovered(&start_btn, mouse_x, mouse_y))
                {
                    start_game = 1;
                    running = 0;
                }
                if (is_button_hovered(&quit_btn, mouse_x, mouse_y))
                    return 0;
            }
        }
        
        
        start_btn.hovered = is_button_hovered(&start_btn, mouse_x, mouse_y);
        quit_btn.hovered = is_button_hovered(&quit_btn, mouse_x, mouse_y);
        
        
        clear_image(env, 0xFF1A1A1A);  
        
        
        draw_text(env, "DOOM", env->w / 2 - 32, 80, 0xFFFF0000);
        draw_text(env, "NUKEM", env->w / 2 - 48, 100, 0xFFFF0000);
        
        
        draw_text(env, "v0.4", env->w / 2 - 16, 130, 0xFF888888);
        
        
        draw_button(env, &start_btn);
        draw_button(env, &quit_btn);
        
        
        SDL_UpdateTexture(env->sdl.texture, NULL, 
                          env->sdl.texture_pixels, 
                          env->w * sizeof(Uint32));
        SDL_RenderCopy(env->sdl.renderer, env->sdl.texture, NULL, NULL);
        SDL_RenderPresent(env->sdl.renderer);
        
        SDL_Delay(16);  
    }
    
    return start_game ? 1 : 0;
}
