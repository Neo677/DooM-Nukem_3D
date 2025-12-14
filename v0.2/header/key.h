#ifndef KEY_H
#define KEY_H

// ----------------- KEY DEFINES -----------------
#ifdef OS_MAC
    // macOS keycodes
    # define W 13
    # define A 0
    # define S 1
    # define D 2
    # define LEFT 123
    # define RIGHT 124
    # define PRESS_E 14
    # define V 9
    # define SPACE 49
    # define ESC 53
    # define F1 122
    # define F2 120
    # define F3 99
    # define F4 118
    # define V 9
    # define LSHIFT 56
#else
    // Linux keycodes (default)
    # define W 119
    # define A 97
    # define S 115
    # define D 100
    # define LEFT 65361
    # define RIGHT 65363
    # define PRESS_E 101
    # define V 118
    # define SPACE 32
    # define ESC 65307
    # define F1 65470
    # define F2 65471
    # define F3 65472
    # define F4 65473
    # define V 118
    # define LSHIFT 65505
#endif

# define SENSI 0.0005f

int ft_key_quit(int keycode, t_render *render);

#endif