#ifndef KEY_H
#define KEY_H

// ----------------- KEY DEFINES -----------------
// Mac
// # define W 13
// # define A 0
// # define S 1
// # define D 2
// # define LEFT 123
// # define RIGHT 124
// # define ESCP 53

# define SENSI 0.0005f

// // Linux
# define W 119
# define A 97
# define S 115
# define D 100
# define LEFT 65361
# define RIGHT 65363
# define PRESS_E 101
# define ESC 65307

int ft_key_quit(int keycode, t_render *render);

#endif