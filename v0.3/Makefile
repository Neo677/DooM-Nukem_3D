NAME = doom-nukem

OBJDIR = obj

SRCS = main.c
#        map_test_arena.c \
#        srcs/key.c \
#        srcs/collision_vector.c \
#        srcs/texture.c \
#        srcs/rasterize.c \
#        srcs/ceiling_render.c \
#        srcs/draw_utils.c \
#        srcs/ui_overlay.c \
#        srcs/input_handler.c \
#        srcs/player_physics.c \
#        srcs/map_manager.c \
#        srcs/skybox.c \
#        srcs/math_utils.c \
#        srcs/render_engine.c \
#        srcs/game_loop.c

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -D OS_LINUX
INCLUDES = -I../minilibx-linux
LIBS = -L../minilibx-linux -lmlx -lXext -lX11 -lm

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDES) $(LIBS) -o $(NAME)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
