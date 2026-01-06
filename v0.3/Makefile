NAME = doom-nukem

OBJDIR = obj

SRCS = main.c \
       srcs/render.c \
       srcs/maths.c \
       srcs/map.c \
       srcs/collision.c

OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
INCLUDES = -Iheader

# Détection OS
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    # macOS
    MLX_DIR = ../minilibx_opengl_20191021
    MLX_LIB = -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit
else
    # Linux
    MLX_DIR = ../minilibx-linux
    MLX_LIB = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
endif

MLX_INC = -I$(MLX_DIR)

all: $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(MLX_INC) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MLX_LIB) -lm -o $(NAME)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re