# Source files
SRC = main.c srcs/key.c srcs/collision_vector.c srcs/texture.c srcs/rasterize.c srcs/ceiling_render.c
OUTPUT = doom-nukem
FLAGS = -Wall -Wextra -Werror

# OS Detection
UNAME_S := $(shell uname -s)

# Configure MiniLibX based on OS
ifeq ($(UNAME_S),Linux)
    # Linux configuration
    OS_DEF = -D OS_LINUX
    MLX_DIR = ../minilibx-linux
    MLX_INC = -I$(MLX_DIR)
    MLX_LIB = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
    CC = gcc
endif

ifeq ($(UNAME_S),Darwin)
    # macOS configuration
    OS_DEF = -D OS_MAC
    MLX_DIR = ../minilibx_opengl_20191021
    MLX_INC = -I$(MLX_DIR)
    MLX_LIB = -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit -lz
    CC = gcc
endif

all: mlx
	$(CC) $(FLAGS) $(OS_DEF) $(SRC) $(MLX_INC) $(MLX_LIB) -o $(OUTPUT)

mlx:
	@echo "Building MiniLibX for $(UNAME_S)..."
	@make -C $(MLX_DIR)

clean:
	@make -C $(MLX_DIR) clean
	rm -f $(OUTPUT)

fclean: clean
	rm -f $(OUTPUT)

re: fclean all

run:
	./$(OUTPUT)

.PHONY: all mlx clean fclean re run
