NAME = doom-nukem
SRC_DIR = srcs
OBJ_DIR = obj
INC_DIR = includes

SRC = main_game.c init_game.c init_sdl.c clear_image.c \
      draw_line.c draw_circle.c draw_rectangle.c bitmap_font.c \
      menu.c game_loop.c frame_limiter.c init_map.c init_player.c \
      handle_input.c render_3d.c render_2d.c minimap.c render_floor_ceiling.c draw_wall_slice.c \
      bmp_parser.c texture.c init_textures.c raycasting_dda.c \
      draw_textured_wall.c skybox.c precompute_skybox.c draw_skybox_column.c map_loader.c sector_utils.c \
      geometry.c collision.c debug.c loader_sectors.c render_sector.c \
      player_movement.c loader_entities.c 

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -O3
LDFLAGS = -lSDL2 -lm

all: $(NAME)
$(NAME): $(OBJ)
	@echo "Linking $(NAME)..."
	@$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "✅ Compilation reussie!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "🧹 Objects nettoyes"

fclean: clean
	@rm -f $(NAME)
	@echo "🧹 Executable supprime"

re: fclean all

.PHONY: all clean fclean re