

SRC = main.c srcs/key.c srcs/collision_vector.c 
OUTPUT = doom-nukem
FLAGS = -Wall -Wextra -Werror
MLX_DIR = ../minilibx-linux
MLX_INC = -I$(MLX_DIR)
MLX_LIB = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm

all:
	gcc $(FLAGS) $(SRC) $(MLX_INC) $(MLX_LIB) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

run:
	./$(OUTPUT)
